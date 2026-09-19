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

extern DMA_HandleTypeDef dma_handle; // 新增这一行
/* USER CODE BEGIN EV */

/* USER CODE BEGIN EV */
extern void prvvUARTTxReadyISR(void);
extern void prvvUARTRxISR(void);
extern void prvvTIMERExpiredISR(void);
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
uint8_t adc_dma_num = 0;
uint16_t ch0_value[d_ADC_DMA_num_max] = {0}; // PA0
uint16_t ch1_value[d_ADC_DMA_num_max] = {0}; // PA1
uint16_t ch2_value[d_ADC_DMA_num_max] = {0}; // PA2
uint16_t ch3_value[d_ADC_DMA_num_max] = {0}; // PA3
uint16_t adc_result[4] = {0};
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

void DMA1_Channel1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&dma_handle);
}

// DMA传输完成回调函数（4个通道全部采集并传输完毕时自动调用）

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) // 采集完成回调函数
{
  // 采集完成的4个通道数据在 adc_result[0]~[3] 中
  // 在这里处理数据，例如：
  ch0_value[adc_dma_num] = adc_result[0]; // PA0
  ch1_value[adc_dma_num] = adc_result[1]; // PA1
  ch2_value[adc_dma_num] = adc_result[2]; // PA2
  ch3_value[adc_dma_num] = adc_result[3]; // PA3
  adc_dma_num++;
  if (adc_dma_num >= d_ADC_DMA_num_max)
  {
    adc_dma_num = 0;
  }
}

// DMA半传输回调函数（传输2个数据时调用）
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc) // 采集半完成回调函数
{
  // 前2个通道数据已传输完成
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
  // 定时器中断
  if (TIM4->SR & (1 << 0))
  {
    // 定时器中断
    prvvTIMERExpiredISR();

    TIM4->SR &= ~(1 << 0); // 清除中断标志位
  }
  // prvvTIMERExpiredISR();
  /* USER CODE END TIM4_IRQn 1 */
}

/**
 * @brief This function handles USART2 global interrupt.
 */
// xMBRTUReceiveFSM() 接收中断服务函数，用于Modbus接收数据
// xMBRTUReceiveFSM() 发送中断服务函数，用于Modbus发送数据
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
    prvvUARTRxISR(); // 接收中断服务函数，用于Modbus接收数据
  }

  if (__HAL_UART_GET_IT_SOURCE(&huart2, UART_IT_TXE) != RESET)
  {
    prvvUARTTxReadyISR(); // 发送中断服务函数，用于Modbus发送数据
  }

  HAL_NVIC_ClearPendingIRQ(USART2_IRQn);
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

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

#endif
