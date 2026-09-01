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

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"
#include "stm32f1xx_hal.h"
#include "tim.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static functions ---------------------------------*/
// static void prvvTIMERExpiredISR( void );

/* ----------------------- Start implementation -----------------------------*/

/*
 *定时器端口的初始化函数// 初始化定时器
 *核心机制
 *初始化定时器：设置定时器的超时时间
 *协议配合：与 FreeModbus 协议栈的定时器回调函数配合使用
 */
BOOL xMBPortTimersInit(USHORT usTim1Timerout50us) // 定时器初始化直接返回TRUE，已经在mian函数初始化过
{
	// __HAL_RCC_TIM4_CLK_ENABLE();  // 替代直接寄存器操作
	// RCC->APB1ENR |= RCC_APB1ENR_TIM4EN; // 使能定时器4时钟
	RCC->APB1ENR |= (1 << 2);			// 使能定时器4时钟  //RCC->APB1ENR |= 0x04;
	TIM4->ARR = usTim1Timerout50us - 1; // 定时器4的自动重装载寄存器设置为50us超时时间
	TIM4->PSC = 3599;					// 72MHz/3600 = 20k = 50us
	TIM4->DIER |= 1 << 0;				// 使能更新中断使能位 // TIM4->DIER |= TIM_DIER_UIE; // 使能更新中断

	// HAL_NVIC_SetPriority(TIM4_IRQn, 0, 0); // 设置定时器4中断优先级为0,子优先级为0

	// NVICInit(1,0,TIM4_IRQn,4);// 组四，优先级为0，子优先级为0，使能定时器4中断
	/*初始化时不用使能*/
	// HAL_NVIC_EnableIRQ(TIM4_IRQn);		   // 使能定时器4中断

	return TRUE;
}

// 使能定时器中断
inline void
vMBPortTimersEnable() // 使能定时器中断
{
	/* 使能定时器4中断 */
	// TIM4->CNT = 0;		 // 重置定时器4计数器
	// TIM4->CR1 |= 1 << 0; // 使能定时器4计数器
	// TIM4->CR1 |= 1 << 1; // 使能定时器4使能位

	/* Enable the timer with the timeout passed to xMBPortTimersInit( ) */
	__HAL_TIM_CLEAR_IT(&htim4, TIM_IT_UPDATE);
	__HAL_TIM_ENABLE_IT(&htim4, TIM_IT_UPDATE);
	__HAL_TIM_SET_COUNTER(&htim4, 0);
	__HAL_TIM_ENABLE(&htim4);
}

// 取消定时器中断
inline void
vMBPortTimersDisable() // 取消定时器中断
{
	/* 取消定时器4中断 */
	// TIM4->CR1 &= ~(1 << 0); // 取消定时器4计数器使能位

	/* Disable any pending timers. */
	__HAL_TIM_DISABLE(&htim4);
	__HAL_TIM_SET_COUNTER(&htim4, 0);
	__HAL_TIM_DISABLE_IT(&htim4, TIM_IT_UPDATE);
	__HAL_TIM_CLEAR_IT(&htim4, TIM_IT_UPDATE);
}

/* Create an ISR which is called whenever the timer has expired. This function
 * must then call pxMBPortCBTimerExpired( ) to notify the protocol stack that
 * the timer has expired.
 */
// 定时器中断服务函数
// static void
void prvvTIMERExpiredISR(void) // modbus定时器动作，需要在中断内使用
{

	(void)pxMBPortCBTimerExpired();
}

//在中断点C文件里实现
// void TIM4_IRQHandler(void)
// {
// 	if (TIM4->SR & (1 << 0))
// 	{
// 		// 调用定时器中断服务函数,通知协议栈 3.5字符等待时间已到，超时
// 		prvvTIMERExpiredISR();

// 		TIM4->SR &= ~(1 << 0); // 清除更新中断标志位
// 	}
// 	// prvvTIMERExpiredISR();
// }
