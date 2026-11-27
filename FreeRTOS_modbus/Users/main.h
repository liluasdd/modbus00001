#ifndef __MAIN_H__
#define __MAIN_H__

#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include "stdint.h"/* 包含标准整数类型 */

#include "usertype.h"


///* 包含用户bus应用头文件 */
#include "sys_monitor.h"/* 系统监控头文件 */
#include "user_mb_app.h"
///*打印头文件 */
#include <stdio.h>
#include "semphr.h" // 提供互斥锁相关的 API
#include <stdarg.h> // 提供 va_list, va_start, va_end
#include <stdio.h>  // 提供 vprintf
#include <string.h> // 提供 memset

#include "uart.h"

#include "sys.h"
#include "FreeRTOS.h"
#include "delay.h"

#include "gpio.h"
#include "usart.h"
#include "adc.h"
#include "key.h"

#include "tim.h"

#include "mb.h"
#include "mbport.h"
#include "mbrtu.h"
#include "user_mb_app.h"
#include "APP_modbus.h"

#include "bsp_watchdog.h" /* 包含看门狗头文件*/

/////* lcd，led，oled相关 */
#include "lcd.h"
#include "led.h"
#include "oled.h"

#include "log_queue.h"

// #include "font.h"

extern TIM_HandleTypeDef htim4;
extern UART_HandleTypeDef huart2;
extern uint8_t R_Task;
extern uint8_t R_DisDuty;
extern QueueHandle_t queue;

extern u16 g_bat_v;

extern void prvvUARTTxReadyISR(void);
extern void prvvUARTRxISR(void);
extern void prvvTIMERExpiredISR(void);

extern void freertos_test(void);
static void SystemClock_Config(void);
void Error_Handler(void);

/* 测试功能参数 */
#define MB_SAMPLE_TEST_SLAVE_ADDR 1 // 从机设???地址

#endif
