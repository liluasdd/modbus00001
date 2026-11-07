#ifndef __DELAY_H__
#define __DELAY_H__

#include "sys.h"

void delay_init(void);        /* 初始化延时 */
void DWT_Init(void);          /* 初始化 DWT */
void delay_ms(uint16_t nms);  /* 延时nms */
void delay_us(uint16_t nus);  /* 延时nus */
void delay_s(uint32_t ns);    /* 延时ns */
void HAL_Delay(uint32_t nms); /* 延时nms */

#endif
