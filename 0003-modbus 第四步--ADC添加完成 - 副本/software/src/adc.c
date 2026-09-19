#include "adc.h"
#include "main.h"

ADC_HandleTypeDef hadc1;

/* ADC1 init function */
ADC_ChannelConfTypeDef sConfig = {0};
void MX_ADC1_Init(void)
{
  hadc1.Instance = ADC1;
  // hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE; // ��DISABLE�ĳ�ENABLE������ɨ��ģʽ
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
  // ����ԭ����ͨ�����ã������˳�ʱ�ָ�
  ADC_ChannelConfTypeDef orig_sConfig = {0};
  // uint32_t orig_rank;
  // taskENTER_CRITICAL(); // // ???��???????????????????????��?FreeRTOS?????
  g_b_irq_adc_pause = 1;
  // taskEXIT_CRITICAL();// // ???��??????????????????????��?FreeRTOS?????
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
  uint8_t valid_cnt = 0; // ??��????????

  // ����ԭ��ͨ�����ã��˳�ʱ�ָ�����Ӱ��ԭ�ж�ͨ��ɨ��
  // HAL_ADC_GetConfigChannel(hadc, ADC_REGULAR_RANK_1, &orig_sConfig);
  // orig_rank = orig_sConfig.Rank;

  // ?????????????
  sConfig.Channel = ch;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
  if (HAL_ADC_ConfigChannel(hadc, &sConfig) != HAL_OK) // ???????
  {
    // taskENTER_CRITICAL(); // ???????????��?FreeRTOS?????
    g_b_irq_adc_pause = 0;
    // taskEXIT_CRITICAL();// ???????????��?FreeRTOS?????
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

  // // ֻ����һ��ADC�����Ч��
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
  // �ָ�ԭ����ͨ������
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

//  // �����ﶨ��洢ADC���������
// ?uint16_t adc_results[4]; // ����4��ͨ����ADCֵ
// ?���ú�����ȡ����ͨ��
// Read_ADC1_AllChannel(adc_results);

// // ��ȡ֮����Ϳ���ʹ�����ĸ�ֵ�ˣ������ӡ��ʾ��OLED
// OLED_Clear();
// OLED_ShowNum(0, 2, adc_results[0], 4, 16); // ��ʾͨ��7��ֵ

void Read_ADC1_AllChannel(uint16_t *adcValues) // ��ȡ����ͨ����ADCֵ
{
  HAL_ADC_Start(&hadc1);
  if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK) // �ȴ�ת�����

  {
    adcValues[0] = HAL_ADC_GetValue(&hadc1); // ��ȡͨ��0��ADCֵ
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
// /* USER CODE END Init */
// void HAL_ADC_MspInit(ADC_HandleTypeDef *adcHandle)
// {

//   GPIO_InitTypeDef GPIO_InitStruct = {0};
//   if (adcHandle->Instance == ADC1)

//   {
//     /* ADC1 clock enable */
//     __HAL_RCC_ADC1_CLK_ENABLE();

//     __HAL_RCC_GPIOA_CLK_ENABLE();
//     /**ADC1 GPIO Configuration
//     PA2     ------> ADC1_IN2
//     PA5     ------> ADC1_IN5
//     PA7     ------> ADC1_IN7
//     */
//     GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_5 | GPIO_PIN_7;
//     GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
//     HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

//     /* USER CODE BEGIN ADC1_MspInit 1 */

//     GPIO_InitStruct.Pin = GPIO_PIN_3;
//     GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
//     HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

//     GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
//     GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
//     HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

//     /* USER CODE END ADC1_MspInit 1 */
//   }
// }

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

//////======多/单通道ADC——DMA配置======
#include "adc.h"

ADC_HandleTypeDef adc_handle = {0}; // ADC句柄
DMA_HandleTypeDef dma_handle = {0}; // DMA句柄
//// 配置ADC（ADC1）
void adc_config(void)
{
  adc_handle.Instance = ADC1;                      // 选择ADC1外设
  adc_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT; // 数据对齐方式：右对齐（12位ADC数据低12位有效，高4位补0）
  /*多通道*/
  adc_handle.Init.ScanConvMode = ADC_SCAN_ENABLE; // 扫描转换模式：启用（扫描多个通道）
  /*单通道*/
  // adc_handle.Init.ScanConvMode = ADC_SCAN_DISABLE;       // 扫描转换模式：禁用（单通道转换）
  adc_handle.Init.ContinuousConvMode = ENABLE; // 连续转换模式：启用（一次触发后连续自动采集）
  /*多通道*/
  adc_handle.Init.NbrOfConversion = 4; // 规则通道总数：4（后面配置了4个通道CH0-CH3）

  ///*单通道*/
  // adc_handle.Init.NbrOfConversion = 1;                   // 规则通道总数：1
  adc_handle.Init.DiscontinuousConvMode = DISABLE;       // 间断模式：禁用（不允许间断采集）
  adc_handle.Init.NbrOfDiscConversion = 0;               // 间断模式通道数：0（禁用时无意义）
  adc_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START; // 外部触发源：软件触发（调用HAL_ADC_Start启动转换
  HAL_ADC_Init(&adc_handle);                             // 初始化ADC1

  HAL_ADCEx_Calibration_Start(&adc_handle); // 启动ADC自校准（消除ADC内部偏置误差，提高转换精度）
}
// 配置ADC通道
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{

  if (hadc->Instance == ADC1) // 判断是否为ADC1
  {
    RCC_PeriphCLKInitTypeDef adc_clk_init = {0}; //// ADC时钟配置结构体
    GPIO_InitTypeDef gpio_init_struct = {0};     //// GPIO配置结构体

    __HAL_RCC_ADC1_CLK_ENABLE();  // 使能ADC1时钟
    __HAL_RCC_GPIOA_CLK_ENABLE(); // 使能GPIOA时钟（ADC通道引脚所在端口）

    // 配置GPIO引脚为模拟输入模式（ADC通道0-3）
    /*多通道*/
    gpio_init_struct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
    // /*单通道*/
    // gpio_init_struct.Pin = GPIO_PIN_1;
    gpio_init_struct.Mode = GPIO_MODE_ANALOG; // 模拟输入模式
    HAL_GPIO_Init(GPIOA, &gpio_init_struct);  // 初始化GPIOA

    // 配置ADC时钟源
    adc_clk_init.PeriphClockSelection = RCC_PERIPHCLK_ADC; // 选择ADC外设时钟
    adc_clk_init.AdcClockSelection = RCC_ADCPCLK2_DIV6;    // ADC时钟 = PCLK2 / 6 = 72MHz / 6 = 12MHz
    HAL_RCCEx_PeriphCLKConfig(&adc_clk_init);              // 配置ADC时钟
  }
}

//// 配置DMA（用于ADC数据传输）
void dma_config(void)
{
  __HAL_RCC_DMA1_CLK_ENABLE();                      // 使能DMA1时钟
  dma_handle.Instance = DMA1_Channel1;              // 选择DMA1通道1（ADC1使用DMA1通道1）
  dma_handle.Init.Direction = DMA_PERIPH_TO_MEMORY; // 数据传输方向：外设到内存（ADC -> RAM）

  //  // 内存数据对齐：半字（16位），与ADC 12位数据匹配
  dma_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
  dma_handle.Init.MemInc = DMA_MINC_ENABLE; // 内存地址增量：启用（每次传输后内存地址+2，指向下一个数据）

  //// 外设数据对齐：半字（16位）
  dma_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
  dma_handle.Init.PeriphInc = DMA_PINC_DISABLE; // 外设地址增量：禁用（ADC数据寄存器地址固定）

  dma_handle.Init.Priority = DMA_PRIORITY_MEDIUM; // DMA优先级：中等
  dma_handle.Init.Mode = DMA_CIRCULAR;            // DMA模式：循环模式（数据覆盖传输，连续不断采集）
  HAL_DMA_Init(&dma_handle);                      // 初始化DMA

  __HAL_LINKDMA(&adc_handle, DMA_Handle, dma_handle); // 将DMA与ADC关联（ADC的DMA_Handle指向dma_handle）

  // 添加：使能DMA中断
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);// 使能DMA1通道1中断，用于接收DMA传输完成中断
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);// 设置DMA1通道1中断优先级为0，中断服务函数为DMA1_Channel1_IRQHandler
}

// 配置ADC通道
void adc_channel_config(ADC_HandleTypeDef *hadc, uint32_t ch, uint32_t rank, uint32_t stime)
{
  ADC_ChannelConfTypeDef adc_ch_config = {0}; // ADC通道配置结构体

  adc_ch_config.Channel = ch;                  // 通道号（如ADC_CHANNEL_0）
  adc_ch_config.Rank = rank;                   // 通道顺序（ADC_REGULAR_RANK_1~4）
  adc_ch_config.SamplingTime = stime;          // 采样时间（如ADC_SAMPLETIME_239CYCLES_5）
  HAL_ADC_ConfigChannel(hadc, &adc_ch_config); // 配置ADC通道
}

// 配置4个ADC通道（PA0-PA3）
void adc_dma_init(uint32_t *mar)
{
  adc_config(); // 1. 配置ADC基本参数（时钟、GPIO、模式等）

  // 2. 配置4个ADC通道（PA0-PA3）
  /*多通道*/
  adc_channel_config(&adc_handle, ADC_CHANNEL_0, ADC_REGULAR_RANK_1, ADC_SAMPLETIME_239CYCLES_5);
  adc_channel_config(&adc_handle, ADC_CHANNEL_1, ADC_REGULAR_RANK_2, ADC_SAMPLETIME_239CYCLES_5);
  adc_channel_config(&adc_handle, ADC_CHANNEL_2, ADC_REGULAR_RANK_3, ADC_SAMPLETIME_239CYCLES_5);
  adc_channel_config(&adc_handle, ADC_CHANNEL_3, ADC_REGULAR_RANK_4, ADC_SAMPLETIME_239CYCLES_5);
  ///*单通道*/
  // adc_channel_config(&adc_handle, ADC_CHANNEL_1, ADC_REGULAR_RANK_1, ADC_SAMPLETIME_239CYCLES_5);

  dma_config(); // 3. 配置DMA（数据传输通道、循环模式等）
  // 4. 启动ADC DMA传输（mar: 内存地址，4: 4个通道）
  // 此函数调用后ADC开始连续转换，数据自动存入指定内存
  /*多通道*/
  HAL_ADC_Start_DMA(&adc_handle, mar, 4);
  ///*单通道*/
  // HAL_ADC_Start_DMA(&adc_handle, mar, 1);
}
