#include "adc.h"
#include "main.h"

volatile u8 g_adc_done_flag = 0;
u16 g_bat_v = 0;
// u8 adc_DMA_num = 0;
uint16_t adc_ch0[adc_DMA_max];
uint16_t adc_ch1[adc_DMA_max];
// uint16_t adc_ch2[adc_DMA_max];
// uint16_t adc_ch3[adc_DMA_max];
ADC_HandleTypeDef hadc1;

/* ADC1 init function */
ADC_ChannelConfTypeDef sConfig = {0};
void MX_ADC1_Init(void)
{
  hadc1.Instance = ADC1;
  // hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;// 单通道转换模式
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;        // 扫描转换模式
  hadc1.Init.ContinuousConvMode = DISABLE;          // 连续转换模式关闭
  hadc1.Init.DiscontinuousConvMode = DISABLE;       // 断续转换模式关闭
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START; // 软换触发源
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;       // 数据对齐方式
  // hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.NbrOfConversion = 1;     // 转换次数
  if (HAL_ADC_Init(&hadc1) != HAL_OK) // 初始化ADC1
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
  // ????????????????????????????
  ADC_ChannelConfTypeDef orig_sConfig = {0};
  // uint32_t orig_rank;
  // taskENTER_CRITICAL(); // // ???????????????????????????????FreeRTOS?????
  g_b_irq_adc_pause = 1;
  // taskEXIT_CRITICAL();// // ??????????????????????????????FreeRTOS?????
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
  uint8_t valid_cnt = 0; // ????????????

  // ???????????????????????????????ж???????
  // HAL_ADC_GetConfigChannel(hadc, ADC_REGULAR_RANK_1, &orig_sConfig);
  // orig_rank = orig_sConfig.Rank;

  // ?????????????
  sConfig.Channel = ch;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
  if (HAL_ADC_ConfigChannel(hadc, &sConfig) != HAL_OK) // ???????
  {
    // taskENTER_CRITICAL(); // ??????????????FreeRTOS?????
    g_b_irq_adc_pause = 0;
    // taskEXIT_CRITICAL();// ??????????????FreeRTOS?????
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

  // // ????????ADC?????Ч??
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
  // ???????????????
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

//  // ????????洢ADC?????????
// ?uint16_t adc_results[4]; // ????4???????ADC?
// ????ú?????????????
// Read_ADC1_AllChannel(adc_results);

// // ???????????????????????????????????OLED
// OLED_Clear();
// OLED_ShowNum(0, 2, adc_results[0], 4, 16); // ??????7???

void Read_ADC1_AllChannel(uint16_t *adcValues) // ????????????ADC?
{
  HAL_ADC_Start(&hadc1);
  if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK) // ?????????

  {
    adcValues[0] = HAL_ADC_GetValue(&hadc1); // ??????0??ADC?
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
DMA_HandleTypeDef hdma_adc1 = {0};  // 全局定义
// DMA_HandleTypeDef dma_handle = {0}; // DMA句柄

// 配置ADC通道
void adc_channel_config(ADC_HandleTypeDef *hadc, uint32_t ch, uint32_t rank, uint32_t stime)
{
  ADC_ChannelConfTypeDef adc_ch_config = {0}; // ADC通道配置结构体

  adc_ch_config.Channel = ch;                  // 通道号（如ADC_CHANNEL_0）
  adc_ch_config.Rank = rank;                   // 通道顺序（ADC_REGULAR_RANK_1~4）
  adc_ch_config.SamplingTime = stime;          // 采样时间（如ADC_SAMPLETIME_239CYCLES_5）
  HAL_ADC_ConfigChannel(hadc, &adc_ch_config); // 配置ADC通道
}

//// 配置ADC（ADC1）
void adc_config(void)
{
  // 初始化GPIO结构体
  GPIO_InitTypeDef gpio_initstruct = {0};
  gpio_initstruct.Pin = GPIO_PIN_4;
  gpio_initstruct.Mode = GPIO_MODE_ANALOG; // 配置为模拟输入
  gpio_initstruct.Pull = GPIO_NOPULL;      // 不使用上拉下拉
  HAL_GPIO_Init(GPIOA, &gpio_initstruct);
  gpio_initstruct.Pin = GPIO_PIN_5;
  HAL_GPIO_Init(GPIOA, &gpio_initstruct);
  HAL_Delay(1); //
  ///* 配置ADC1方案一 */
  __HAL_RCC_ADC1_CLK_ENABLE();                     // 使能ADC1时钟
  adc_handle.Instance = ADC1;                      // 选择ADC1外设
  adc_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT; // 数据对齐方式：右对齐（12位ADC数据低12位有效，高4位补0）
  adc_handle.Init.ScanConvMode = ADC_SCAN_ENABLE;  // 扫描转换模式：启用（扫描多个通道）
  // adc_handle.Init.ContinuousConvMode = ENABLE; // 连续转换
  adc_handle.Init.ContinuousConvMode = DISABLE;    // 关连续转换
  adc_handle.Init.DiscontinuousConvMode = DISABLE; // 关间断转换
  // adc_handle.Init.NbrOfConversion = 4; // 4个通道
  adc_handle.Init.NbrOfConversion = 3; // 3个通道
  // adc_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START; // 软件触发
  adc_handle.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T3_TRGO; // 以 TIM3 为例
  // adc_handle.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T2_CC2; // 以 TIM2 为例
  // HAL_ADCEx_Calibration_Start(&adc_handle);
  HAL_ADC_Init(&adc_handle);
  HAL_ADCEx_Calibration_Start(&adc_handle);
  // 配置2个通道
  adc_channel_config(&adc_handle, PT10_ADC, ADC_REGULAR_RANK_1, ADC_SAMPLETIME_41CYCLES_5);
  adc_channel_config(&adc_handle, PT100_ADC, ADC_REGULAR_RANK_2, ADC_SAMPLETIME_41CYCLES_5);
  adc_channel_config(&adc_handle, ADC_CHANNEL_BATT, ADC_REGULAR_RANK_3, ADC_SAMPLETIME_239CYCLES_5);

  // /*配置ADC1方案二*/
  // adc_handle.Instance = ADC1;                      // 选择ADC1外设
  // adc_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT; // 数据对齐方式：右对齐（12位ADC数据低12位有效，高4位补0）
  // /*多通道*/
  // adc_handle.Init.ScanConvMode = ADC_SCAN_ENABLE; // 扫描转换模式：启用（扫描多个通道）
  // /*单通道*/
  // // adc_handle.Init.ScanConvMode = ADC_SCAN_DISABLE;       // 扫描转换模式：禁用（单通道转换）
  // adc_handle.Init.ContinuousConvMode = ENABLE; // 连续转换模式：启用（一次触发后连续自动采集）
  // /*多通道*/
  // // adc_handle.Init.NbrOfConversion = 4; // 规则通道总数：4（后面配置了4个通道CH0-CH3）
  // adc_handle.Init.NbrOfConversion = 2; // 规则通道总数：2（后面配置了2个通道CH0-CH3）
  // ///*单通道*/
  // // adc_handle.Init.NbrOfConversion = 1;                   // 规则通道总数：1
  // adc_handle.Init.DiscontinuousConvMode = DISABLE;       // 间断模式：禁用（不允许间断采集）
  // adc_handle.Init.NbrOfDiscConversion = 0;               // 间断模式通道数：0（禁用时无意义）
  // adc_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START; // 外部触发源：软件触发（调用HAL_ADC_Start启动转换
  // HAL_ADC_Init(&adc_handle);                             // 初始化ADC1
  // HAL_ADCEx_Calibration_Start(&adc_handle);              // 启动ADC自校准（消除ADC内部偏置误差，提高转换精度）
  // adc_channel_config(&adc_handle, PT10_ADC, ADC_REGULAR_RANK_1, ADC_SAMPLETIME_239CYCLES_5);
  // adc_channel_config(&adc_handle, PT100_ADC, ADC_REGULAR_RANK_2, ADC_SAMPLETIME_239CYCLES_5);
}
/*配置ADC通道， HAL_ADC_Init 自动调用不需要手动调用*/
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
  if (hadc->Instance == ADC1) // 判断是否为ADC1
  {
    RCC_PeriphCLKInitTypeDef adc_clk_init = {0}; //// ADC时钟配置结构体
    GPIO_InitTypeDef gpio_init_struct = {0};     //// GPIO配置结构体

    /*外部未配置时，需要配置ADC通道引脚为模拟输入模式*/
    __HAL_RCC_ADC1_CLK_ENABLE();  // 使能ADC1时钟  (配置调用前已使能了)
    __HAL_RCC_GPIOA_CLK_ENABLE(); // 使能GPIOA时钟（GPIO已经初始化了）
    /*多通道*/
    gpio_init_struct.Pin = GPIO_PIN_4 | GPIO_PIN_5; // 请在进入前配置ADC通道引脚为模拟输入模式
    gpio_init_struct.Mode = GPIO_MODE_ANALOG;       // 模拟输入模式
    HAL_GPIO_Init(GPIOA, &gpio_init_struct);        // 初始化GPIOA

    // 配置ADC时钟源
    adc_clk_init.PeriphClockSelection = RCC_PERIPHCLK_ADC; // 选择ADC外设时钟
    adc_clk_init.AdcClockSelection = RCC_ADCPCLK2_DIV6;    // ADC时钟 = PCLK2 / 6 = 72MHz / 6 = 12MHz
    HAL_RCCEx_PeriphCLKConfig(&adc_clk_init);              // 配置ADC时钟
  }
}

//// 配置DMA（用于ADC数据传输）
void dma_config(void)
{
  __HAL_RCC_DMA1_CLK_ENABLE();
  hdma_adc1.Instance = DMA1_Channel1;              // 选择DMA1通道1外设
  hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY; // DMA方向（外设到内存）

  // 内存相关配置
  hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD; // 内存数据对齐方式（半字对齐）
  hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;                   // 内存地址自动递增（每个传输后内存地址增加1）

  // 外设相关配置
  hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD; // 外设数据对齐方式（半字对齐）
  hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;                  // 外设地址不自动递增（每个传输后外设地址不变）

  hdma_adc1.Init.Priority = DMA_PRIORITY_MEDIUM;     // DMA优先级（中）
  hdma_adc1.Init.Mode = DMA_CIRCULAR;                // DMA模式（循环模式）
  HAL_DMA_Init(&hdma_adc1);                          // 初始化DMA1通道1
  __HAL_LINKDMA(&adc_handle, DMA_Handle, hdma_adc1); // 将DMA与ADC关联（ADC的DMA_Handle指向hdma_adc1）

  /* ---------- 新增：开启 DMA 中断 ---------- */
  // 半传输中断（Half Transfer）→ 触发 ConvHalfCpltCallback
  // 传输完成中断（Transfer Complete）→ 触发 ConvCpltCallback
  __HAL_DMA_ENABLE_IT(&hdma_adc1, DMA_IT_HT); // ← 关键，别漏
  __HAL_DMA_ENABLE_IT(&hdma_adc1, DMA_IT_TC);

// NVIC 配置
#if freertos_modbus
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
#else
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
#endif
}

// 配置4个ADC通道（PA0-PA3）
// void adc_dma_init(uint32_t *mar)
void adc_dma_init(uint16_t *mar)
{
  // adc_config(); // 1. 配置ADC基本参数（时钟、GPIO、模式等）
  // // 2. 配置4个ADC通道（PA0-PA3） 方案二时启用
  // // /*多通道*/
  // // adc_channel_config(&adc_handle, ADC_CHANNEL_0, ADC_REGULAR_RANK_1, ADC_SAMPLETIME_239CYCLES_5);
  // // adc_channel_config(&adc_handle, ADC_CHANNEL_1, ADC_REGULAR_RANK_2, ADC_SAMPLETIME_239CYCLES_5);
  // // adc_channel_config(&adc_handle, ADC_CHANNEL_2, ADC_REGULAR_RANK_3, ADC_SAMPLETIME_239CYCLES_5);
  // // adc_channel_config(&adc_handle, ADC_CHANNEL_3, ADC_REGULAR_RANK_4, ADC_SAMPLETIME_239CYCLES_5);
  // ///*单通道*/
  // // adc_channel_config(&adc_handle, ADC_CHANNEL_1, ADC_REGULAR_RANK_1, ADC_SAMPLETIME_239CYCLES_5);
  // dma_config(); // 3. 配置DMA（数据传输通道、循环模式等）
  // // 4. 启动ADC DMA传输（mar: 内存地址，4: 4个通道）
  // // 此函数调用后ADC开始连续转换，数据自动存入指定内存
  // /*多通道*/
  // HAL_ADC_Start_DMA(&adc_handle, (uint32_t *)mar, 40); // 启动ADC DMA传输，4个通道（共四个数据），数据自动存入指定内存
  // ///*单通道*/
  // // HAL_ADC_Start_DMA(&adc_handle, mar, 1);
  // //  HAL_ADC_Stop_DMA(&adc_handle);// 停止ADC DMA传输，释放DMA资源

  adc_config();                                                 // 配置ADC基本参数（时钟、GPIO、模式等）
  dma_config();                                                 // 配置DMA（数据传输通道、循环模式等）
  HAL_ADC_Start_DMA(&adc_handle, (uint32_t *)mar, ADC_BUF_LEN); // 启动ADC DMA传输，4个通道（共四个数据），数据自动存入指定内存
}

void start_adc(void) // 启动ADC DMA传输
{
  // adc_DMA_num = 0;
  // HAL_ADC_Start_DMA(&adc_handle, (uint32_t *)(void *)adc_result, ADC_BUF_LEN); // 启动ADC DMA传输，4个通道（共四个数据），数据自动存入指定内存

  HAL_ADC_Start_DMA(&adc_handle, (uint32_t *)adc_result, ADC_BUF_LEN);
  // HAL_TIM_Base_Start(&htim2);
  HAL_TIM_Base_Start(&htim3);
}

void stop_adc(void) // 停止ADC DMA传输
{
  HAL_ADC_Stop_DMA(&adc_handle); // 停止ADC DMA传输，释放DMA资源

  // // 彻底关掉 TIM2 的时钟（它连转都不会转）
  // __HAL_RCC_TIM2_CLK_DISABLE();
  // // 彻底关掉 TIM2 的 NVIC 中断通道（它想进都进不来）
  // HAL_NVIC_DisableIRQ(TIM2_IRQn);
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc) // ADC+DMA转换半完成回调函数
{
  if (hadc->Instance == ADC1)
    g_adc_done_flag = 1;
}
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) // ADC+DMA转换完成回调函数
{
  if (hadc->Instance == ADC1)
    g_adc_done_flag = 2;
}

u16 ADC_CHi_NUM1;
u16 ADC_CHi_NUM2;
u16 ADC_CHi_NUM3;

u16 ADC_CHi_tmp1;
u16 ADC_CHi_tmp2;
u16 ADC_CHi_tmp3;

uint8_t R_DisDuty;
uint8_t OLED_DisDuty;

uint8_t R_Task = 0;
uint16_t adc_dma_result_min1 = 0xFFFF;
uint16_t adc_dma_result_max1 = 0;
uint16_t adc_dma_result_min2 = 0xFFFF;
uint16_t adc_dma_result_max2 = 0;
uint16_t adc_dma_result_min3 = 0xFFFF;
uint16_t adc_dma_result_max3 = 0;
/* 输入寄存器缓冲区（只读） */
extern USHORT usSRegInBuf[S_REG_INPUT_NREGS];

// typedef struct
// {
//   uint16_t ch1;
//   uint16_t ch2;
//   // uint32_t tick;
// } ADC_Msg_t;
// ADC_Msg_t msg;

void ADC_DMA_ProcessChannels(void) // 处理ADC通道数据
{
  uint8_t i = 0;
  uint16_t base, v0, v1, v2;
  OLED_DisDuty = 0;
  if (g_adc_done_flag != 1 && g_adc_done_flag != 2)
    return;
  base = (g_adc_done_flag == 1) ? 0 : ADC_GROUP_PER_HALF;

  adc_dma_result_max1 = 0;
  adc_dma_result_min1 = 0xFFFF;
  adc_dma_result_max2 = 0;
  adc_dma_result_min2 = 0xFFFF;
  adc_dma_result_max3 = 0;
  adc_dma_result_min3 = 0xFFFF;
  ADC_CHi_NUM1 = 0;
  ADC_CHi_NUM2 = 0;
  ADC_CHi_NUM3 = 0;
  for (i = 0; i < ADC_GROUP_PER_HALF; i++)
  {
    v0 = adc_result[base + i][0];
    v1 = adc_result[base + i][1];
    v2 = adc_result[base + i][2];

    ADC_CHi_NUM1 += v0;
    ADC_CHi_NUM2 += v1;
    ADC_CHi_NUM3 += v2;

    if (v0 < adc_dma_result_min1)
      adc_dma_result_min1 = v0;
    if (v0 > adc_dma_result_max1)
      adc_dma_result_max1 = v0;
    if (v1 < adc_dma_result_min2)
      adc_dma_result_min2 = v1;
    if (v1 > adc_dma_result_max2)
      adc_dma_result_max2 = v1;
    if (v2 < adc_dma_result_min3)
      adc_dma_result_min3 = v2;
    if (v2 > adc_dma_result_max3)
      adc_dma_result_max3 = v2;
  }

  ADC_CHi_NUM1 = (ADC_CHi_NUM1 - adc_dma_result_max1 - adc_dma_result_min1) / (ADC_GROUP_PER_HALF - 2);
  ADC_CHi_NUM2 = (ADC_CHi_NUM2 - adc_dma_result_max2 - adc_dma_result_min2) / (ADC_GROUP_PER_HALF - 2);
  ADC_CHi_NUM3 = (ADC_CHi_NUM3 - adc_dma_result_max3 - adc_dma_result_min3) / (ADC_GROUP_PER_HALF - 2);

  if (ADC_CHi_tmp1 != ADC_CHi_NUM1)
  {
    OLED_DisDuty |= 0x01;
    // msg.ch1 = ADC_CHi_NUM1;
    usSRegInBuf[0] = ADC_CHi_NUM1; // 通道1 // 直接赋值modbus寄存器
    ADC_CHi_tmp1 = ADC_CHi_NUM1;
  }
  if (ADC_CHi_tmp2 != ADC_CHi_NUM2)
  {
    OLED_DisDuty |= 0x02;
    // msg.ch2 = ADC_CHi_NUM2;
    usSRegInBuf[1] = ADC_CHi_NUM2; // 通道2 // 直接赋值modbus寄存器
    ADC_CHi_tmp2 = ADC_CHi_NUM2;
  }
  if (ADC_CHi_tmp3 != ADC_CHi_NUM3)
  {
    OLED_DisDuty |= 0x04;
    // msg.ch3 = ADC_CHi_NUM3;
    usSRegInBuf[2] = ADC_CHi_NUM3; // 通道3 // 直接赋值modbus寄存器
    ADC_CHi_tmp3 = ADC_CHi_NUM3;
  }
  // xQueueSend(queue, &msg, 0);
  g_adc_done_flag = 0;
}

u16 read_bat_vol(void)
{
  return JS_BATZ(ADC_CHi_NUM3);
}

void Get_BatActaul(void)
{
  // if (g_b_Bat_Check == 1)
  {
    g_b_Bat_Check = 0;
    // R_Bat.Actual = read_bat_vol(); // 获取电池实际电压
    // g_bat_v = R_Bat.Actual;   // 电池实际电压
    g_bat_v = read_bat_vol(); // 获取电池实际电压
  }
}

void ADC_DMA_scan(void)
{
  char str[6];

  // if (R_Task != D_Task2)
  //     return;
  if (R_DisDuty == 0)
  {
		R_DisDuty++;
    ADC_CHi_tmp1 = 0xffff;
    ADC_CHi_tmp2 = 0xffff;
		ADC_CHi_tmp3 = 0xffff;
    Show_Str(5, 5, "AD1:0000", 16, 0);
    Show_Str(80, 5, "AD2:0000", 16, 0);
    Show_Str(155, 5, "AD3:0000", 16, 0);
    Show_Str(155, 25, "BAT:0000", 16, 0);
  }

  ADC_DMA_ProcessChannels();
  Get_BatActaul();

  // if (ADC_CHi_tmp1 != ADC_CHi_NUM1) //
  if (OLED_DisDuty & 0x01)
  {
    usSRegInBuf[0] = ADC_CHi_NUM1; // 通道1 // 直接赋值modbus寄存器
    ADC_CHi_tmp1 = ADC_CHi_NUM1;
    u16_to_4digits(str, ADC_CHi_NUM1, 0);
    Show_Str(37, 5, (u8 *)str, 16, 0);
  }
  // if (ADC_CHi_tmp2 != ADC_CHi_NUM2)
  if (OLED_DisDuty & 0x02)
  {
    usSRegInBuf[1] = ADC_CHi_NUM2; // 通道2 // 直接赋值modbus寄存器
    ADC_CHi_tmp2 = ADC_CHi_NUM2;
    u16_to_4digits(str, ADC_CHi_NUM2, 0);
    Show_Str(112, 5, (u8 *)str, 16, 0);
		Log_Send(LOG_INFO, "ADC2:%d", ADC_CHi_NUM2);
  }
	
  // if (ADC_CHi_tmp3 != ADC_CHi_NUM3)
  if (OLED_DisDuty & 0x04)
  {
    usSRegInBuf[2] = ADC_CHi_NUM3; // 通道3 // 直接赋值modbus寄存器
    ADC_CHi_tmp3 = ADC_CHi_NUM3;
    u16_to_4digits(str, ADC_CHi_NUM3, 0);
    Show_Str(187, 5, (u8 *)str, 16, 0);
    u16_to_4digits(str, g_bat_v, 1);
    usSRegInBuf[3] = g_bat_v; // 通道3 // 直接赋值modbus寄存器
    Show_Str(187, 25, (u8 *)str, 16, 0);
    g_adc_done_flag = 0;
  }
}
