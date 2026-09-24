#ifndef __MAIN_H__
#define __MAIN_H__

#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"


#include "usertype.h"

///* 包含用户bus应用头文件 */
#include "user_mb_app.h"
///*打印头文件 */
#include <stdio.h>

#include "sys.h"
#include "delay.h"
#include "uart.h"

#include "gpio.h"
#include "usart.h"
#include "adc.h"
#include "key.h"

#include "tim.h"

#include "mb.h"
#include "mbport.h"
#include "user_mb_app.h"

//////lcd
#include "lcd.h"
// #include "font.h"

extern TIM_HandleTypeDef htim4;
extern UART_HandleTypeDef huart2;

extern void prvvUARTTxReadyISR(void);
extern void prvvUARTRxISR(void);
extern void prvvTIMERExpiredISR(void);

static void SystemClock_Config(void);
void Error_Handler(void);

/* 测试功能参数 */
#define MB_SAMPLE_TEST_SLAVE_ADDR 1 // 从机设???地址

#endif
