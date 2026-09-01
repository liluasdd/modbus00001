/**
 ******************************************************************************
 * @file    bsp_basic_tim.c
 * @author  STMicroelectronics
 * @version V1.0
 * @date    2020-xx-xx
 * @brief   串口范例
 ******************************************************************************
 * @attention
 *
 * 实验平台:野火  STM32 F103 开发板
 * 论坛    :http://www.firebbs.cn
 * 淘宝    :http://fire-stm32.taobao.com
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

UART_HandleTypeDef huart2;

/**
 * @brief  DEBUG_USART GPIO 配置,工作模式配置。115200 8-N-1
 * @param  无
 * @retval 无
 */
void MX_USART2_UART_Init(uint8_t ucPORT, uint32_t ulBaudRate, uint8_t eParity)
{
  if (ucPORT != 2) // 必须设置为串口2
    return;
  huart2.Instance = DEBUG_USART;
  huart2.Init.BaudRate = ulBaudRate;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = eParity;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    while (1)
      ;
  }
}
/**
 * @brief UART MSP 初始化
 * @param huart: UART handle
 * @retval 无
 */
void HAL_UART_MspInit(UART_HandleTypeDef *uartHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  if (uartHandle->Instance == USART1) /* 如果是串口1，进行串口1 MSP初始化 */
  {
    __HAL_RCC_GPIOA_CLK_ENABLE();  /* 使能串口TX脚时钟 */
    __HAL_RCC_USART1_CLK_ENABLE(); /* 使能串口时钟 */

    GPIO_InitStruct.Pin = GPIO_PIN_9;             /* 串口发送引脚号 */
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;       /* 复用推挽输出 */
    GPIO_InitStruct.Pull = GPIO_PULLUP;           /* 上拉 */
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; /* IO速度设置为高速 */
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10; /* 串口RX脚 模式设置 */
    GPIO_InitStruct.Mode = GPIO_MODE_AF_INPUT;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); /* 串口RX脚 必须设置成输入模式 */

    HAL_NVIC_EnableIRQ(USART1_IRQn);         /* 使能USART1中断通道 */
    HAL_NVIC_SetPriority(USART1_IRQn, 3, 3); /* 组2，最低优先级:抢占优先级3，子优先级3 */

    __HAL_UART_ENABLE_IT(uartHandle, UART_IT_RXNE); /* 使能UART1接收中断 */
    __HAL_UART_ENABLE_IT(uartHandle, UART_IT_IDLE); /* 使能UART1总线空闲中断 */
  }

  if (uartHandle->Instance == USART2)
  {
    /* USER CODE BEGIN USART2_MspInit 0 */

    /* USER CODE END USART2_MspInit 0 */
    /* USART2 clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();

    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    /* 配置Tx引脚为复用功能  */
    GPIO_InitStruct.Pin = DEBUG_USART_TX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = DEBUG_USART_RX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_INPUT;
    HAL_GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStruct);

    /* USART2 interrupt Init */
    HAL_NVIC_SetPriority(DEBUG_USART_IRQ, 0, 0); // 抢占优先级0，子优先级0
    HAL_NVIC_EnableIRQ(DEBUG_USART_IRQ);         // 使能USART2中断通道
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef *uartHandle)
{

  if (uartHandle->Instance == USART2)
  {
    /* Peripheral clock disable */
    __HAL_RCC_USARTx_CLK_DISABLE;

    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    HAL_GPIO_DeInit(DEBUG_USART_TX_GPIO_PORT, DEBUG_USART_TX_PIN | DEBUG_USART_RX_PIN);

    /* USART2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(DEBUG_USART_IRQ);
  }
}

// 串口2发送一个字符串
void usart_send_string(uint8_t *s)
{
  while (*s != NULL)
  {
    HAL_UART_Transmit(&huart2, s, 1, 0xffff);
    s++;
  }
}

// 485收发状态切换管脚
void modbus_master_control_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.Pin = MODBUS_MASTER_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(MODBUS_MASTER_GPIO_PORT, &GPIO_InitStruct);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
