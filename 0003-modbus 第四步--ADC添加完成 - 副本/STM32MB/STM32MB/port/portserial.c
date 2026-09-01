/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
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
 * File: $Id$
 */

#include "port.h"
#include "stm32f1xx_hal.h"
#include "usart.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static functions ---------------------------------*/
// static void prvvUARTTxReadyISR( void );
// static void prvvUARTRxISR( void );

/* ----------------------- Start implementation -----------------------------*/
// 初始化串口
BOOL xMBPortSerialInit(UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity)
{
	/**
	 * set 485 mode receive and transmit control IO
	 * @note MODBUS_MASTER_RT_CONTROL_PIN_INDEX need be defined by user
	 */

	/* 使用485时需要在usart.h中打开RT_MODBUS_MASTER_USE_CONTROL_PIN宏定义 */
#if defined(MODBUS_MASTER_USE_CONTROL_PIN)
	modbus_master_control_init();
#endif

	MX_USART2_UART_Init(ucPORT, ulBaudRate, eParity);
	return TRUE;
}
// 使能串口接收中断和发送中断
/*
这个函数通过控制串口中断的使能/禁止，实现 Modbus 协议中 半双工通信的收发切换。
核心机制
中断控制：通过 HAL 库函数控制 USART2 的 RXNE（接收非空）和 TXE（发送空）中断
485 控制：根据收发状态切换 485 芯片的 DE/RE 引脚电平
协议配合：与 FreeModbus 协议栈的帧处理回调函数配合使用
*/
void vMBPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
{
	/* If xRXEnable enable serial receive interrupts. If xTxENable enable
	 * transmitter empty interrupts.
	 */
	if (xRxEnable) // 使能接收中断，
	{
		// // /* 等待接收寄存器完成，确保接收数据完整 */
		// while ((usart2->SR & 0x40) == 0)
		// 	;
		// usart2->CR1 |= (1 << 5);
		// GPIOB->BRR = GPIO_PIN_8; //拉低
		// GPIOA->BRR = GPIO_PIN_4;
		// GPIOA->ODR &= ~(1 << 4); // 拉低为接收态，S485硬件收发脚
		/* 串口2接收中断使能 */
		__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); // 使能接收中断
#if defined(MODBUS_MASTER_USE_CONTROL_PIN)
		/* 485低电平接收 */
		HAL_GPIO_WritePin(MODBUS_MASTER_GPIO_PORT, MODBUS_MASTER_GPIO_PIN, MODBUS_MASTER_GPIO_PIN_LOW);
#endif
	}
	else // 关闭接收中断,使能发送
	{
		// usart2->CR1 &= ~(1 << 5);
		// GPIOB->BSRR = GPIO_PIN_8; //拉高
		// GPIOA->BSRR = GPIO_PIN_4;
		// GPIOA->ODR |= 1 << 4; //拉高为发送态，S485硬件收发脚

		/* 串口2接收中断关闭 */
		__HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);
#if defined(MODBUS_MASTER_USE_CONTROL_PIN)
		/* 485高电平发送 */
		HAL_GPIO_WritePin(MODBUS_MASTER_GPIO_PORT, MODBUS_MASTER_GPIO_PIN, MODBUS_MASTER_GPIO_PIN_HIGH);
#endif
	}
	if (xTxEnable) // 使能发送中断
	{
		// usart2->CR1 |= (1 << 7);
		/* 串口2发送中断使能 */
		__HAL_UART_ENABLE_IT(&huart2, UART_IT_TXE);
#if defined(MODBUS_MASTER_USE_CONTROL_PIN)
		/* 485高电平发送*/
		HAL_GPIO_WritePin(MODBUS_MASTER_GPIO_PORT, MODBUS_MASTER_GPIO_PIN, MODBUS_MASTER_GPIO_PIN_HIGH);
#endif
	}
	else // 关闭发送中断,使能接收
	{
		// usart2->CR1 &= ~(1 << 7);
		/* 串口2发送中断关闭 */
		__HAL_UART_DISABLE_IT(&huart2, UART_IT_TXE);
#if defined(MODBUS_MASTER_USE_CONTROL_PIN)
		/* 485低电平接收*/
		HAL_GPIO_WritePin(MODBUS_MASTER_GPIO_PORT, MODBUS_MASTER_GPIO_PIN, MODBUS_MASTER_GPIO_PIN_LOW);
#endif
	}
}
// 发送字节到串口
BOOL xMBPortSerialPutByte(CHAR ucByte)
{
	/* Put a byte in the UARTs transmit buffer. This function is called
	 * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
	 * called. */
	// huart2->DR = ucByte; //寄存器代码
	if (HAL_UART_Transmit(&huart2, (uint8_t *)&ucByte, 1, 0x01) != HAL_OK) // 添加发送一位代码
		return FALSE;
	else
		return TRUE;
}
// 从串口接收字节
BOOL xMBPortSerialGetByte(CHAR *pucByte)
{
	/* Return the byte in the UARTs receive buffer. This function is called
	 * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
	 */
	// *pucByte = huart2->DR; //寄存器代码
	if (HAL_UART_Receive(&huart2, (uint8_t *)pucByte, 1, 0x01) != HAL_OK) // 添加接收一位代码
		return FALSE;
	else
		return TRUE;
}
/*
 *发送中断服务函数
 *核心机制
 *发送中断：当发送缓冲区为空时，触发发送中断
 *协议配合：与 FreeModbus 协议栈的发送空回调函数配合使用
 */
void prvvUARTTxReadyISR(void) // 删去前面的static，方便在串口中断使用
{
	pxMBFrameCBTransmitterEmpty(); // 发送中断服务函数，调用协议栈的发送空回调函数
}
/*
 *接收中断服务函数
 *核心机制
 *接收中断：当接收缓冲区非空时，触发接收中断
 *协议配合：与 FreeModbus 协议栈的接收回调函数配合使用
 */
void prvvUARTRxISR(void) // 删去前面的static，方便在串口中断使用
{
	pxMBFrameCBByteReceived(); // 接收中断服务函数，调用协议栈的接收回调函数
}

/*
脱偶写至外面了。写在中断位置了 xx_it.c中
*/
// void USART2_IRQHandler(void) // 串口中断服务函数
// {
// 	if (usart2->SR & (1 << 5)) // 接收中断(数据)
// 	{
// 		prvvUARTRxISR(); // 接收完毕，数据处理
// 	}
// 	else if (usart2->SR & (1 << 7)) // 发送中断(空)
// 	{
// 		prvvUARTTxReadyISR(); // 发送中断(空)，发送完成
// 	}
// 	else if (usart2->SR & (1 << 3)) // 清空ORE标志位
// 	{
// 		u8 ore = usart2->DR; // 获取发送数据，清空ORE标志位。
// 	}
// }
