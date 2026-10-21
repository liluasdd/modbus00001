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
extern void prvvUARTTxReadyISR(void);
extern void prvvUARTRxISR(void);
extern void prvvTIMERExpiredISR(void);

u8 s_tim0_10ms = 0;
u16 s_tim0_100ms = 1;
u8 s_tim0_2s = 0;
u8 g_485_send_tim = 0;
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

/**
 * @brief  This function handles SVCall exception.
 * @param  None
 * @retval None
 */
void SVC_Handler(void)
{
}

/**
 * @brief  This function handles Debug Monitor exception.
 * @param  None
 * @retval None
 */
void DebugMon_Handler(void)
{
}

/**
 * @brief  This function handles PendSVC exception.
 * @param  None
 * @retval None
 */
void PendSV_Handler(void)
{
}

/**
 * @brief  This function handles SysTick Handler.
 * @param  None
 * @retval None
 */
void SysTick_Handler(void)
{
  HAL_IncTick();
}

extern ADC_HandleTypeDef adc_handle; // ADC句柄
/*
ADC+DMA转换完成回调函数
 **/
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
  // 40个数据采集完成
  // 在这里处理 adc_buf 数据
  adc_ch0[adc_DMA_num] = adc_result[0]; // CH0 的值;
  adc_ch1[adc_DMA_num] = adc_result[1]; // CH1 的值;
  adc_ch2[adc_DMA_num] = adc_result[2]; // CH2 的值;
  adc_ch3[adc_DMA_num] = adc_result[3]; // CH3 的值;
  adc_DMA_num++;
  if (adc_DMA_num >= adc_DMA_max)
  {
    adc_DMA_num = 0;
    // 处理完后停止 ADC
    HAL_ADC_Stop_DMA(&adc_handle);
  }
}

// /**
//  * @brief This function handles TIM4 global interrupt.
//  */
// void TIM2_IRQHandler(void)
// {
//   if (TIM2->SR & (1 << 0)) // 定时器中断  50/100us一次
//   {
//     TIM2->SR &= ~(1 << 0); // 清除中断标志位

//     // /* 停掉定时器 */
//     // HAL_TIM_Base_Stop_IT(&htim2);

//     __HAL_TIM_DISABLE(&htim2);
//     __HAL_TIM_SET_COUNTER(&htim2, 0);
//     __HAL_TIM_DISABLE_IT(&htim2, TIM_IT_UPDATE);
//     __HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE);

//     // /* 强制切回接收空闲状态，并清空缓冲区 */
//     // extern void vMBRTUReceiveBufferFlush(void);
//     // vMBRTUReceiveBufferFlush();
//   }
// }

void TIM3_IRQHandler(void)
{
  if (TIM3->SR & (1 << 0)) // 定时器中断 100us一次
  {
    TIM3->SR &= ~(1 << 0); // 清除中断标志位
    s_tim0_10ms++;
    s_tim0_100ms++;
    if (s_tim0_10ms >= 100)
    {
      g_b_task_jtim = 1;
      s_tim0_10ms = 0;
      if (g_b_485_send_tick) // 485发送超时
      {
        g_485_send_tim++;
        if (g_485_send_tim >= 50)
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
    if (s_tim0_100ms >= 1000)
    {
      s_tim0_2s++;
      s_tim0_100ms = 0;
      g_b_100ms_jtim = 1;

      if (s_tim0_2s >= 20)
      {
        s_tim0_2s = 0;
        g_b_2s_jtim = 1;
      }
    }
  }
}

extern void vMBRTUReceiveBufferFlush(void);
ULONG eCurrentTimerState = 0;
extern ULONG usTimerT15_50us;
extern ULONG usTimerT35_50us;
/**
 * @brief This function handles TIM4 global interrupt.
 */
void TIM4_IRQHandler(void)
{
  //  if (htim->Instance == TIM4)
  if (TIM4->SR & (1 << 0)) // 定时器中断  50/100us一次
  {
    TIM4->SR &= ~(1 << 0); // 清除中断标志位

    if (eCurrentTimerState == usTimerT15_50us)
    {
      /* ================= 第一阶段：T1.5 超时 ================= */
      /* 说明：两个字符之间间隔超过了 1.5 个字符，当前帧作废 */

      eCurrentTimerState = usTimerT35_50us; // 状态机切换到 T3.5

      // /* 1. 清空接收缓冲区，把 Modbus 状态机拉回 IDLE */
      vMBRTUReceiveBufferFlush();

      /* 2. 变轨：重新装载剩余时间 (T3.5 - T1.5) */
      HAL_TIM_Base_Stop_IT(&htim4);
      __HAL_TIM_SET_COUNTER(&htim4, 0);
      __HAL_TIM_SET_AUTORELOAD(&htim4, (usTimerT35_50us - usTimerT15_50us) - 1);
      __HAL_TIM_CLEAR_FLAG(&htim4, TIM_FLAG_UPDATE);
      HAL_TIM_Base_Start_IT(&htim4);
      // vMBPortTimersEnable();
    }
    else if (eCurrentTimerState == usTimerT35_50us)
    {
      /* ================= 第二阶段：T3.5 超时 ================= */
      /* 说明：总线彻底安静了，一帧接收完毕 */
      eCurrentTimerState = usTimerT15_50us; // 状态机切换到 T1.5

      HAL_TIM_Base_Stop_IT(&htim4); // 停掉定时器，防止反复进

      /* 通知 FreeModbus 协议栈：产生 EV_FRAME_RECEIVED 事件 */
      prvvTIMERExpiredISR();
    }
  }
}

/**
 * @brief This function handles USART2 global interrupt.
 */
void USART2_IRQHandler(void)
{
  // // 接收中断
  // if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE) != RESET)
  // {
  //   if (__HAL_UART_GET_IT_SOURCE(&huart2, UART_IT_RXNE) != RESET)
  //   {
  //     __HAL_UART_CLEAR_FLAG(&huart2, UART_FLAG_RXNE);
  //     prvvUARTRxISR();
  //   }
  // }
  // // 发送中断
  // if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TXE) != RESET)
  // {
  //   if (__HAL_UART_GET_IT_SOURCE(&huart2, UART_IT_TXE) != RESET)
  //   {
  //     __HAL_UART_CLEAR_FLAG(&huart2, UART_FLAG_TXE);
  //     prvvUARTTxReadyISR();
  //   }
  // }

  /* USER CODE BEGIN USART2_IRQn 0 */

  /* USER CODE END USART2_IRQn 0 */
  // HAL_UART_IRQHandler(&huart2);
  /* USER CODE BEGIN USART2_IRQn 1 */
  if (__HAL_UART_GET_IT_SOURCE(&huart2, UART_IT_RXNE) != RESET)
  {
    // uint8_t ch = (uint8_t)(huart2.Instance->DR);
    if (g_b_485_send_tick) // 发送数据标准，发送数据时，不接收数据。
    {
      // uint8_t ch = (uint8_t)(huart2.Instance->DR);
      (void)huart2.Instance->DR; // 发送中：读 DR 清 RXNE，数据丢弃
    }
    else
    {
      // /* 存进缓冲区 */
      // if (usRcvBufferPos < 256)
      // {
      //   ucRTUBuf[usRcvBufferPos++] = ch;
      // }
      // /* 重置 3.5 字符定时器 */
      // vMBPortTimersEnable();
      prvvUARTRxISR(); // 接收中断服务函数，用于Modbus接收数据
    }
  }
  ///* 发送阶段 */
  if (__HAL_UART_GET_IT_SOURCE(&huart2, UART_IT_TXE) != RESET)
  {
    prvvUARTTxReadyISR(); // 发送中断服务函数，用于Modbus发送数据
  }

  ///* 发送最后一个字节 *//* 当硬件真正发完最后一个字节，触发 TC 中断 */
  if (__HAL_UART_GET_IT_SOURCE(&huart2, UART_IT_TC) != RESET &&
      __HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC) != RESET)
  {
    __HAL_UART_CLEAR_FLAG(&huart2, UART_FLAG_TC); // 清 TC 标志
    __HAL_UART_DISABLE_IT(&huart2, UART_IT_TC);   // 关 TC 中断

    RS485_DE_LOW_OFF; // 现在可以安全切回接收了
    g_b_485_send_Error = 0;
    g_b_485_send_tick = 0; // 清除发送标志

    /* 确保接收中断是打开的，准备接收主站下一帧 */
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
  }

  /*清 NVIC pending 位，防止中断退出后立刻又进来。但正常情况硬件会自动处理，这行是保险。*/
  // HAL_NVIC_ClearPendingIRQ(USART2_IRQn);//不屏蔽，可能丢字节：清除了合法的新 pending 请求
  HAL_UART_IRQHandler(&huart2);
  /* USER CODE END USART2_IRQn 1 */
}

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
