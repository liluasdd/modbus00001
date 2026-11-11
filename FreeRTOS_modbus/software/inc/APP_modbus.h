#ifndef __APP_MODBUS_H
#define __APP_MODBUS_H

#include <stdint.h>

#define REG_NUM  20  // Modbus 寄存器总数

// 使用 extern 声明全局变量，让其他文件也能访问
extern uint16_t usRegBuf_A[REG_NUM];
extern uint16_t usRegBuf_B[REG_NUM];
extern uint16_t * volatile ptr_ReadBuf;
extern uint16_t * volatile ptr_WriteBuf;

// 声明一个写缓冲并交换的辅助函数，方便 ADC 任务调用
void Modbus_Update_Registers(uint16_t *new_data, uint16_t len);

#endif
