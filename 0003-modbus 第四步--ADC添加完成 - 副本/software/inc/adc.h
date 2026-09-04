#ifndef __ADC_H__
#define __ADC_H__

#include "sys.h"

// void adc_init(void);
void MX_ADC1_Init(void);
int16_t Read_ADC(/*uint8_t adc_id, */ uint32_t ch, uint8_t num);
// int16_t Read_ADC(uint32_t ch, uint32_t times);
// uint32_t adc_get_result(uint32_t ch);

#endif
