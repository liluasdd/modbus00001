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
 * 初始化定时器
 * @param usTim1Timerout50us 定时器1的超时时间，单位50us
 */
BOOL xMBPortTimersInit(USHORT usTim1Timerout50us) // 初始化定时器1
{
	// __HAL_RCC_TIM4_CLK_ENABLE();  // 初始化定时器4时钟使能
	// RCC->APB1ENR |= RCC_APB1ENR_TIM4EN; // 初始化定时器4使能位
	RCC->APB1ENR |= (1 << 2);			// 初始化定时器4使能位  //RCC->APB1ENR |= 0x04;
	TIM4->ARR = usTim1Timerout50us - 1; // 初始化定时器4自动重装载寄存器
	TIM4->PSC = 3599;					// 72MHz/3600 = 20k = 50us
	TIM4->DIER |= 1 << 0;				// TIM4->DIER |= TIM_DIER_UIE; // 初始化定时器4更新中断使能位

	// HAL_NVIC_SetPriority(TIM4_IRQn, 0, 0); // 初始化定时器4中断优先级为0

	// NVICInit(1,0,TIM4_IRQn,4);// 初始化定时器4中断优先级为0
	// HAL_NVIC_EnableIRQ(TIM4_IRQn);		   // 初始化定时器4中断

	return TRUE;
}

/*
 * 启用定时器
 * @param usTim1Timerout50us 定时器1的超时时间，单位50us
 */
inline void
vMBPortTimersEnable() // 启用定时器1
{
	/* 启用定时器1 */
	// TIM4->CNT = 0;		 // 初始化定时器1计时寄数器
	// TIM4->CR1 |= 1 << 0; // 启用定时器1计数器
	// TIM4->CR1 |= 1 << 1; // 启用定时器1更新中断使能位

	/* Enable the timer with the timeout passed to xMBPortTimersInit( ) */
	__HAL_TIM_CLEAR_IT(&htim4, TIM_IT_UPDATE);
	__HAL_TIM_ENABLE_IT(&htim4, TIM_IT_UPDATE);
	__HAL_TIM_SET_COUNTER(&htim4, 0);
	__HAL_TIM_ENABLE(&htim4);
}

/*
 * 禁用定时器1
 */
inline void
vMBPortTimersDisable() // 禁用定时器1
{
	/* 禁用定时器1 */
	// TIM4->CR1 &= ~(1 << 0); // 禁用定时器1计数器

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
/*
 * 定时器1中断服务函数
 */
void prvvTIMERExpiredISR(void) // 定时器1中断服务函数
{

	(void)pxMBPortCBTimerExpired();
}

/*
 * 定时器1中断服务函数实现
 */
// void TIM4_IRQHandler(void)
// {
// 	if (TIM4->SR & (1 << 0))
// 	{
// 		// 定时器1中断服务函数实现
// 		prvvTIMERExpiredISR();

// 		TIM4->SR &= ~(1 << 0); // 清除定时器1中断标志位
// 	}
// 	// prvvTIMERExpiredISR();
// }
