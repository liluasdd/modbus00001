/*
 * FreeModbus Libary: A portable Modbus implementation for Modbus ASCII/RTU.
 * Copyright (c) 2006-2018 Christian Walter <cwalter@embedded-solutions.at>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/* ----------------------- System includes ----------------------------------*/
#include "stdlib.h"
#include "string.h"

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"
#include "main.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbrtu.h"
#include "mbframe.h"

#include "mbcrc.h"
#include "mbport.h"

/* ----------------------- Defines ------------------------------------------*/
#define MB_SER_PDU_SIZE_MIN 4   /*!< Minimum size of a Modbus RTU frame. */
#define MB_SER_PDU_SIZE_MAX 256 /*!< Maximum size of a Modbus RTU frame. */
#define MB_SER_PDU_SIZE_CRC 2   /*!< Size of CRC field in PDU. */
#define MB_SER_PDU_ADDR_OFF 0   /*!< Offset of slave address in Ser-PDU. */
#define MB_SER_PDU_PDU_OFF 1    /*!< Offset of Modbus-PDU in Ser-PDU. */

/* ----------------------- Type definitions ---------------------------------*/
typedef enum
{
    STATE_RX_INIT, /*!< Receiver is in initial state. */
    STATE_RX_IDLE, /*!< Receiver is in idle state. */
    STATE_RX_RCV,  /*!< Frame is beeing received. */
    STATE_RX_ERROR /*!< If the frame is invalid. */
} eMBRcvState;

typedef enum
{
    STATE_TX_IDLE, /*!< Transmitter is in idle state. */
    STATE_TX_XMIT  /*!< Transmitter is in transfer state. */
} eMBSndState;

/* ----------------------- Static variables ---------------------------------*/
static volatile eMBSndState eSndState;
static volatile eMBRcvState eRcvState;

volatile UCHAR ucRTUBuf[MB_SER_PDU_SIZE_MAX];

static volatile UCHAR *pucSndBufferCur;
static volatile USHORT usSndBufferCount;

static volatile USHORT usRcvBufferPos;

/* ----------------------- Start implementation -----------------------------*/

// Modbus RTU 初始化
eMBErrorCode
eMBRTUInit(UCHAR ucSlaveAddress, UCHAR ucPort, ULONG ulBaudRate, eMBParity eParity)
{
    eMBErrorCode eStatus = MB_ENOERR;
    ULONG usTimerT35_50us;

    (void)ucSlaveAddress;
    ENTER_CRITICAL_SECTION(); // 进入临界区

    /* Modbus RTU uses 8 Databits. */
    if (xMBPortSerialInit(ucPort, ulBaudRate, 8, eParity) != TRUE)
    {
        eStatus = MB_EPORTERR;
    }
    else
    {
        /* If baudrate > 19200 then we should use the fixed timer values
         * t35 = 1750us. Otherwise t35 must be 3.5 times the character time.
         */
        if (ulBaudRate > 19200)
        {
            usTimerT35_50us = 35; /* 1800us. */
        }
        else
        {
            /* The timer reload value for a character is given by:
             *
             * ChTimeValue = Ticks_per_1s / ( Baudrate / 11 )
             *             = 11 * Ticks_per_1s / Baudrate
             *             = 220000 / Baudrate
             * The reload for t3.5 is 1.5 times this value and similary
             * for t3.5.
             */
            usTimerT35_50us = (7UL * 220000UL) / (2UL * ulBaudRate);
        }
        if (xMBPortTimersInit((USHORT)usTimerT35_50us) != TRUE)
        {
            eStatus = MB_EPORTERR;
        }
    }
    EXIT_CRITICAL_SECTION(); // 退出临界区

    return eStatus;
}

// Modbus RTU 启动
void eMBRTUStart(void)
{
    ENTER_CRITICAL_SECTION(); // 进入临界区
    /* Initially the receiver is in the state STATE_RX_INIT. we start
     * the timer and if no character is received within t3.5 we change
     * to STATE_RX_IDLE. This makes sure that we delay startup of the
     * modbus protocol stack until the bus is free.
     */
    eRcvState = STATE_RX_INIT;        // 接收器初始状态
    vMBPortSerialEnable(TRUE, FALSE); // 启用串口中断接收
    vMBPortTimersEnable();            // 启用定时器

    EXIT_CRITICAL_SECTION(); // 退出临界区
}

// Modbus RTU 停止
void eMBRTUStop(void)
{
    ENTER_CRITICAL_SECTION();
    vMBPortSerialEnable(FALSE, FALSE);
    vMBPortTimersDisable();
    EXIT_CRITICAL_SECTION();
}

// Modbus RTU 接收
/*
参数1：接收地址指针，从机地址
参数2：接收帧指针，指向接收的Modbus RTU帧的起始地址，指向功能码字段
参数3：接收帧长度指针，指向接收的Modbus RTU帧的长度字段
*/
eMBErrorCode
eMBRTUReceive(UCHAR *pucRcvAddress, UCHAR **pucFrame, USHORT *pusLength)
{
    BOOL xFrameReceived = FALSE;
    eMBErrorCode eStatus = MB_ENOERR;

    ENTER_CRITICAL_SECTION();
    assert(usRcvBufferPos < MB_SER_PDU_SIZE_MAX); // 检查接收缓冲区位置是否超出最大大小

    /* Length and CRC check */
    if ((usRcvBufferPos >= MB_SER_PDU_SIZE_MIN) && (usMBCRC16((UCHAR *)ucRTUBuf, usRcvBufferPos) == 0))
    {
        /* Save the address field. All frames are passed to the upper layed
         * and the decision if a frame is used is done there.
         */
        *pucRcvAddress = ucRTUBuf[MB_SER_PDU_ADDR_OFF]; // 从站地址

        /* Total length of Modbus-PDU is Modbus-Serial-Line-PDU minus
         * size of address field and CRC checksum.
         */
        // 总长度 = 接收缓冲区位置 - 功能码字段 - CRC校验字段
        *pusLength = (USHORT)(usRcvBufferPos - MB_SER_PDU_PDU_OFF - MB_SER_PDU_SIZE_CRC);

        /* Return the start of the Modbus PDU to the caller. */
        *pucFrame = (UCHAR *)&ucRTUBuf[MB_SER_PDU_PDU_OFF]; // 功能码
        xFrameReceived = TRUE;
    }
    else
    {
        eStatus = MB_EIO;
    }

    EXIT_CRITICAL_SECTION(); // 退出临界区
    //printf("RTU接收 从机地址： %d 功能码： %d 长度： %d\r\n", ucRTUBuf[MB_SER_PDU_ADDR_OFF], ucRTUBuf[MB_SER_PDU_PDU_OFF], *pusLength);
    return eStatus;
}

// Modbus RTU 发送
/*

*/
eMBErrorCode
eMBRTUSend(UCHAR ucSlaveAddress, const UCHAR *pucFrame, USHORT usLength)
{
    eMBErrorCode eStatus = MB_ENOERR;
    USHORT usCRC16;

    ENTER_CRITICAL_SECTION();

    /* Check if the receiver is still in idle state. If not we where to
     * slow with processing the received frame and the master sent another
     * frame on the network. We have to abort sending the frame.
     */
    if (eRcvState == STATE_RX_IDLE)
    {
        /* First byte before the Modbus-PDU is the slave address. */
        pucSndBufferCur = (UCHAR *)pucFrame - 1;
        usSndBufferCount = 1;

        /* Now copy the Modbus-PDU into the Modbus-Serial-Line-PDU. */
        pucSndBufferCur[MB_SER_PDU_ADDR_OFF] = ucSlaveAddress;
        usSndBufferCount += usLength;

        /* Calculate CRC16 checksum for Modbus-Serial-Line-PDU. */
        usCRC16 = usMBCRC16((UCHAR *)pucSndBufferCur, usSndBufferCount);
        ucRTUBuf[usSndBufferCount++] = (UCHAR)(usCRC16 & 0xFF);
        ucRTUBuf[usSndBufferCount++] = (UCHAR)(usCRC16 >> 8);

        /* Activate the transmitter. */
        eSndState = STATE_TX_XMIT;
        vMBPortSerialEnable(FALSE, TRUE);
    }
    else
    {
        eStatus = MB_EIO;
    }
    EXIT_CRITICAL_SECTION();
    return eStatus;
}

// Modbus从机接收状态机
BOOL xMBRTUReceiveFSM(void)
{
    BOOL xTaskNeedSwitch = FALSE;
    UCHAR ucByte;

    assert(eSndState == STATE_TX_IDLE);

    /* Always read the character. */
    (void)xMBPortSerialGetByte((CHAR *)&ucByte);

    switch (eRcvState) // 接收状态机
    {
        /* If we have received a character in the init state we have to
         * wait until the frame is finished.
         */
    case STATE_RX_INIT:
        vMBPortTimersEnable(); // 启用接收定时器
        break;

        /* In the error state we wait until all characters in the
         * damaged frame are transmitted.
         */
    case STATE_RX_ERROR:
        vMBPortTimersEnable(); // 启用接收定时器
        break;

        /* In the idle state we wait for a new character. If a character
         * is received the t1.5 and t3.5 timers are started and the
         * receiver is in the state STATE_RX_RECEIVCE.
         */
    case STATE_RX_IDLE:
        usRcvBufferPos = 0;
        ucRTUBuf[usRcvBufferPos++] = ucByte; // 存入接收缓冲区
        eRcvState = STATE_RX_RCV;            // 接收状态机切换到接收状态

        /* Enable t3.5 timers. */
        vMBPortTimersEnable(); // 启用接收定时器
        break;

        /* We are currently receiving a frame. Reset the timer after
         * every character received. If more than the maximum possible
         * number of bytes in a modbus frame is received the frame is
         * ignored.
         */
    case STATE_RX_RCV:
        if (usRcvBufferPos < MB_SER_PDU_SIZE_MAX) // 接收缓冲区未满
        {
            ucRTUBuf[usRcvBufferPos++] = ucByte; // 继续存入
        }
        else
        {
            eRcvState = STATE_RX_ERROR; // 接收状态机切换到错误状态
        }
        vMBPortTimersEnable(); // 启用接收定时器
        break;
    }
    return xTaskNeedSwitch; // 是否需要切换任务
}

/*
 *发送状态机
 *核心机制
 *发送中断：当发送缓冲区为空时，触发发送中断
 *协议配合：与 FreeModbus 协议栈的发送空回调函数配合使用
 */
BOOL xMBRTUTransmitFSM(void)
{
    BOOL xNeedPoll = FALSE; // 是否需要轮询

    assert(eRcvState == STATE_RX_IDLE); // 接收状态机必须在空闲状态

    switch (eSndState) // 发送状态机
    {
        /* We should not get a transmitter event if the transmitter is in
         * idle state.  */
    case STATE_TX_IDLE: // 发送空闲状态
        /* enable receiver/disable transmitter. */
        vMBPortSerialEnable(TRUE, FALSE);
        break;

    case STATE_TX_XMIT: // 发送状态
        /* check if we are finished. */
        if (usSndBufferCount != 0) // 发送缓冲区非空
        {
            xMBPortSerialPutByte((CHAR)*pucSndBufferCur); // 发送下一个字节
            pucSndBufferCur++;                            /* next byte in sendbuffer. */
            ;                                             // 指向下一个字节
            usSndBufferCount--;
        }
        else
        {
            xNeedPoll = xMBPortEventPost(EV_FRAME_SENT); // 发送 EV_FRAME_SENT 事件
            /* Disable transmitter. This prevents another transmit buffer
             * empty interrupt. */
            vMBPortSerialEnable(TRUE, FALSE); // 禁用发送中断
            eSndState = STATE_TX_IDLE;        // 发送状态机切换到空闲状态
        }
        break;
    }

    return xNeedPoll;
}

// Modbus从机定时器T35过期状态机，帧（接收结束）完成处理  发送 EV_FRAME_RECEIVED 事件
BOOL xMBRTUTimerT35Expired(void)
{
    BOOL xNeedPoll = FALSE;

    switch (eRcvState)
    {
        /* Timer t35 expired. Startup phase is finished. */
    case STATE_RX_INIT:
        xNeedPoll = xMBPortEventPost(EV_READY);
        break;

        /* A frame was received and t35 expired. Notify the listener that
         * a new frame was received. */
    case STATE_RX_RCV:
        xNeedPoll = xMBPortEventPost(EV_FRAME_RECEIVED); // 发送 EV_FRAME_RECEIVED 事件
        break;

        /* An error occured while receiving the frame. */
    case STATE_RX_ERROR:
        break;

        /* Function called in an illegal state. */
    default:
        assert((eRcvState == STATE_RX_INIT) ||
               (eRcvState == STATE_RX_RCV) || (eRcvState == STATE_RX_ERROR));
    }

    vMBPortTimersDisable();
    eRcvState = STATE_RX_IDLE;

    return xNeedPoll;
}
