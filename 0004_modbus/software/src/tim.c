/**
 ******************************************************************************
 * @file    bsp_basic_tim.c
 * @author  STMicroelectronics
 * @version V1.0
 * @date    2020-xx-xx
 * @brief   锟斤拷锟斤拷锟斤拷时锟斤拷锟斤拷时锟斤拷锟斤??
 ******************************************************************************
 * @attention
 *
 * 实锟斤拷平台:野锟斤拷  STM32 F429 锟斤拷锟斤拷锟斤??
 * 锟斤拷坛    :http://www.firebbs.cn
 * 锟皆憋拷    :http://fire-stm32.taobao.com
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "tim.h"

TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim2;

/* TIM4 init function */
void MX_TIM4_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  // 锟斤拷时锟斤拷时锟斤拷源TIMxCLK = 2 * PCLK1
  //				PCLK1 = HCLK / 4
  //				=> TIMxCLK=HCLK/2=SystemCoreClock/2=90MHz
  //  锟借定锟斤拷时锟斤拷???锟斤拷??=TIMxCLK/(TIM_Prescaler+1)=20000Hz
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 1800 - 1; // 20KHZ
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 35;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    while (1)
      ;
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    while (1)
      ;
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    while (1)
      ;
  }

  // ///* 3. 开启 TIM4 的 NVIC 中断*/
  // HAL_NVIC_SetPriority(TIM4_IRQn, 0, 0);// 优先级0
  // HAL_NVIC_EnableIRQ(TIM4_IRQn);
  // ///* 4.启动定时器，并开启中断*/
  // HAL_TIM_Base_Start_IT(&htim4);
}

void MX_TIM2_Init(void)
{
  // TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  // TIM_MasterConfigTypeDef sMasterConfig = {0};
  // 1. 开启 TIM2 时钟
  __HAL_RCC_TIM2_CLK_ENABLE();
  htim2.Instance = TIM2;
  // 2. 修改 PSC 和 ARR 实现 100us
  htim2.Init.Prescaler = 72 - 1; // 72MHz / 72 = 1MHz (1us)
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 100 - 1; // 1MHz 计数 100 次 = 100us
  // htim2.Init.Period = 200 - 1; // 1MHz 计数 100 次 = 100us
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  HAL_TIM_Base_Init(&htim2);

  // sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL; // 内部时钟源
  // HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig);    // 配置时钟源
  // sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  // sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  // HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig);

  // 3. 开启 TIM2 的 NVIC 中断
  HAL_NVIC_SetPriority(TIM2_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(TIM2_IRQn);
  // 4.启动定时器，并开启中断
  HAL_TIM_Base_Start_IT(&htim2);
}

/* TIM3 init function - 100us定时 */
void MX_TIM3_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  // 1. 开启 TIM3 时钟
  __HAL_RCC_TIM3_CLK_ENABLE();
  htim3.Instance = TIM3;
  // 2. 配置计数频率和周期
  htim3.Init.Prescaler = 72 - 1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 300 - 1;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  HAL_TIM_Base_Init(&htim3);
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig);
  // 3. 配置主模式输出 TRGO 为更新事件
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig);

  // TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  // TIM_MasterConfigTypeDef sMasterConfig = {0};
  // htim3.Instance = TIM3;
  // htim3.Init.Prescaler = 36 - 1; // 72MHz / 36 = 2MHz（前提：TIM3 时钟确为 72MHz）
  // htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  // htim3.Init.Period = 200 - 1; // 2MHz / 200 = 10kHz → 100?s 更新一次
  // htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  // htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  // if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  // { while (1); }
  // sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  // if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  // {while (1) ;}
  // sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET; // 使能主从模式
  // sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  // if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  // {while (1);}
  // HAL_NVIC_SetPriority(TIM3_IRQn, 0, 0);
  // HAL_NVIC_EnableIRQ(TIM3_IRQn);
  // HAL_TIM_Base_Start_IT(&htim3); /* 启动TIM3定时器中断 */
}

// 初???化  时钟和中??
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *tim_baseHandle)
{

  if (tim_baseHandle->Instance == TIM4)
  {
    /* TIM4 clock enable */
    __HAL_RCC_TIM4_CLK_ENABLE();

    // TIM4????配置
    HAL_NVIC_SetPriority(TIM4_IRQn, 0, 0);// 最高优先级0
    // TIM4????使能
    HAL_NVIC_EnableIRQ(TIM4_IRQn);
  }
  else if (tim_baseHandle->Instance == TIM3)
  {
    /* TIM3 clock enable */
    __HAL_RCC_TIM3_CLK_ENABLE();

    /* TIM3????配置 */
    HAL_NVIC_SetPriority(TIM3_IRQn, 0, 2); //
    HAL_NVIC_EnableIRQ(TIM3_IRQn);
  }
  else if (tim_baseHandle->Instance == TIM2)
  {
    /* TIM2 clock enable */
    __HAL_RCC_TIM2_CLK_ENABLE();

    /* TIM2????配置 */
    HAL_NVIC_SetPriority(TIM2_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
  }
}

// 释放  时钟和中??
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *tim_baseHandle)
{

  if (tim_baseHandle->Instance == TIM4) // 使能TIM4定时??
  {
    /* Peripheral clock disable */
    __HAL_RCC_TIM4_CLK_DISABLE(); // 禁用TIM4时钟

    /* TIM4 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM4_IRQn); // 禁用TIM4????
  }
  else if (tim_baseHandle->Instance == TIM3)
  {
    /* Peripheral clock disable */
    __HAL_RCC_TIM3_CLK_DISABLE();

    /* TIM3 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM3_IRQn);
  }
}

/* USER CODE BEGIN 1 */

// u8 g_b_task_jtim = 0;
// u8 g_b_485_send_tick = 0;
// u8 g_b_485_send_Error = 0;
// u8 g_b_100ms_jtim = 0;
// u8 g_b_2s_jtim = 0;

// extern void prvvTIMERExpiredISR(void);
// /* TIM3????回调函数 - 100us定时???? */
// void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
// {
//   if (htim->Instance == TIM3)
//   {

//     s_tim0_10ms++;
//     s_tim0_100ms++;
//     if (s_tim0_10ms >= 100)
//     {
//       g_b_task_jtim = 1;
//       s_tim0_10ms = 0;
//       if (g_b_485_send_tick)// 485发送超时
//       {
//         g_485_send_tim++;
//         if (g_485_send_tim >= 50)
//         {
//           g_485_send_tim = 0;
//           g_b_485_send_Error = 1;
//         }
//       }
//       else//
//       {
//         g_485_send_tim = 0;
//       }
//     }
//     if (s_tim0_100ms >= 1000)
//     {
//       s_tim0_2s++;
//       s_tim0_100ms = 0;
//       g_b_100ms_jtim = 1;

//       if (s_tim0_2s >= 20)
//       {
//         s_tim0_2s = 0;
//         g_b_2s_jtim = 1;
//       }
//     }
//   }

//   if (htim->Instance == TIM4)
//   {
//     prvvTIMERExpiredISR(); // 调用定时器中??服务函数
//   }
// }

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
