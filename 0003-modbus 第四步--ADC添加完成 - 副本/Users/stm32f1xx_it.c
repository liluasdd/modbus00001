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
extern TIM_HandleTypeDef htim4;
extern UART_HandleTypeDef huart2;
/* USER CODE BEGIN EV */
extern void prvvUARTTxReadyISR(void);
extern void prvvUARTRxISR(void);
extern void prvvTIMERExpiredISR(void);
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
/**
 * @brief This function handles TIM4 global interrupt.
 */
void TIM4_IRQHandler(void)
{
  /* USER CODE BEGIN TIM4_IRQn 0 */

  /* USER CODE END TIM4_IRQn 0 */
  // HAL_TIM_IRQHandler(&htim4);
  /* USER CODE BEGIN TIM4_IRQn 1 */
  //新加
  if (TIM4->SR & (1 << 0))
  {
    // 调用定时器中断服务函数,通知协议栈 3.5字符等待时间已到，超时
    prvvTIMERExpiredISR();

    TIM4->SR &= ~(1 << 0); // 清除更新中断标志位
  }
  // prvvTIMERExpiredISR();
  /* USER CODE END TIM4_IRQn 1 */
}

/**
 * @brief This function handles USART2 global interrupt.
 */
// xMBRTUReceiveFSM() 缓存字节，用于接收完整的Modbus帧
// xMBRTUReceiveFSM() 处理接收的字节，根据状态机的当前状态进行处理
void USART2_IRQHandler(void)
{
  // // 接收中断处理
  // if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE) != RESET)
  // {
  //   if (__HAL_UART_GET_IT_SOURCE(&huart2, UART_IT_RXNE) != RESET)
  //   {
  //     __HAL_UART_CLEAR_FLAG(&huart2, UART_FLAG_RXNE);
  //     prvvUARTRxISR();
  //   }
  // }

  // // 发送中断处理
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
    prvvUARTRxISR(); // 接收中断
  }

  if (__HAL_UART_GET_IT_SOURCE(&huart2, UART_IT_TXE) != RESET)
  {
    prvvUARTTxReadyISR(); // 发送中断
  }

  HAL_NVIC_ClearPendingIRQ(USART2_IRQn);
  HAL_UART_IRQHandler(&huart2);
  /* USER CODE END USART2_IRQn 1 */
}

/* USER CODE BEGIN 1 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) // 定时器中断回调函数，用于连接porttimer.c文件的函数
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the __HAL_TIM_PeriodElapsedCallback could be implemented in the user file
   */
  prvvTIMERExpiredISR();
}
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

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

#endif
