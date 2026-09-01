/*
 * FreeModbus Libary: BARE Demo Application
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- Defines ------------------------------------------*/
#define REG_INPUT_START 0x0000 // 输入寄存器起始地址
#define REG_INPUT_NREGS 6      // 输入寄存器数量

/* ----------------------- Static variables ---------------------------------*/
static USHORT usRegInputStart = REG_INPUT_START;
static USHORT usRegInputBuf[REG_INPUT_NREGS];
uint16_t InputBuff[6];

//demo main  
int Main1(void)
{
    eMBErrorCode eStatus;

    /*初始化Modbus协议栈
    参数1： RTU模式（Modbus模式）
    参数2： 本从机地址
    参数3： 所使用的串口，多串口时可以根据这个参数分别初始化使用到的串口
    参数4： 波特率
    参数5： 校验位
    */
    eStatus = eMBInit(MB_RTU, MB_SAMPLE_TEST_SLAVE_ADDR, MB_MASTER_USARTx, MB_MASTER_USART_BAUDRATE, MB_PAR_NONE);
    eStatus = eMBEnable();
    // for (;;;)
    {
        /*从机轮询*/
        eMBPoll();
    }
}
eMBErrorCode
eMBRegInputCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNRegs)
{
    eMBErrorCode eStatus = MB_ENOERR;
    int iRegIndex;
    int i;
    InputBuff[0] = 0x11;
    InputBuff[1] = 0x22;
    InputBuff[2] = 0x33;
    InputBuff[3] = 0x44;
    InputBuff[4] = 0x55;

    if ((usAddress >= REG_INPUT_START) && (usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS))
    {
        iRegIndex = (int)(usAddress - usRegInputStart);
        for (i = 0; i < usNRegs; i++)
        {
            *pucRegBuffer = InputBuff[i + usAddress - 1] >> 8;
            pucRegBuffer++;
            *pucRegBuffer = InputBuff[i + usAddress - 1] & 0xff;
            pucRegBuffer++;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}

eMBErrorCode
eMBRegHoldingCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                eMBRegisterMode eMode)
{
    return MB_ENOREG;
}

eMBErrorCode
eMBRegCoilsCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNCoils,
              eMBRegisterMode eMode)
{
    return MB_ENOREG;
}

eMBErrorCode
eMBRegDiscreteCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNDiscrete)
{
    return MB_ENOREG;
}
