/**
 ******************************************************************************
 * @file    bsp_basic_tim.c
 * @author  STMicroelectronics
 * @version V1.0
 * @date    2020-xx-xx
 * @brief   锟斤拷锟斤拷锟斤拷时锟斤拷锟斤拷时锟斤拷锟斤拷
 ******************************************************************************
 * @attention
 *
 * 实锟斤拷平台:野锟斤拷  STM32 F429 锟斤拷锟斤拷锟斤拷
 * 锟斤拷坛    :http://www.firebbs.cn
 * 锟皆憋拷    :http://fire-stm32.taobao.com
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "tim.h"

TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim3;

/* TIM4 init function */
void MX_TIM4_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  // 锟斤拷时锟斤拷时锟斤拷源TIMxCLK = 2 * PCLK1
  //				PCLK1 = HCLK / 4
  //				=> TIMxCLK=HCLK/2=SystemCoreClock/2=90MHz
  //  锟借定锟斤拷时锟斤拷频锟斤拷为=TIMxCLK/(TIM_Prescaler+1)=20000Hz
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
  
}

/* TIM3 init function - 100us定时 */
void MX_TIM3_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 36 - 1; // 72MHz / 36 = 2MHz
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 200 - 1; // 2MHz / 200 = 10kHz (100us)
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    while (1)
      ;
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;// 使能时钟
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)// 配置时钟源
  {
    while (1)
      ;
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;// 使能中断
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;// 禁用主从模式
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)// 配置主从模式
  {
    while (1)
      ;
  }
}

// 初始化  时钟和中断
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *tim_baseHandle)
{

  if (tim_baseHandle->Instance == TIM4)
  {
    /* TIM4 clock enable */
    __HAL_RCC_TIM4_CLK_ENABLE();

    // TIM4中断配置
    HAL_NVIC_SetPriority(TIM4_IRQn, 0, 1);
    // TIM4中断使能
    HAL_NVIC_EnableIRQ(TIM4_IRQn);
  }
  else if (tim_baseHandle->Instance == TIM3)
  {
    /* TIM3 clock enable */
    __HAL_RCC_TIM3_CLK_ENABLE();

    /* TIM3中断配置 */
    HAL_NVIC_SetPriority(TIM3_IRQn, 0, 2);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);
  }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *tim_baseHandle)
{

  if (tim_baseHandle->Instance == TIM4) // 使能TIM4定时器
  {
    /* Peripheral clock disable */
    __HAL_RCC_TIM4_CLK_DISABLE(); // 禁用TIM4时钟

    /* TIM4 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM4_IRQn); // 禁用TIM4中断
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

u8 s_tim0_10ms = 0;
u16 s_tim0_100ms = 1;
u8 s_tim0_2s = 0;
u8 g_485_send_tim = 0;

//u8 g_b_task_jtim = 0;
//u8 g_b_485_send_tick = 0;
//u8 g_b_485_send_Error = 0;
//u8 g_b_100ms_jtim = 0;
//u8 g_b_2s_jtim = 0;

extern void prvvTIMERExpiredISR(void);

/* TIM3中断回调函数 - 100us定时中断 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM3)
  {

    s_tim0_10ms++;
    s_tim0_100ms++;
    if (s_tim0_10ms >= 100)
    {
      g_b_task_jtim = 1;
      s_tim0_10ms = 0;
      if (g_b_485_send_tick)
      {
        g_485_send_tim++;
        if (g_485_send_tim >= 50)
        {
          g_485_send_tim = 0;
          g_b_485_send_Error = 1;
        }
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

  if (htim->Instance == TIM4)
  {
    prvvTIMERExpiredISR(); // 调用定时器中断服务函数
  }
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
