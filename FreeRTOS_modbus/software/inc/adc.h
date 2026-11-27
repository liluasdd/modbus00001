#ifndef __ADC_H__
#define __ADC_H__

#include "sys.h"

#define JS_BATZ(a)       ((4095*120) /a)
#define JS_BATZ(a)       ((4095*120) /a)

// void adc_init(void);
extern void MX_ADC1_Init(void);
extern int16_t Read_ADC(/*uint8_t adc_id, */ uint32_t ch, uint8_t num);
// void adc_dma_init(uint32_t *mar);
extern void adc_dma_init(uint16_t *mar);
extern void start_adc(void);
extern void ADC_DMA_scan(void);
extern void ADC_DMA_ProcessChannels(void);
// int16_t Read_ADC(uint32_t ch, uint32_t times);
// uint32_t adc_get_result(uint32_t ch);

#endif
