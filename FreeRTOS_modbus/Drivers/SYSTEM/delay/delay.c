#include "delay.h"
#include "FreeRTOSConfig.h"

/**
 * @brief  微秒级延时
 * @param  nus 延时时长，范围：0~233015
 * @retval 无
 */
// void delay_us(uint16_t nus)
// {
//     uint32_t temp;
//     SysTick->LOAD = 72 * nus;                           /* 设置定时器重装值 */
//     SysTick->VAL = 0x00;                                /* 清空当前计数值 */
//     SysTick->CTRL |= 1 << 2;                            /* 设置分频系数为1分频 */
//     SysTick->CTRL |= 1 << 0;                            /* 启动定时器 */
//     do
//     {
//         temp = SysTick->CTRL;
//     } while ((temp & 0x01) && !(temp & (1 << 16)));     /* 等待计数到0 */
//     SysTick->CTRL &= ~(1 << 0);                         /* 关闭定时器 */
// }

void delay_init(void)
{
    SysTick_Config(HAL_RCC_GetHCLKFreq()/configTICK_RATE_HZ);
}

// 初始化 DWT
void DWT_Init(void)
{
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CYCCNT = 0;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

// uint8_t DWT_Init_count = 0;
// 基于 DWT 的绝对安全微秒延时
void delay_us(uint16_t nus)
{
  // if (DWT_Init_count == 0)
  // {
  //   DWT_Init();
  //   DWT_Init_count++;
  // }
  
  uint32_t start = DWT->CYCCNT;
  uint32_t cycles = nus * (SystemCoreClock / 1000000);
  while ((DWT->CYCCNT - start) < cycles)
    ;
}

/**
 * @brief  毫秒级延时
 * @param  nms 延时时长，范围：0~4294967295
 * @retval 无
 */
void delay_ms(uint16_t nms)
{
  while (nms--)
    delay_us(1000);
}

/**
 * @brief  秒级延时
 * @param  ns 延时时长，范围：0~4294967295
 * @retval 无
 */
void delay_s(uint32_t ns)
{
  while (ns--)
    delay_ms(1000);
}

/**
 * @brief  重写HAL_Delay函数
 * @param  nms 延时时长，范围：0~4294967295
 * @retval 无
 */
void HAL_Delay(uint32_t nms)
{
  delay_ms(nms);
}
