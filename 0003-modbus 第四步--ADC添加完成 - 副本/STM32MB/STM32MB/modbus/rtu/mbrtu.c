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

// Modbus RTU ��ʼ��
eMBErrorCode
eMBRTUInit(UCHAR ucSlaveAddress, UCHAR ucPort, ULONG ulBaudRate, eMBParity eParity)
{
    eMBErrorCode eStatus = MB_ENOERR;
    ULONG usTimerT35_50us;
    //    ULONG usTimerT15_50us;
    (void)ucSlaveAddress;
    ENTER_CRITICAL_SECTION(); // Modbus协议栈初始化临界区

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
            // /*FreeModbus 公式*/
            // usTimerT35_50us = 35; /* 1800us. */
            /*裸机 公式*/
            //            usTimerT15_50us = 15; // 750us
            usTimerT35_50us = 35; // 1750us
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
            // /*FreeModbus 公式*/
            // usTimerT35_50us = (7UL * 220000UL) / (2UL * ulBaudRate);
            // usTimerT15_50us = (3UL * 220000UL) / (2UL * ulBaudRate);
            /*裸机 公式*/
            usTimerT35_50us = (7UL * 10000000UL) / (2UL * ulBaudRate);
            //            usTimerT15_50us = (3UL * 10000000UL) / (2UL * ulBaudRate);
        }
        if (xMBPortTimersInit((USHORT)usTimerT35_50us) != TRUE)
        {
            eStatus = MB_EPORTERR;
        }
    }
    EXIT_CRITICAL_SECTION(); // Modbus协议栈初始化临界区退出

    return eStatus;
}

// Modbus RTU ����
void eMBRTUStart(void)
{
    ENTER_CRITICAL_SECTION(); // Modbus协议栈启动临界区
    /* Initially the receiver is in the state STATE_RX_INIT. we start
     * the timer and if no character is received within t3.5 we change
     * to STATE_RX_IDLE. This makes sure that we delay startup of the
     * modbus protocol stack until the bus is free.
     */
    eRcvState = STATE_RX_INIT;        // Modbus协议栈启动状态为初始状态
    vMBPortSerialEnable(TRUE, FALSE); // 启用串口接收中断
    vMBPortTimersEnable();            // 启用定时器

    EXIT_CRITICAL_SECTION(); // Modbus协议栈启动临界区退出
}

// Modbus RTU 停止
void eMBRTUStop(void)
{
    ENTER_CRITICAL_SECTION();
    vMBPortSerialEnable(FALSE, FALSE);
    vMBPortTimersDisable();
    EXIT_CRITICAL_SECTION();
}

/* Modbus RTU 接收


*/
eMBErrorCode
eMBRTUReceive(UCHAR *pucRcvAddress, UCHAR **pucFrame, USHORT *pusLength)
{
    BOOL xFrameReceived = FALSE;
    eMBErrorCode eStatus = MB_ENOERR;

    ENTER_CRITICAL_SECTION();
    assert(usRcvBufferPos < MB_SER_PDU_SIZE_MAX); // 检查接收缓冲区是否已满

    /* Length and CRC check */
    if ((usRcvBufferPos >= MB_SER_PDU_SIZE_MIN) && (usMBCRC16((UCHAR *)ucRTUBuf, usRcvBufferPos) == 0))
    {
        /* Save the address field. All frames are passed to the upper layed
         * and the decision if a frame is used is done there.
         */
        *pucRcvAddress = ucRTUBuf[MB_SER_PDU_ADDR_OFF]; // 保存从站地址

        /* Total length of Modbus-PDU is Modbus-Serial-Line-PDU minus
         * size of address field and CRC checksum.
         */
        // 总长度 = 接收缓冲区位置 - 数据偏移量 - CRC长度
        *pusLength = (USHORT)(usRcvBufferPos - MB_SER_PDU_PDU_OFF - MB_SER_PDU_SIZE_CRC);

        /* Return the start of the Modbus PDU to the caller. */
        *pucFrame = (UCHAR *)&ucRTUBuf[MB_SER_PDU_PDU_OFF]; // 返回Modbus PDU的起始地址
        xFrameReceived = TRUE;
    }
    else
    {
        eStatus = MB_EIO;
    }

    EXIT_CRITICAL_SECTION(); // Modbus协议栈接收临界区退出
#if DEBUG_UART_ENABLE
    printf("RTU %d  %d  %d\r\n", ucRTUBuf[MB_SER_PDU_ADDR_OFF], ucRTUBuf[MB_SER_PDU_PDU_OFF], *pusLength);
#endif
    return eStatus;
}

extern u8 g_485_send_tim;
/* Modbus RTU 发送
 * @param ucSlaveAddress 从站地址
 * @param pucFrame Modbus PDU的起始地址
 * @param usLength Modbus PDU的长度
 * @return eMBErrorCode 发送状态
 */
eMBErrorCode
eMBRTUSend(UCHAR ucSlaveAddress, const UCHAR *pucFrame, USHORT usLength)
{
    eMBErrorCode eStatus = MB_ENOERR;
    USHORT usCRC16;

    ENTER_CRITICAL_SECTION(); // Modbus协议栈发送临界区

    /* Check if the receiver is still in idle state. If not we where to
     * slow with processing the received frame and the master sent another
     * frame on the network. We have to abort sending the frame.
     */
    if (eRcvState == STATE_RX_IDLE) // 检查接收状态是否为闲状态
    {
        /* First byte before the Modbus-PDU is the slave address. */
        pucSndBufferCur = (UCHAR *)pucFrame - 1;
        usSndBufferCount = 1; // 发送缓冲区计数器初始化为1，因为第一个字节是从站地址

        /* Now copy the Modbus-PDU into the Modbus-Serial-Line-PDU. */
        pucSndBufferCur[MB_SER_PDU_ADDR_OFF] = ucSlaveAddress; // 复制从站地址到发送缓冲区
        usSndBufferCount += usLength;                          // 发送缓冲区计数器增加，将Modbus PDU的长度添加到计数器中

        /* Calculate CRC16 checksum for Modbus-Serial-Line-PDU. */
        usCRC16 = usMBCRC16((UCHAR *)pucSndBufferCur, usSndBufferCount); // 计算Modbus PDU的CRC16校验和
        ucRTUBuf[usSndBufferCount++] = (UCHAR)(usCRC16 & 0xFF);          // 发送缓冲区计数器增加，将低字节写入缓冲区
        ucRTUBuf[usSndBufferCount++] = (UCHAR)(usCRC16 >> 8);            // 发送缓冲区计数器增加，将高字节写入缓冲区

        /* Activate the transmitter. */
        eSndState = STATE_TX_XMIT;        // 发送状态机状态切换为发送状态
        vMBPortSerialEnable(FALSE, TRUE); // 启用串口发送中断
        //////发送前使能RS485发送使能，并清空发送发送定时器
        RS485_DE_HIGH_ON;      // 使能RS485发送使能
        g_b_485_send_tick = 1; // 发送定时器初始化为1
        g_485_send_tim = 0;    // 发送定时器初始化为0
    }
    else
    {
        eStatus = MB_EIO; // 发送状态机状态切换为错误状态
    }
    EXIT_CRITICAL_SECTION(); // Modbus协议栈发送临界区退出
    return eStatus;
}

// Modbus RTU 接收状态机
BOOL xMBRTUReceiveFSM(void)
{
    BOOL xTaskNeedSwitch = FALSE;
    UCHAR ucByte;

    assert(eSndState == STATE_TX_IDLE);

    /* Always read the character. */
    (void)xMBPortSerialGetByte((CHAR *)&ucByte);

    switch (eRcvState) // Modbus协议栈接收状态机状态
    {
        /* If we have received a character in the init state we have to
         * wait until the frame is finished.
         */
    case STATE_RX_INIT:
        vMBPortTimersEnable(); // 启用定时器
        break;

        /* In the error state we wait until all characters in the
         * damaged frame are transmitted.
         */
    case STATE_RX_ERROR:
        vMBPortTimersEnable();     // 启用定时器
        g_b_485_receive_Error = 1; // 接收错误标志置1
        break;

        /* In the idle state we wait for a new character. If a character
         * is received the t1.5 and t3.5 timers are started and the
         * receiver is in the state STATE_RX_RECEIVCE.
         */
    case STATE_RX_IDLE:
        usRcvBufferPos = 0;
        ucRTUBuf[usRcvBufferPos++] = ucByte; // 接收缓冲区位置增加，将新字符写入缓冲区
        eRcvState = STATE_RX_RCV;            // 接收状态机状态切换为接收状态

        /* Enable t3.5 timers. */
        vMBPortTimersEnable(); // 启用定时器
        break;

        /* We are currently receiving a frame. Reset the timer after
         * every character received. If more than the maximum possible
         * number of bytes in a modbus frame is received the frame is
         * ignored.
         */
    case STATE_RX_RCV:
        if (usRcvBufferPos < MB_SER_PDU_SIZE_MAX) // 接收缓冲区位置小于最大长度
        {
            ucRTUBuf[usRcvBufferPos++] = ucByte; // 接收缓冲区位置增加，将新字符写入缓冲区
        }
        else
        {
            eRcvState = STATE_RX_ERROR; // 接收状态机状态切换为错误状态
        }
        vMBPortTimersEnable(); // 启用定时器
        break;
    }
    return xTaskNeedSwitch; // 是否需要切换任务
}

/*
 *Modbus RTU 发送状态机
 */
BOOL xMBRTUTransmitFSM(void)
{
    BOOL xNeedPoll = FALSE; // 是否需要轮询

    assert(eRcvState == STATE_RX_IDLE); // 接收状态机状态必须为空闲状态

    switch (eSndState) // 发送状态机状态
    {
        /* We should not get a transmitter event if the transmitter is in
         * idle state.  */
    case STATE_TX_IDLE: // 发送状态机状态为空闲状态
        /* enable receiver/disable transmitter. */
        vMBPortSerialEnable(TRUE, FALSE);
        break;

    case STATE_TX_XMIT: // 发送状态机状态为发送状态
        /* check if we are finished. */
        if (usSndBufferCount != 0) // 发送缓冲区计数器不为0
        {
            xMBPortSerialPutByte((CHAR)*pucSndBufferCur); // 发送一个字符
            pucSndBufferCur++;                            /* next byte in sendbuffer. */
            ;                                             // 发送下一个字符
            usSndBufferCount--;
        }
        else
        {
            xNeedPoll = xMBPortEventPost(EV_FRAME_SENT); // 发送完成，发送 EV_FRAME_SENT 事件
            /* Disable transmitter. This prevents another transmit buffer
             * empty interrupt. */
            vMBPortSerialEnable(TRUE, FALSE); // 禁用发送器
            eSndState = STATE_TX_IDLE;        // 发送状态机状态切换为空闲状态
            RS485_DE_LOW_OFF;                 // 使能RS485接收使能
            g_b_485_send_Error = 0;           // 发送错误标志位初始化为0
            g_b_485_send_tick = 0;            // 发送定时器初始化为0
        }
        break;
    }

    return xNeedPoll;
}

// Modbus RTU 接收状态机定时器 T35 事件处理
BOOL xMBRTUTimerT35Expired(void)
{
    BOOL xNeedPoll = FALSE;

    switch (eRcvState)
    {
        /* Timer t35 expired. Startup phase is finished. */
    case STATE_RX_INIT:
        xNeedPoll = xMBPortEventPost(EV_READY); // 发送 EV_READY 事件
        break;

        /* A frame was received and t35 expired. Notify the listener that
         * a new frame was received. */
    case STATE_RX_RCV:
        xNeedPoll = xMBPortEventPost(EV_FRAME_RECEIVED); // 接收完成，发送 EV_FRAME_RECEIVED 事件
        break;

        /* An error occured while receiving the frame. */
    case STATE_RX_ERROR:
        break;

        /* Function called in an illegal state. */
    default:
        assert((eRcvState == STATE_RX_INIT) ||
               (eRcvState == STATE_RX_RCV) || (eRcvState == STATE_RX_ERROR));
    }

    vMBPortTimersDisable();    // 禁用定时器
    eRcvState = STATE_RX_IDLE; // 接收状态机状态切换为空闲状态

    return xNeedPoll; // 是否需要轮询
}

void Modbus_ResetRx(void)
{
    RS485_DE_LOW_OFF;
    ENTER_CRITICAL_SECTION();
    eRcvState = STATE_RX_IDLE;
    usRcvBufferPos = 0;
    vMBPortTimersDisable();
    eSndState = STATE_TX_IDLE;
    usSndBufferCount = 0;
    vMBPortSerialEnable(TRUE, FALSE);
    EXIT_CRITICAL_SECTION();
}
