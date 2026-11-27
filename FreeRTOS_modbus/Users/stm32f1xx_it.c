#if 1
/**
 ******************************************************************************
 * @file    GPIO/GPIO_IOToggle/Src/stm32f1xx_it.c
 * @author  MCD Application Team
 * @version V1.5.0
 * @date    14-April-2017
 * @brief   Main Interrupt Service Routines.
 *          This file provides template for all exceptions handler and
 *          peripherals interrupt service routine.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_it.h"
#include "FreeRTOS.h"
#include "task.h"

/** @addtogroup STM32F1xx_HAL_Examples
 * @{
 */

/** @addtogroup GPIO_IOToggle
 * @{
 */
// extern volatile USHORT usRcvBufferPos;
// extern volatile UCHAR ucRTUBuf[256];
// extern inline void vMBPortTimersEnable(); // 启用定时器1

extern TIM_HandleTypeDef htim4;
extern UART_HandleTypeDef huart2;
/* USER CODE BEGIN EV */

// volatile u8 s_tim0_10ms = 0;
u8 s_tim_20ms = 0;
// u16 s_tim_100ms = 1;
u8 s_tim_100ms = 1;
u8 s_tim_500ms = 1;
u8 s_tim_2s = 0;
volatile u8 g_485_send_tim = 0;

extern ADC_HandleTypeDef adc_handle; // ADC句柄

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
 * @brief  This function handles NMI exception.
 * @param  None
 * @retval None
 */
void NMI_Handler(void)
{
}

/**
 * @brief  This function handles Hard Fault exception.
 * @param  None
 * @retval None
 */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
 * @brief  This function handles Memory Manage exception.
 * @param  None
 * @retval None
 */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
 * @brief  This function handles Bus Fault exception.
 * @param  None
 * @retval None
 */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
 * @brief  This function handles Usage Fault exception.
 * @param  None
 * @retval None
 */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

// /**
//  * @brief  This function handles SVCall exception.
//  * @param  None
//  * @retval None
//  */
// void SVC_Handler(void)
// {
// }

/**
 * @brief  This function handles Debug Monitor exception.
 * @param  None
 * @retval None
 */
void DebugMon_Handler(void)
{
}

// /**
//  * @brief  This function handles PendSVC exception.
//  * @param  None
//  * @retval None
//  */
// void PendSV_Handler(void)
// {
// }

/**
 * @brief  This function handles SysTick Handler.
 * @param  None
 * @retval None
 */
void SysTick_Handler(void)
{
  HAL_IncTick();
  if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) // 只在调度器起来后才跑
  {
    xPortSysTickHandler(); // FreeRTOS 内核的 xTickCount + 触发 PendSV
  }
}

// 声明 dma_handle 是在其他文件定义的全局变量
extern DMA_HandleTypeDef hdma_adc1;
void DMA1_Channel1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_adc1); // ??:??? HAL ??????
}

void TIM2_IRQHandler(void)
{
  if (TIM2->SR & TIM_SR_UIF) // 10ms中断
  {
    TIM2->SR = (uint16_t)~TIM_SR_UIF; // 清标志

    s_tim_20ms++; // 10ms
    s_tim_100ms++;
    if (s_tim_20ms >= 2)
    {
      s_tim_20ms = 0;
      R_Task = 0; // 任务循环
      g_b_task_jtim = 1;
      if (g_b_485_send_tick) // 485发送超时
      {
        g_485_send_tim++;
        if (g_485_send_tim >= 25) /* 485发送超时 500ms*/
        {
          g_485_send_tim = 0;
          g_b_485_send_Error = 1;
        }
      }
      else //
      {
        g_485_send_tim = 0;
      }
    }
    if (s_tim_100ms >= 10)
    {
      s_tim_100ms = 0;
      s_tim_2s++;
      if (s_tim_500ms < 10)
      {
        s_tim_500ms++;
      }

      g_b_100ms_jtim = 1;

      if (s_tim_2s >= 20)
      {
        s_tim_2s = 0;
        g_b_2s_jtim = 1;
      }
    }
    KEY_Filter_processing();
  }
}
void TIM3_IRQHandler(void)
{
  if (TIM3->SR & TIM_SR_UIF) // 用宏，别用 (1<<0)
  {
    TIM3->SR = (uint16_t)~TIM_SR_UIF; // 清标志
  }
}

extern void vMBRTUReceiveBufferFlush(void);
// volatile ULONG eCurrentTimerState = 0;
// ULONG usTimerT15_50us_tmp = 0;
extern ULONG usTimerT15_50us;
extern ULONG usTimerT35_50us;
// uint8_t g_b_485_35num = 0;
/**
 * @brief This function handles TIM4 global interrupt.
 */
void TIM4_IRQHandler(void)
{
#if freertos_modbus == 0
  if (TIM4->SR & TIM_SR_UIF) // 用宏，别用 (1<<0)
  {
    TIM4->SR = (uint16_t)~TIM_SR_UIF; // 清标志

    // // __HAL_TIM_CLEAR_FLAG(&htim4, TIM_FLAG_UPDATE); // 清除更新标志位
    // prvvTIMERExpiredISR(); // ← 关键：T3.5 到期，通知协议栈
    if (eCurrentTimerState > 0)
    {

      if (usTimerT35_50us == 0)
      {
        usTimerT15_50us = 15;
        usTimerT35_50us = 35;
      }

      if (eCurrentTimerState == usTimerT15_50us)
      {
        /* ================= 第一阶段：T1.5 超时 ================= */
        eCurrentTimerState = usTimerT35_50us; // 状态机切换到 T3.5

        // /* 1. 清空接收缓冲区，把 Modbus 状态机拉回 IDLE */
        vMBRTUReceiveBufferFlush();

        /* 2. 变轨：重新装载剩余时间 (T3.5 - T1.5) */
        // HAL_TIM_Base_Stop_IT(&htim4);
        __HAL_TIM_SET_COUNTER(&htim4, 0);
        __HAL_TIM_SET_AUTORELOAD(&htim4, (usTimerT35_50us - usTimerT15_50us) - 1);
        __HAL_TIM_CLEAR_FLAG(&htim4, TIM_FLAG_UPDATE);
        // 5. 重新启动
        __HAL_TIM_ENABLE(&htim4);
      }
      else // if (eCurrentTimerState == usTimerT35_50us)
      {
        /* ================= 第二阶段：T3.5 超时 ================= */
        /* 说明：总线彻底安静了，一帧接收完毕 */
        eCurrentTimerState = usTimerT15_50us;        // 状态机切换到 T1.5
        __HAL_TIM_DISABLE(&htim4);                   // 直接写 CR1 寄存器，关闭计数器
        __HAL_TIM_DISABLE_IT(&htim4, TIM_IT_UPDATE); // 直接写 DIER 寄存器，关闭中断

        /* 通知 FreeModbus 协议栈：产生 EV_FRAME_RECEIVED 事件 */
        prvvTIMERExpiredISR();
      }
    }
  }
#else
  /* 判断更新中断标志 */
  if (TIM4->SR & TIM_SR_UIF)
  {
    TIM4->SR &= (uint16_t)~TIM_SR_UIF; // 清除更新标志
    // if (g_b_485_35num < 235)
    // {
    //   g_b_485_35num++;
    // }
    // else
    // {
    //   g_b_485_35num = 0;
    // }

    prvvTIMERExpiredISR(); // 通知 FreeModbus 超时
  }
  /* 清除所有残留的 CC 标志，防止误触发死循环 */
  TIM4->SR &= ~(TIM_SR_CC1IF | TIM_SR_CC2IF | TIM_SR_CC3IF | TIM_SR_CC4IF);
#endif
}

// void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
// {
//   if (htim->Instance == TIM4)
//   {
//     prvvTIMERExpiredISR();
//      HAL_TIM_IRQHandler(&htim4);
//   }
//   if (htim->Instance == TIM3)
//   {
//     /* code */
//   }
//   if (htim->Instance == TIM2)
//   {
//     /* code */
//   }
// }

/* USER CODE BEGIN 1 */

/******************************************************************************/
/*                 STM32F1xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f1xx.s).                                               */
/******************************************************************************/

/**
 * @brief  This function handles PPP interrupt request.
 * @param  None
 * @retval None
 */
/*void PPP_IRQHandler(void)
{
}*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

#endif
