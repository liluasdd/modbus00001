/*
 * FreeModbus Libary: user callback functions and buffer define in slave mode
 * Copyright (C) 2013 Armink <armink.ztl@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: user_mb_app.c,v 1.60 2013/11/23 11:49:05 Armink $
 */
#include "user_mb_app.h"
#include "main.h"

/*------------------------Slave mode use these variables----------------------*/
// Slave mode:DiscreteInputs variables
USHORT usSDiscInStart = S_DISCRETE_INPUT_START;
#if S_DISCRETE_INPUT_NDISCRETES % 8
UCHAR ucSDiscInBuf[S_DISCRETE_INPUT_NDISCRETES / 8 + 1];
#else
UCHAR ucSDiscInBuf[S_DISCRETE_INPUT_NDISCRETES / 8];
#endif
// Slave mode:Coils variables
USHORT usSCoilStart = S_COIL_START;
#if S_COIL_NCOILS % 8
UCHAR ucSCoilBuf[S_COIL_NCOILS / 8 + 1];
#else
UCHAR ucSCoilBuf[S_COIL_NCOILS / 8] = {0xAA, 0x55, 0x10, 0x09, 0x07, 0x00, 0x00, 0x00}; // 线圈缓冲区（读写）
#endif
// Slave mode:InputRegister variables
USHORT usSRegInStart = S_REG_INPUT_START;                                                         // 输入寄存器起始地址
USHORT usSRegInBuf[S_REG_INPUT_NREGS] = {0x0101, 0x0203, 0x0305, 0x0007, 0x0009, 0x0011, 0x0013}; // 输入寄存器缓冲区
// Slave mode:HoldingRegister variables
USHORT usSRegHoldStart = S_REG_HOLDING_START;                                                        // 保持寄存器起始地址
USHORT usSRegHoldBuf[S_REG_HOLDING_NREGS] = {0x0001, 0x003, 0x0005, 0x0007, 0x0009, 0x0011, 0x0013}; // 保持寄存器缓冲区

/**
 * Modbus slave input register callback function.
 *
 * @param pucRegBuffer input register buffer
 * @param usAddress input register address
 * @param usNRegs input register number
 *
 * @return result
 */
///* 输入寄存器回调函数 */
eMBErrorCode
eMBRegInputCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNRegs)
{
    eMBErrorCode eStatus = MB_ENOERR;
    USHORT iRegIndex;
    USHORT *pusRegInputBuf;
    USHORT REG_INPUT_START;
    USHORT REG_INPUT_NREGS;
    USHORT usRegInStart;

    pusRegInputBuf = usSRegInBuf;
    REG_INPUT_START = S_REG_INPUT_START;
    REG_INPUT_NREGS = S_REG_INPUT_NREGS;
    usRegInStart = usSRegInStart;

    /* it already plus one in modbus function method. */
    usAddress--;

    // 检查寄存器地址是否在有效范围内
    if ((usAddress >= REG_INPUT_START) && (usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS))
    {
        iRegIndex = usAddress - usRegInStart; // 计算寄存器索引偏移量
        while (usNRegs > 0)
        {
            *pucRegBuffer++ = (UCHAR)(pusRegInputBuf[iRegIndex] >> 8);   // 高字节先写
            *pucRegBuffer++ = (UCHAR)(pusRegInputBuf[iRegIndex] & 0xFF); // 低字节后写
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG; // 寄存器地址超出范围
    }
#if DEBUG_UART_ENABLE
    printf("hello %d\r\n", pusRegInputBuf[iRegIndex - 1]); // usSRegInBuf[S_REG_INPUT_NREGS]
#endif
    return eStatus;
}

/**
 * Modbus slave holding register callback function.
 * @param pucRegBuffer holding register buffer
 * @param usAddress holding register address
 * @param usNRegs holding register number
 * @param eMode read or write
 * @return result
 */

/* Modbus保持寄存器回调函数
0x03 读取保持寄存器值      MB_REG_READ   MB_FUNC_READ_HOLDING_ENABLED
0x06 写入保持寄存器值  MB_REG_WRITE  MB_FUNC_WRITE_SINGLE_HOLDING
0x10 写入保持寄存器值  MB_REG_WRITE  MB_FUNC_WRITE_MULTIPLE_HOLDING
 1. 保持寄存器地址usAddress
 2. 保持寄存器数量usNRegs
 3. 操作模式eMode
 4. 保持寄存器缓冲区指针pucRegBuffer
*/
eMBErrorCode eMBRegHoldingCB(UCHAR *pucRegBuffer, USHORT usAddress,
                             USHORT usNRegs, eMBRegisterMode eMode)
{
    eMBErrorCode eStatus = MB_ENOERR;
    USHORT iRegIndex;
    USHORT *pusRegHoldingBuf; // uint16_t * 保持寄存器缓冲区指针
    USHORT REG_HOLDING_START;
    USHORT REG_HOLDING_NREGS;
    USHORT usRegHoldStart;

    pusRegHoldingBuf = usSRegHoldBuf;
    REG_HOLDING_START = S_REG_HOLDING_START;
    REG_HOLDING_NREGS = S_REG_HOLDING_NREGS;
    usRegHoldStart = usSRegHoldStart;

    /* it already plus one in modbus function method. */
    usAddress--;
    if ((usAddress >= REG_HOLDING_START) && (usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS))
    {
        iRegIndex = usAddress - usRegHoldStart;
        switch (eMode)
        {
        /* read current register values from the protocol stack. */
        case MB_REG_READ: // 读取保持寄存器值
            while (usNRegs > 0)
            {
                *pucRegBuffer++ = (UCHAR)(pusRegHoldingBuf[iRegIndex] >> 8);   // 高字节先写
                *pucRegBuffer++ = (UCHAR)(pusRegHoldingBuf[iRegIndex] & 0xFF); // 低字节后写
                iRegIndex++;
                usNRegs--;
            }
            break;

        /* write current register values with new values from the protocol stack. */
        case MB_REG_WRITE: // 写入保持寄存器值
            while (usNRegs > 0)
            {
                pusRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8; // 高字节先写
                pusRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;     // 低字节后写
                iRegIndex++;
                usNRegs--;
            }
            break;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

#if DEBUG_UART_ENABLE
    printf("0x03,0x06,0x10=16. %d\r\n", pusRegHoldingBuf[iRegIndex - 1]);
#endif

     return eStatus;
}

/**
 * Modbus slave coils callback function.
 *
 * @param pucRegBuffer coils buffer
 * @param usAddress coils address
 * @param usNCoils coils number
 * @param eMode read or write
 *
 * @return result
 */
/* 功能码：0x05 线圈寄存器回调函数，用于处理对线圈（Coils）的读写操作 */
eMBErrorCode eMBRegCoilsCB(UCHAR *pucRegBuffer, USHORT usAddress,
                           USHORT usNCoils, eMBRegisterMode eMode)
{
    eMBErrorCode eStatus = MB_ENOERR;
    USHORT iRegIndex, iRegBitIndex, iNReg;
    UCHAR *pucCoilBuf;
    USHORT COIL_START;
    USHORT COIL_NCOILS;
    USHORT usCoilStart;
    iNReg = usNCoils / 8 + 1;

    pucCoilBuf = ucSCoilBuf;
    COIL_START = S_COIL_START;
    COIL_NCOILS = S_COIL_NCOILS;
    usCoilStart = usSCoilStart;

    /* it already plus one in modbus function method. */
    usAddress--;

    if ((usAddress >= COIL_START) &&
        (usAddress + usNCoils <= COIL_START + COIL_NCOILS))
    {
        iRegIndex = (USHORT)(usAddress - usCoilStart) / 8;
        iRegBitIndex = (USHORT)(usAddress - usCoilStart) % 8;
        switch (eMode)
        {
        /* read current coil values from the protocol stack. */
        case MB_REG_READ:
            while (iNReg > 0)
            {
                *pucRegBuffer++ = xMBUtilGetBits(&pucCoilBuf[iRegIndex++],
                                                 iRegBitIndex, 8);
                iNReg--;
            }
            pucRegBuffer--;
            /* last coils */
            usNCoils = usNCoils % 8;
            /* filling zero to high bit */
            *pucRegBuffer = *pucRegBuffer << (8 - usNCoils);
            *pucRegBuffer = *pucRegBuffer >> (8 - usNCoils);
            break;

            /* write current coil values with new values from the protocol stack. */
        case MB_REG_WRITE:
            while (iNReg > 1)
            {
                xMBUtilSetBits(&pucCoilBuf[iRegIndex++], iRegBitIndex, 8,
                               *pucRegBuffer++);
                iNReg--;
            }
            /* last coils */
            usNCoils = usNCoils % 8;
            /* xMBUtilSetBits has bug when ucNBits is zero */
            if (usNCoils != 0)
            {
                xMBUtilSetBits(&pucCoilBuf[iRegIndex++], iRegBitIndex, usNCoils,
                               *pucRegBuffer++);
            }
            break;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

/**
 * Modbus slave discrete callback function.
 *
 * @param pucRegBuffer discrete buffer
 * @param usAddress discrete address
 * @param usNDiscrete discrete number
 *
 * @return result
 */
/* 功能码：0x05 离散输入寄存器回调函数，用于处理对离散输入（Discrete Input）的读操作 */
eMBErrorCode eMBRegDiscreteCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNDiscrete)
{
    eMBErrorCode eStatus = MB_ENOERR;
    USHORT iRegIndex, iRegBitIndex, iNReg;
    UCHAR *pucDiscreteInputBuf;
    USHORT DISCRETE_INPUT_START;
    USHORT DISCRETE_INPUT_NDISCRETES;
    USHORT usDiscreteInputStart;
    iNReg = usNDiscrete / 8 + 1;

    pucDiscreteInputBuf = ucSDiscInBuf;
    DISCRETE_INPUT_START = S_DISCRETE_INPUT_START;
    DISCRETE_INPUT_NDISCRETES = S_DISCRETE_INPUT_NDISCRETES;
    usDiscreteInputStart = usSDiscInStart;

    /* it already plus one in modbus function method. */
    usAddress--;

    if ((usAddress >= DISCRETE_INPUT_START) && (usAddress + usNDiscrete <= DISCRETE_INPUT_START + DISCRETE_INPUT_NDISCRETES))
    {
        iRegIndex = (USHORT)(usAddress - usDiscreteInputStart) / 8;
        iRegBitIndex = (USHORT)(usAddress - usDiscreteInputStart) % 8;

        while (iNReg > 0)
        {
            *pucRegBuffer++ = xMBUtilGetBits(&pucDiscreteInputBuf[iRegIndex++],
                                             iRegBitIndex, 8);
            iNReg--;
        }
        pucRegBuffer--;
        /* last discrete */
        usNDiscrete = usNDiscrete % 8;
        /* filling zero to high bit */
        *pucRegBuffer = *pucRegBuffer << (8 - usNDiscrete);
        *pucRegBuffer = *pucRegBuffer >> (8 - usNDiscrete);
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}
