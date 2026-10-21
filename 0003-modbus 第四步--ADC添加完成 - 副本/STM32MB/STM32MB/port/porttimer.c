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
	// 先关闭定时器，修改PSC
	__HAL_TIM_DISABLE(&htim4);

	__HAL_TIM_SET_PRESCALER(&htim4, 3599);

	// 更新预分频影子寄存器，让PSC立刻生效
	htim4.Instance->EGR |= TIM_EGR_UG;
	//    __HAL_TIM_UPDATE_PRESCALER(&htim4);

	// 设置ARR
	__HAL_TIM_SET_AUTORELOAD(&htim4, usTim1Timerout50us - 1);
	// 打开更新中断
	__HAL_TIM_ENABLE_IT(&htim4, TIM_IT_UPDATE);

	return TRUE;
}
// BOOL xMBPortT15Init(USHORT usT15Tick)
// {
// 	// 先关闭定时器，修改PSC
// 	__HAL_TIM_DISABLE(&htim2);

// 	__HAL_TIM_SET_PRESCALER(&htim2, 3599);

// 	// 更新预分频影子寄存器，让PSC立刻生效
// 	htim2.Instance->EGR |= TIM_EGR_UG;
// 	//    __HAL_TIM_UPDATE_PRESCALER(&htim2);
// 	// 设置ARR
// 	__HAL_TIM_SET_AUTORELOAD(&htim2, usT15Tick - 1);
// 	// 打开更新中断
// 	__HAL_TIM_ENABLE_IT(&htim2, TIM_IT_UPDATE);
// 	return TRUE;
// }
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

	// /* Enable the timer with the timeout passed to xMBPortTimersInit( ) */
	// __HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE);
	// __HAL_TIM_ENABLE_IT(&htim2, TIM_IT_UPDATE);
	// __HAL_TIM_SET_COUNTER(&htim2, 0);
	// __HAL_TIM_ENABLE(&htim2);

	/* Enable the timer with the timeout passed to xMBPortTimersInit( ) */
	__HAL_TIM_CLEAR_IT(&htim4, TIM_IT_UPDATE);//清除定时器4更新中断挂起标志 —— 清除中断标志位，防止残留中断触发
	__HAL_TIM_ENABLE_IT(&htim4, TIM_IT_UPDATE);//使能定时器4更新中断 —— 开启溢出/更新中断使能
	__HAL_TIM_SET_COUNTER(&htim4, 0);//将定时器4计数器值清零 —— 重置计数值为0
	__HAL_TIM_ENABLE(&htim4);//使能定时器4 —— 启动定时器运行

	// HAL_TIM_Base_Start_IT(&htim2);
	// HAL_TIM_Base_Start_IT(&htim4);
}

/*
 * 禁用定时器1
 */
inline void
vMBPortTimersDisable() // 禁用定时器1
{
	/* 禁用定时器1 */
	// TIM4->CR1 &= ~(1 << 0); // 禁用定时器1计数器

	// /* Disable any pending timers. */
	// __HAL_TIM_DISABLE(&htim2);
	// __HAL_TIM_SET_COUNTER(&htim2, 0);
	// __HAL_TIM_DISABLE_IT(&htim2, TIM_IT_UPDATE);
	// __HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE);
	/* Disable any pending timers. */
	__HAL_TIM_DISABLE(&htim4);//禁用定时器4 —— 停止定时器运行
	__HAL_TIM_SET_COUNTER(&htim4, 0);//将定时器4计数器值清零 —— 重置计数值为0
	__HAL_TIM_DISABLE_IT(&htim4, TIM_IT_UPDATE);//禁用定时器4更新中断 —— 关闭溢出/更新中断使能
	__HAL_TIM_CLEAR_IT(&htim4, TIM_IT_UPDATE);//清除定时器4更新中断挂起标志 —— 清除中断标志位，防止残留中断触发

	// // 超时后、空闲时关闭定时器
	// HAL_TIM_Base_Stop_IT(&htim2);
	// HAL_TIM_Base_Stop_IT(&htim4);
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
