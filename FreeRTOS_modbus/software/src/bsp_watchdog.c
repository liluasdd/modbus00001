#include "bsp_watchdog.h"

#define WATCHDOG_MAX 20
uint8_t s_watchdog_tim = 0;
uint8_t s_watchdog_en = 0;
static IWDG_HandleTypeDef hiwdg;

void bsp_iwdgconfig(void)
{
  // 先稳定LSI
  __HAL_RCC_LSI_ENABLE(); // 使能LSI
  // 等待LSI稳定
  while (__HAL_RCC_GET_FLAG(RCC_FLAG_LSIRDY) == RESET)
    ;
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_64; // 64分频
  hiwdg.Init.Reload = 4000;                 //

  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    // 初始化失败，可以死循环或处理
    for (;;)
      ;
  }
}

void feed_watchdog(void)
{
  HAL_IWDG_Refresh(&hiwdg);
  // IWDG->KR = 0xAAAA; // 重装载计数器
}

// void watchdog_on(void)
//{
//     feed_watchdog();
//     s_watchdog_en = 1;
// }
// void watchdog_off(void)
//{
//     s_watchdog_en = 0;
// }
// void watchdog_reset(void)
//{
//     feed_watchdog();
//     s_watchdog_en = 1;
// }
