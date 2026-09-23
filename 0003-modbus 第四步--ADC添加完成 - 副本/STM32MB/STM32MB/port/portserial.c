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
// 串口初始化
BOOL xMBPortSerialInit(UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity)
{
	/**
	 * set 485 mode receive and transmit control IO
	 * @note MODBUS_MASTER_RT_CONTROL_PIN_INDEX need be defined by user
	 */

	/* MODBUS_MASTER_USE_CONTROL_PIN定义时初始化485控制引脚 */
#if defined(MODBUS_MASTER_USE_CONTROL_PIN)
	modbus_master_control_init();
#endif

	MX_USART2_UART_Init(ucPORT, ulBaudRate, eParity);
	return TRUE;
}

/*
 * 串口使能接收中断和发送中断服务函数
 * @param xRxEnable 是否使能接收中断
 * @param xTxEnable 是否使能发送中断
 */
void vMBPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
{
	/* If xRXEnable enable serial receive interrupts. If xTxENable enable
	 * transmitter empty interrupts.
	 */
	if (xRxEnable) // 使能接收中断
	{
		// // /* 使能接收中断服务函数 */
		// while ((usart2->SR & 0x40) == 0)
		// 	;
		// usart2->CR1 |= (1 << 5);
		// GPIOB->BRR = GPIO_PIN_8; // 清除接收中断标志位
		// GPIOA->BRR = GPIO_PIN_4;
		// GPIOA->ODR &= ~(1 << 4); // 485Ӳ���շ���//RS485_DE_off
		/* 使能接收中断 */
		__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); // 使能接收中断
#if defined(MODBUS_MASTER_USE_CONTROL_PIN)
		/* 485�͵�ƽ���� */
		HAL_GPIO_WritePin(MODBUS_MASTER_GPIO_PORT, MODBUS_MASTER_GPIO_PIN, MODBUS_MASTER_GPIO_PIN_LOW);
#endif
	}
	else // 禁用接收中断
	{
		// usart2->CR1 &= ~(1 << 5);
		// GPIOB->BSRR = GPIO_PIN_8; // 清除接收中断标志位
		// GPIOA->BSRR = GPIO_PIN_4;
		// GPIOA->ODR |= 1 << 4; //  //RS485_DE_on

		/* 禁用接收中断 */
		__HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);
#if defined(MODBUS_MASTER_USE_CONTROL_PIN)
		/* 485�ߵ�ƽ���� */
		HAL_GPIO_WritePin(MODBUS_MASTER_GPIO_PORT, MODBUS_MASTER_GPIO_PIN, MODBUS_MASTER_GPIO_PIN_HIGH);
#endif
	}
	if (xTxEnable) // 使能发送中断
	{
		// usart2->CR1 |= (1 << 7);
		/* 使能发送中断 */
		__HAL_UART_ENABLE_IT(&huart2, UART_IT_TXE);
#if defined(MODBUS_MASTER_USE_CONTROL_PIN)
		/* 485�ߵ�ƽ����*/
		HAL_GPIO_WritePin(MODBUS_MASTER_GPIO_PORT, MODBUS_MASTER_GPIO_PIN, MODBUS_MASTER_GPIO_PIN_HIGH);
#endif
	}
	else // 禁用发送中断
	{
		// usart2->CR1 &= ~(1 << 7);
		/* 禁用发送中断 */
		__HAL_UART_DISABLE_IT(&huart2, UART_IT_TXE);
#if defined(MODBUS_MASTER_USE_CONTROL_PIN)
		/* 485�͵�ƽ����*/
		HAL_GPIO_WritePin(MODBUS_MASTER_GPIO_PORT, MODBUS_MASTER_GPIO_PIN, MODBUS_MASTER_GPIO_PIN_LOW);
#endif
	}
}
// 发送一个字节到串口2
BOOL xMBPortSerialPutByte(CHAR ucByte)
{
	/* Put a byte in the UARTs transmit buffer. This function is called
	 * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
	 * called. */
	// huart2->DR = ucByte; // 发送一个字节到串口2
	if (HAL_UART_Transmit(&huart2, (uint8_t *)&ucByte, 1, 0x01) != HAL_OK) // 发送一个字节到串口2失败
		return FALSE;
	else
		return TRUE;
}
// 从串口2接收一个字节
BOOL xMBPortSerialGetByte(CHAR *pucByte)
{
	/* Return the byte in the UARTs receive buffer. This function is called
	 * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
	 */
	// *pucByte = huart2->DR; // 从串口2接收一个字节
	if (HAL_UART_Receive(&huart2, (uint8_t *)pucByte, 1, 0x01) != HAL_OK) // 从串口2接收一个字节失败
		return FALSE;
	else
		return TRUE;
}
/*
 * 发送中断服务函数
 * @param ucByte 发送的字节
 */
void prvvUARTTxReadyISR(void) // 发送中断服务函数
{
	pxMBFrameCBTransmitterEmpty(); // 发送中断服务函数回调
}
/*
 * 接收中断服务函数
 * @param ucByte 接收的字节
 */
void prvvUARTRxISR(void) // 接收中断服务函数
{
	pxMBFrameCBByteReceived(); // 接收中断服务函数回调
}

/*
 * 串口2中断服务函数
 * @param ucByte 接收的字节
*/
// void USART2_IRQHandler(void) // 串口2中断服务函数
// {
// 	if (usart2->SR & (1 << 5)) // 接收中断
// 	{
// 		prvvUARTRxISR(); // 接收中断服务函数
// 	}
// 	else if (usart2->SR & (1 << 7)) // 发送中断
// 	{
// 		prvvUARTTxReadyISR(); // 发送中断服务函数
// 	}
// 	else if (usart2->SR & (1 << 3)) // ORRE中断
// 	{
// 		u8 ore = usart2->DR; // 获取ORE值
// 	}
// }
