#ifndef __MAIN_H__
#define __MAIN_H__

#include "stm32f1xx_hal.h"

#include "user_type.h"


///* 包含用户bus应用头文件 */
#include "user_mb_app.h"
///*打印头文件 */
#include <stdio.h>

#include "gpio.h"
#include "adc.h"
#include "key.h"

extern TIM_HandleTypeDef htim4;
extern UART_HandleTypeDef huart2;

extern void prvvUARTTxReadyISR(void);
extern void prvvUARTRxISR(void);
extern void prvvTIMERExpiredISR(void);

static void SystemClock_Config(void);
void Error_Handler(void);

/* 测试功能参数 */
#define MB_SAMPLE_TEST_SLAVE_ADDR						1			//从机设备地址

#endif
