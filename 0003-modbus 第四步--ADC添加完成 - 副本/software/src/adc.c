#include "adc.h"
#include "main.h"

ADC_HandleTypeDef hadc1;

/* ADC1 init function */
ADC_ChannelConfTypeDef sConfig = {0};
void MX_ADC1_Init(void)
{

  // /* USER CODE BEGIN ADC1_Init 0 */

  // hadc1.Instance = ADC1;
  // hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  // hadc1.Init.ContinuousConvMode = DISABLE;
  // hadc1.Init.DiscontinuousConvMode = DISABLE;
  // hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START; //
  // hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  // hadc1.Init.NbrOfConversion = 1;
  // if (HAL_ADC_Init(&hadc1) != HAL_OK)
  // {
  //   Error_Handler();
  // }

  // // F1
  // if (HAL_ADCEx_Calibration_Start(&hadc1) != HAL_OK)
  // {
  //   Error_Handler();
  // }

  hadc1.Instance = ADC1;
  // hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE; // 从DISABLE改成ENABLE，开启扫描模式
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START; //
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  // hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)

  {
    Error_Handler();
  }

  if (HAL_ADCEx_Calibration_Start(&hadc1) != HAL_OK)

  {
    Error_Handler();
  }

  /* USER CODE BEGIN ADC1_Init 2 */

  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  // sConfig.Channel = ADC_CHANNEL_7;
  // sConfig.Rank = ADC_REGULAR_RANK_1;
  // sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
  // if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)

  // {
  //   Error_Handler();
  // }

  // /** Configure Regular Channel 2: ADC_CHANNEL_8 */
  // sConfig.Channel = ADC_CHANNEL_8;
  // sConfig.Rank = ADC_REGULAR_RANK_2;
  // sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
  // if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)

  // {
  //   Error_Handler();
  // }

  // /** Configure Regular Channel 3: ADC_CHANNEL_3 */
  // sConfig.Channel = ADC_CHANNEL_3;
  // sConfig.Rank = ADC_REGULAR_RANK_3;
  // sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
  // if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)

  // {
  //   Error_Handler();
  // }

  // /** Configure Regular Channel 4: ADC_CHANNEL_9 */
  // sConfig.Channel = ADC_CHANNEL_9;
  // sConfig.Rank = ADC_REGULAR_RANK_4;
  // sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
  // if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)

  // {
  //   Error_Handler();
  // }

  /* USER CODE END ADC1_Init 2 */
}

uint16_t Read_ADC1_CH7(void)
{
  uint16_t value = 0;

  HAL_ADC_Start(&hadc1);
  if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK)

  {
    value = HAL_ADC_GetValue(&hadc1);
  }
  HAL_ADC_Stop(&hadc1);

  return value;
}

/* USER CODE BEGIN Init */
uint8_t g_b_irq_adc_pause = 0; //
// //
// int16_t adc_val = Read_ADC(ADC_CHANNEL_7, 8);
// g_bat_volt_real = get_adc_td10c(AP_CHN_VBAT, 6); //
// if(adc_val != -1)
// {
//     // ??? adc_val
// }
// else
// {
//     // ADC???????????????
// }

int16_t Read_ADC(/*uint8_t adc_id, */ uint32_t ch, uint8_t num)
{
  // 保存原来的通道配置，函数退出时恢复
  ADC_ChannelConfTypeDef orig_sConfig = {0};
  // uint32_t orig_rank;
  // taskENTER_CRITICAL(); // // ???ж???????????????????????ж?FreeRTOS?????
  g_b_irq_adc_pause = 1;
  // taskEXIT_CRITICAL();// // ???ж??????????????????????ж?FreeRTOS?????
  // ?????????ADC1?????????????ADC
  ADC_HandleTypeDef *hadc = &hadc1;

  // ADC_HandleTypeDef *hadc = NULL;
  // /* ????adc_id????? */
  // if(adc_id == 1)
  // {
  //     hadc = &hadc1;
  // }
  // else if(adc_id == 2)
  // {
  //     hadc = &hadc2;
  // }
  // else
  // {
  //     return 0U;
  // }

  ADC_ChannelConfTypeDef sConfig = {0};
  uint32_t sum = 0U;
  uint16_t max = 0;
  uint16_t min = 0xffff;
  // uint16_t Dat = 0;
  uint8_t valid_cnt = 0; // ??Ч????????

  // 保存原有通道配置，退出时恢复，不影响原有多通道扫描
  // HAL_ADC_GetConfigChannel(hadc, ADC_REGULAR_RANK_1, &orig_sConfig);
  // orig_rank = orig_sConfig.Rank;

  // ?????????????
  sConfig.Channel = ch;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
  if (HAL_ADC_ConfigChannel(hadc, &sConfig) != HAL_OK) // ???????
  {
    // taskENTER_CRITICAL(); // ???????????ж?FreeRTOS?????
    g_b_irq_adc_pause = 0;
    // taskEXIT_CRITICAL();// ???????????ж?FreeRTOS?????
    return -1;
  }
  if (num < 1)

  {
    num = 1;
  }
  else if (num > 20)

  {
    num = 20;
  }

  // // 只启动一次ADC，提高效率
  // if (HAL_ADC_Start(hadc) != HAL_OK)
  // {
  //   g_b_irq_adc_pause = 0;
  //   return -1;
  // }

  for (uint8_t i = 0; i < num; i++)

  {
    HAL_ADC_Start(hadc);
    if (HAL_ADC_PollForConversion(hadc, 10U) == HAL_OK)

    {
      uint16_t Dat = HAL_ADC_GetValue(hadc);
      // Dat = HAL_ADC_GetValue(hadc);
      sum += Dat;
      valid_cnt++;

      if (Dat > max)
        max = Dat;
      if (Dat < min)
        min = Dat;
    }

    HAL_ADC_Stop(hadc);
  }
  // HAL_ADC_Stop(hadc);
  // 恢复原来的通道配置
  HAL_ADC_ConfigChannel(hadc, &orig_sConfig);
  if (valid_cnt == 0)

  {
    // taskENTER_CRITICAL();
    g_b_irq_adc_pause = 0;
    // taskEXIT_CRITICAL();
    return -1; // ??????????
  }

  if (valid_cnt > 2) // ????????

  {
    sum = (sum - max - min) / (valid_cnt - 2);
  }
  else
  {
    sum = sum / valid_cnt; // ????????
  }
  // taskENTER_CRITICAL();
  g_b_irq_adc_pause = 0;
  // taskEXIT_CRITICAL();

  return (int16_t)(sum);
}

//  // 在这里定义存储ADC结果的数组
// ?uint16_t adc_results[4]; // 保存4个通道的ADC值
// ?调用函数读取所有通道
// Read_ADC1_AllChannel(adc_results);

// // 读取之后你就可以使用这四个值了，比如打印显示到OLED
// OLED_Clear();
// OLED_ShowNum(0, 2, adc_results[0], 4, 16); // 显示通道7的值

void Read_ADC1_AllChannel(uint16_t *adcValues) // 读取所有通道的ADC值
{
  HAL_ADC_Start(&hadc1);
  if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK) // 等待转换完成

  {
    adcValues[0] = HAL_ADC_GetValue(&hadc1); // 获取通道0的ADC值
  }
  if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK)

  {
    adcValues[1] = HAL_ADC_GetValue(&hadc1);
  }
  if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK)

  {
    adcValues[2] = HAL_ADC_GetValue(&hadc1);
  }
  if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK)

  {
    adcValues[3] = HAL_ADC_GetValue(&hadc1);
  }
  HAL_ADC_Stop(&hadc1);
}
/* USER CODE END Init */
void HAL_ADC_MspInit(ADC_HandleTypeDef *adcHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if (adcHandle->Instance == ADC1)

  {
    /* ADC1 clock enable */
    __HAL_RCC_ADC1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**ADC1 GPIO Configuration
    PA2     ------> ADC1_IN2
    PA5     ------> ADC1_IN5
    PA7     ------> ADC1_IN7
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_5 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USER CODE BEGIN ADC1_MspInit 1 */

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* USER CODE END ADC1_MspInit 1 */
  }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef *adcHandle)
{

  if (adcHandle->Instance == ADC1)

  {
    /* Peripheral clock disable */
    __HAL_RCC_ADC1_CLK_DISABLE();

    /**ADC1 GPIO Configuration
    PA2     ------> ADC1_IN2
    PA5     ------> ADC1_IN5
    PA7     ------> ADC1_IN7
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2 | GPIO_PIN_5 | GPIO_PIN_7);
  }
}
