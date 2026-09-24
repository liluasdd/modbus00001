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
#include "usertype.h"
#include "gpio.h"

#include "stdlib.h"
#include "string.h"

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbconfig.h"
#include "mbframe.h"
#include "mbproto.h"
#include "mbfunc.h"

#include "mbport.h"
#if MB_RTU_ENABLED == 1
#include "mbrtu.h"
#endif
#if MB_ASCII_ENABLED == 1
#include "mbascii.h"
#endif
#if MB_TCP_ENABLED == 1
#include "mbtcp.h"
#endif

#ifndef MB_PORT_HAS_CLOSE
#define MB_PORT_HAS_CLOSE 0
#endif

/* ----------------------- Static variables ---------------------------------*/

static UCHAR ucMBAddress;
static eMBMode eMBCurrentMode;

/*
 * Modbus ??
 */
static enum {
    STATE_ENABLED,
    STATE_DISABLED,
    STATE_NOT_INITIALIZED
} eMBState = STATE_NOT_INITIALIZED;

/* Functions pointer which are initialized in eMBInit( ). Depending on the
 * mode (RTU or ASCII) the are set to the correct implementations.
 */
static peMBFrameSend peMBFrameSendCur;
static pvMBFrameStart pvMBFrameStartCur;
static pvMBFrameStop pvMBFrameStopCur;
static peMBFrameReceive peMBFrameReceiveCur;
static pvMBFrameClose pvMBFrameCloseCur;

/* Callback functions required by the porting layer. They are called when
 * an external event has happend which includes a timeout or the reception
 * or transmission of a character.
 */
BOOL (*pxMBFrameCBByteReceived)(void);
BOOL (*pxMBFrameCBTransmitterEmpty)(void);
BOOL (*pxMBPortCBTimerExpired)(void);

BOOL (*pxMBFrameCBReceiveFSMCur)(void);
BOOL (*pxMBFrameCBTransmitFSMCur)(void);

/* An array of Modbus functions handlers which associates Modbus function
 * codes with implementing functions.
 */
static xMBFunctionHandler xFuncHandlers[MB_FUNC_HANDLERS_MAX] = {
#if MB_FUNC_OTHER_REP_SLAVEID_ENABLED > 0
    {MB_FUNC_OTHER_REPORT_SLAVEID, eMBFuncReportSlaveID},
#endif
#if MB_FUNC_READ_INPUT_ENABLED > 0
    {MB_FUNC_READ_INPUT_REGISTER, eMBFuncReadInputRegister},
#endif
#if MB_FUNC_READ_HOLDING_ENABLED > 0
    {MB_FUNC_READ_HOLDING_REGISTER, eMBFuncReadHoldingRegister},
#endif
#if MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0
    {MB_FUNC_WRITE_MULTIPLE_REGISTERS, eMBFuncWriteMultipleHoldingRegister},
#endif
#if MB_FUNC_WRITE_HOLDING_ENABLED > 0
    {MB_FUNC_WRITE_REGISTER, eMBFuncWriteHoldingRegister},
#endif
#if MB_FUNC_READWRITE_HOLDING_ENABLED > 0
    {MB_FUNC_READWRITE_MULTIPLE_REGISTERS, eMBFuncReadWriteMultipleHoldingRegister},
#endif
#if MB_FUNC_READ_COILS_ENABLED > 0
    {MB_FUNC_READ_COILS, eMBFuncReadCoils},
#endif
#if MB_FUNC_WRITE_COIL_ENABLED > 0
    {MB_FUNC_WRITE_SINGLE_COIL, eMBFuncWriteCoil},
#endif
#if MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED > 0
    {MB_FUNC_WRITE_MULTIPLE_COILS, eMBFuncWriteMultipleCoils},
#endif
#if MB_FUNC_READ_DISCRETE_INPUTS_ENABLED > 0
    {MB_FUNC_READ_DISCRETE_INPUTS, eMBFuncReadDiscreteInputs},
#endif
};

/* ----------------------- Start implementation -----------------------------*/
/*
 * @brief  初始化Modbus协议栈
 * @param  eMode  Modbus模式
 * @param  ucSlaveAddress 从站地址
 * @param  ucPort 端口号
 * @param  ulBaudRate 波特率
 * @param  eParity 校验位
 * @retval eMBErrorCode 初始化状态
 */
eMBErrorCode
eMBInit(eMBMode eMode, UCHAR ucSlaveAddress, UCHAR ucPort, ULONG ulBaudRate, eMBParity eParity)
{
    eMBErrorCode eStatus = MB_ENOERR; // 初始化状态

    /* check preconditions */
    /* 检查从站地址是否有效 */
    if ((ucSlaveAddress == MB_ADDRESS_BROADCAST) ||
        (ucSlaveAddress < MB_ADDRESS_MIN) || (ucSlaveAddress > MB_ADDRESS_MAX))
    {
        eStatus = MB_EINVAL;
    }
    else // 从站地址有效时
    {
        ucMBAddress = ucSlaveAddress; // 设置从站地址

        switch (eMode) // 选择Modbus模式
        {
#if MB_RTU_ENABLED > 0
        case MB_RTU:
            pvMBFrameStartCur = eMBRTUStart;                             // 启用Modbus协议栈
            pvMBFrameStopCur = eMBRTUStop;                               // 停用Modbus协议栈
            peMBFrameSendCur = eMBRTUSend;                               // 发送Modbus数据
            peMBFrameReceiveCur = eMBRTUReceive;                         // 接收Modbus数据
            pvMBFrameCloseCur = MB_PORT_HAS_CLOSE ? vMBPortClose : NULL; // 关闭Modbus端口
            pxMBFrameCBByteReceived = xMBRTUReceiveFSM;                  // 接收Modbus数据状态机
            pxMBFrameCBTransmitterEmpty = xMBRTUTransmitFSM;             // 发送Modbus数据为空状态机
            pxMBPortCBTimerExpired = xMBRTUTimerT35Expired;              // 定时器超时回调函数

            eStatus = eMBRTUInit(ucMBAddress, ucPort, ulBaudRate, eParity); // 初始化Modbus协议栈
            break;
#endif
#if MB_ASCII_ENABLED > 0
        case MB_ASCII:
            pvMBFrameStartCur = eMBASCIIStart;
            pvMBFrameStopCur = eMBASCIIStop;
            peMBFrameSendCur = eMBASCIISend;
            peMBFrameReceiveCur = eMBASCIIReceive;
            pvMBFrameCloseCur = MB_PORT_HAS_CLOSE ? vMBPortClose : NULL;
            pxMBFrameCBByteReceived = xMBASCIIReceiveFSM;
            pxMBFrameCBTransmitterEmpty = xMBASCIITransmitFSM;
            pxMBPortCBTimerExpired = xMBASCIITimerT1SExpired;

            eStatus = eMBASCIIInit(ucMBAddress, ucPort, ulBaudRate, eParity);
            break;
#endif
        default:
            eStatus = MB_EINVAL;
        }

        if (eStatus == MB_ENOERR)
        {
            if (!xMBPortEventInit())
            {
                /* port dependent event module initalization failed. */
                eStatus = MB_EPORTERR; // 初始化Modbus端口事件模块失败
            }
            else
            {
                eMBCurrentMode = eMode;
                eMBState = STATE_DISABLED; // Modbus协议栈已初始化
            }
        }
    }
    return eStatus;
}

#if MB_TCP_ENABLED > 0
eMBErrorCode
eMBTCPInit(USHORT ucTCPPort)
{
    eMBErrorCode eStatus = MB_ENOERR;

    if ((eStatus = eMBTCPDoInit(ucTCPPort)) != MB_ENOERR)
    {
        eMBState = STATE_DISABLED;
    }
    else if (!xMBPortEventInit())
    {
        /* Port dependent event module initalization failed. */
        eStatus = MB_EPORTERR;
    }
    else
    {
        pvMBFrameStartCur = eMBTCPStart;
        pvMBFrameStopCur = eMBTCPStop;
        peMBFrameReceiveCur = eMBTCPReceive;
        peMBFrameSendCur = eMBTCPSend;
        pvMBFrameCloseCur = MB_PORT_HAS_CLOSE ? vMBTCPPortClose : NULL;
        ucMBAddress = MB_TCP_PSEUDO_ADDRESS;
        eMBCurrentMode = MB_TCP;
        eMBState = STATE_DISABLED;
    }
    return eStatus;
}
#endif

eMBErrorCode
eMBRegisterCB(UCHAR ucFunctionCode, pxMBFunctionHandler pxHandler)
{
    int i;
    eMBErrorCode eStatus;

    if ((0 < ucFunctionCode) && (ucFunctionCode <= 127))
    {
        ENTER_CRITICAL_SECTION();
        if (pxHandler != NULL)
        {
            for (i = 0; i < MB_FUNC_HANDLERS_MAX; i++)
            {
                if ((xFuncHandlers[i].pxHandler == NULL) ||
                    (xFuncHandlers[i].pxHandler == pxHandler))
                {
                    xFuncHandlers[i].ucFunctionCode = ucFunctionCode;
                    xFuncHandlers[i].pxHandler = pxHandler;
                    break;
                }
            }
            eStatus = (i != MB_FUNC_HANDLERS_MAX) ? MB_ENOERR : MB_ENORES;
        }
        else
        {
            for (i = 0; i < MB_FUNC_HANDLERS_MAX; i++)
            {
                if (xFuncHandlers[i].ucFunctionCode == ucFunctionCode)
                {
                    xFuncHandlers[i].ucFunctionCode = 0;
                    xFuncHandlers[i].pxHandler = NULL;
                    break;
                }
            }
            /* Remove can't fail. */
            eStatus = MB_ENOERR;
        }
        EXIT_CRITICAL_SECTION();
    }
    else
    {
        eStatus = MB_EINVAL;
    }
    return eStatus;
}

eMBErrorCode
eMBClose(void)
{
    eMBErrorCode eStatus = MB_ENOERR;

    if (eMBState == STATE_DISABLED)
    {
        if (pvMBFrameCloseCur != NULL)
        {
            pvMBFrameCloseCur();
        }
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
    return eStatus;
}

// 启用Modbus协议栈
eMBErrorCode
eMBEnable(void)
{
    eMBErrorCode eStatus = MB_ENOERR;

    if (eMBState == STATE_DISABLED)
    {
        /* Activate the protocol stack. */
        pvMBFrameStartCur();      // 启用Modbus协议栈
        eMBState = STATE_ENABLED; // 启用Modbus协议栈
    }
    else
    {
        eStatus = MB_EILLSTATE; // Modbus协议栈已启用
    }
    return eStatus;
}

eMBErrorCode
eMBDisable(void)
{
    eMBErrorCode eStatus;

    if (eMBState == STATE_ENABLED)
    {
        pvMBFrameStopCur();
        eMBState = STATE_DISABLED;
        eStatus = MB_ENOERR;
    }
    else if (eMBState == STATE_DISABLED)
    {
        eStatus = MB_ENOERR;
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
    return eStatus;
}

//     eMBRTUReceive()
eMBErrorCode
eMBPoll(void)
{
    static UCHAR *ucMBFrame;        /* Modbus数据帧指针变量 */
    static UCHAR ucRcvAddress;      /* Modbus接收地址变量 */
    static UCHAR ucFunctionCode;    /* Modbus功能码变量 */
    static USHORT usLength;         /* Modbus数据帧长度变量 */
    static eMBException eException; /* Modbus异常变量 */

    int i;
    eMBErrorCode eStatus = MB_ENOERR;
    eMBEventType eEvent;

    /* Check if the protocol stack is ready. */
    if (eMBState != STATE_ENABLED) // Modbus协议栈是否已启用
    {
        return MB_EILLSTATE;
    }

    /* Check if there is a event available. If not return control to caller.
     * Otherwise we will handle the event. */
    if (xMBPortEventGet(&eEvent) == TRUE)
    {
        switch (eEvent)
        {
        case EV_READY:
            break;

        case EV_FRAME_RECEIVED: // Modbus数据帧接收事件
            eStatus = peMBFrameReceiveCur(&ucRcvAddress, &ucMBFrame, &usLength);
            if (eStatus == MB_ENOERR)
            {
                /* Check if the frame is for us. If not ignore the frame. */
                if ((ucRcvAddress == ucMBAddress) || (ucRcvAddress == MB_ADDRESS_BROADCAST))
                {
                    (void)xMBPortEventPost(EV_EXECUTE);
                }
            }
            break;

        case EV_EXECUTE: // Modbus执行事件
            ucFunctionCode = ucMBFrame[MB_PDU_FUNC_OFF];
            eException = MB_EX_ILLEGAL_FUNCTION;
            for (i = 0; i < MB_FUNC_HANDLERS_MAX; i++)
            {
                /* No more function handlers registered. Abort. */
                if (xFuncHandlers[i].ucFunctionCode == 0)
                {
                    break;
                }
                else if (xFuncHandlers[i].ucFunctionCode == ucFunctionCode)
                {
                    eException = xFuncHandlers[i].pxHandler(ucMBFrame, &usLength);
                    break;
                }
            }

            /* If the request was not sent to the broadcast address we
             * return a reply. */
            if (ucRcvAddress != MB_ADDRESS_BROADCAST)
            {
                if (eException != MB_EX_NONE)
                {
                    /* An exception occured. Build an error frame. */
                    usLength = 0;
                    ucMBFrame[usLength++] = (UCHAR)(ucFunctionCode | MB_FUNC_ERROR);
                    ucMBFrame[usLength++] = eException;
                }
                if ((eMBCurrentMode == MB_ASCII) && MB_ASCII_TIMEOUT_WAIT_BEFORE_SEND_MS)
                {
                    vMBPortTimersDelay(MB_ASCII_TIMEOUT_WAIT_BEFORE_SEND_MS);
                }
                eStatus = peMBFrameSendCur(ucMBAddress, ucMBFrame, usLength);
            }
            break;

        case EV_FRAME_SENT:
            break;
        }
    }
    return MB_ENOERR;
}

extern UART_HandleTypeDef huart2;
u8 g_485_send_Error_num = 0; // 4485发送超时故障次数
u8 g_485_receive_Error_num = 0; // 485接收超限故障次数

void eMBsend_Error(void)
{
    //============485发送超时故障处理============
    if (g_b_485_send_Error == 1)
    {
        // 1.强制释放485总线
        RS485_DE_LOW_OFF;
        // g_485_tx_busy = 0;// 485总线释放
        __HAL_UART_DISABLE_IT(&huart2, UART_IT_TC);

        // 2.Modbus协议状态复位，丢弃半帧
        Modbus_ResetRx();
        // 发送标志位清零
        g_b_485_send_tick = 0;
        // 3.??必须把错误标记清零，否则会一直重复进这个分支
        g_b_485_send_Error = 0;
        if (g_485_send_Error_num < 255)
            g_485_send_Error_num++; // 发送超时故障次数加1,最大255次
    }
    else if (g_b_485_receive_Error == 1) //============485接收错误处理============
    {
        // 1.强制释放485总线
        RS485_DE_LOW_OFF;

        // 2.Modbus协议状态复位，丢弃半帧
        Modbus_ResetRx();

        if (g_485_receive_Error_num < 255)
            g_485_receive_Error_num++; // 发送接收超限故障次数加1,最大255次

        // 3.??必须把错误标记清零，否则会一直重复进这个分支
        g_b_485_receive_Error = 0;
    }
}
