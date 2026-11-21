#include "usart.h"
#include "main.h"

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3; /* UART3句柄 */

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
  HAL_NVIC_SetPriority(USART2_IRQn, 5, 0); // 同样要 >= 6
  HAL_NVIC_EnableIRQ(USART2_IRQn);
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

    GPIO_InitStruct.Pin = USART1_TX_pin;          /* 串口发送引脚号 */
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;       /* 复用推挽输出 */
    GPIO_InitStruct.Pull = GPIO_PULLUP;           /* 上拉 */
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; /* IO速度设置为高速 */
    HAL_GPIO_Init(USART1_TX_port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = USART1_RX_pin; /* 串口RX脚 模式设置 */
    GPIO_InitStruct.Mode = GPIO_MODE_AF_INPUT;
    HAL_GPIO_Init(USART1_RX_port, &GPIO_InitStruct); /* 串口RX脚 必须设置成输入模式 */

    HAL_NVIC_EnableIRQ(USART1_IRQn);         /* 使能USART1中断通道 */
    HAL_NVIC_SetPriority(USART1_IRQn, 7, 0); /* 组2，最低优先级:抢占优先级6，子优先级3 */

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
    HAL_NVIC_SetPriority(DEBUG_USART_IRQ, 6, 0); // 抢占优先级6，子优先级0
    HAL_NVIC_EnableIRQ(DEBUG_USART_IRQ);         // 使能USART2中断通道

    // __HAL_UART_ENABLE_IT(uartHandle, UART_IT_RXNE); /* 使能UART2接收中断 */
    // __HAL_UART_ENABLE_IT(uartHandle, UART_IT_IDLE); /* 使能UART2总线空闲中断 */
  }

  if (uartHandle->Instance == USART3)
  {
    /* USER CODE BEGIN USART3_MspInit 0 */

    /* USER CODE END USART3_MspInit 0 */
    /* USART3 clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();

    __HAL_RCC_GPIOD_CLK_ENABLE();

    /* 配置Tx引脚为复用功能  */
    GPIO_InitStruct.Pin = USART3_TX_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(USART3_TX_port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = USART3_RX_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_INPUT;
    HAL_GPIO_Init(USART3_RX_port, &GPIO_InitStruct);

    /* USART3 interrupt Init */
    HAL_NVIC_SetPriority(USART3_IRQn, 7, 0); // 抢占优先级6，子优先级2
    HAL_NVIC_EnableIRQ(USART3_IRQn);         // 使能USART3中断通道

    __HAL_UART_ENABLE_IT(uartHandle, UART_IT_RXNE); /* 使能UART3接收中断 */
    __HAL_UART_ENABLE_IT(uartHandle, UART_IT_IDLE); /* 使能UART3总线空闲中断 */
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

// // 串口2发送一个字符串
// void usart_send_string(uint8_t *s)
// {
//   while (*s != NULL)
//   {
//     HAL_UART_Transmit(&huart2, s, 1, 0xffff);
//     s++;
//   }
// }
// // 485收发状态切换管脚
// void modbus_master_control_init(void)
// {
//   GPIO_InitTypeDef GPIO_InitStruct = {0};
//   GPIO_InitStruct.Pin = MODBUS_MASTER_GPIO_PIN;
//   GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//   GPIO_InitStruct.Pull = GPIO_NOPULL;
//   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//   HAL_GPIO_Init(MODBUS_MASTER_GPIO_PORT, &GPIO_InitStruct);
// }


/**
 * @brief This function handles USART2 global interrupt.
 */
void USART2_IRQHandler(void)
{
#if freertos_modbus
  /* USER CODE BEGIN USART2_IRQn 1 */

  // 1. 处理溢出错误（不处理的话串口会卡死）
  if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_ORE) != RESET)
  {
    __HAL_UART_CLEAR_OREFLAG(&huart2); // 清除 ORE 标志（先读 SR 再读 DR）
    // 发生溢出说明丢数据了，直接丢弃当前帧，等待下一帧
    if (!g_b_485_send_tick) /*发送计时标志位，0表示标志，不在发送*/
      Modbus_ResetRx();     // Modbus 复位函数 /* 只有接收态才复位协议栈 */
  }

  // 2. 接收中断
  if (__HAL_UART_GET_IT_SOURCE(&huart2, UART_IT_RXNE) != RESET &&
      __HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE) != RESET)
  {
    if (g_b_485_send_tick)
    {
      (void)huart2.Instance->DR; // 发送中：读 DR 清 RXNE，数据丢弃
    }
    else
    {
      // 交给 FreeModbus 处理。
      // 注意：内部的 xMBRTUReceiveFSM 会读走 DR（也就是调用了你的 GetByte）
      // 并且在收完一帧后调用 xQueueSendFromISR 产生 EV_FRAME_RECEIVED 事件
      prvvUARTRxISR();
    }
  }

  // 3. 发送空（TXE）中断
  if (__HAL_UART_GET_IT_SOURCE(&huart2, UART_IT_TXE) != RESET &&
      __HAL_UART_GET_FLAG(&huart2, UART_FLAG_TXE) != RESET)
  {
    // 注意：通常不用手动清 TXE，因为向 DR 写数据会自动清 TXE
    prvvUARTTxReadyISR();
  }

  // 4. 发送完成（TC）中断
  if (__HAL_UART_GET_IT_SOURCE(&huart2, UART_IT_TC) != RESET &&
      __HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC) != RESET)
  {
    __HAL_UART_CLEAR_FLAG(&huart2, UART_FLAG_TC); // 清 TC 标志
    __HAL_UART_DISABLE_IT(&huart2, UART_IT_TC);   // 关 TC 中断

    RS485_DE_LOW_OFF; // 切回接收
    g_b_485_send_Error = 0;
    g_b_485_send_tick = 0;

    /* 确保接收中断是打开的，准备接收主站下一帧 */
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
  }

  ///* 不能在这里调用 HAL_UART_IRQHandler(&huart2);  我已经在上面完全接管了硬件！*/
  /* USER CODE END USART2_IRQn 1 */

#else
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
#endif
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

