#ifndef __BKP_STORAGE_H
#define __BKP_STORAGE_H

#include "stm32f1xx_hal.h"
#include <string.h>

#include "stm32f1xx_hal.h"
#include <string.h>

/* 全局运行参数结构体 (仅用于 RAM 中快速运算) */
typedef struct {
    uint16_t sys_flags;       // 16位标志位集合
    float    adc_gain;        // ADC增益 (32位浮点)
    uint16_t bat_voltage;     // 电池电压 (16位整数，如放大100倍存)
    uint8_t  bat_level;       // 电量百分比 (8位)
    uint32_t output_count;    // 输出计数 (32位无符号整型)
} DeviceParams_t;

extern RTC_HandleTypeDef hrtc;

/* 定义 BKP 寄存器的偏移地址 (HAL库中 BKP 基地址为 0x40006C00) */
#define BKP_DR1_OFFSET    0x04
#define BKP_DR2_OFFSET    0x08
#define BKP_DR3_OFFSET    0x0C
#define BKP_DR4_OFFSET    0x10
#define BKP_DR5_OFFSET    0x14
#define BKP_DR6_OFFSET    0x18
#define BKP_DR7_OFFSET    0x1C
#define BKP_DR8_OFFSET    0x20
#define BKP_DR9_OFFSET    0x24
#define BKP_DR10_OFFSET   0x28

/* 定义魔数，用于判断 BKP 数据是否有效 (防止第一次上电时是随机值) */
#define BKP_MAGIC_NUM     0xA55A

/* 需要跨复位保存的参数结构体 (总大小不能超过 20 字节) */
typedef struct {
    uint16_t magic_number;   // 魔数 (2字节)
    uint8_t  modbus_addr;    // Modbus 从站地址 (1字节)
    uint8_t  reserved;       // 对齐保留 (1字节)
    uint16_t run_count;      // 运行次数或状态标记 (2字节)
    float    adc_offset;     // ADC 零点校准偏移量 (4字节)
    float    adc_gain;       // ADC 增益校准系数 (4字节)
} BkpParams_t;

/* 函数声明 */
void BKP_Unlock(void);
void BKP_SaveParams(BkpParams_t *params);
void BKP_LoadParams(BkpParams_t *params);
void BKP_Unlock_init(void);
void BKP_Unlock_Save(void);

#endif /* __BKP_STORAGE_H */
