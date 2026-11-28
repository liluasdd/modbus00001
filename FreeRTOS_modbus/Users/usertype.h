#ifndef __USER_TYPE_H
#define __USER_TYPE_H

#include "stdint.h" /* 包含标准整数类型 */

#define s64 int64_t //////* 64位有符号整数*/
#define s32 int32_t
#define s16 int16_t
#define s8 int8_t

#define sc64 const int64_t /*!< Read Only */ /*64位 有符号整数*/
#define sc32 const int32_t                   /*!< Read Only */
#define sc16 const int16_t                   /*!< Read Only */
#define sc8 const int8_t /*!< Read Only */   /*8位只读 有符号整数*/

#define u64 uint64_t //////* 64位无符号整数*/
#define u32 uint32_t
#define u16 uint16_t
#define u8 uint8_t // 8位无符号整数

#define uc64 const uint64_t /*!< Read Only */ /*64位无符号整数*/
#define uc32 const uint32_t                   /*!< Read Only */
#define uc16 const uint16_t                   /*!< Read Only */
#define uc8 const uint8_t /*!< Read Only */   /*8位只读 无符号保护重变量*/

#define vu64 volatile u64 //////* 64位无符号整数*/
#define vu32 volatile u32
#define vu16 volatile u16
#define vu8 volatile u8 //////* 8位易失性变量，每访问都从内存取*/

void APP_ErrorHandler(void);

typedef union /* 联合体，用于存储8位标志位 */
{
    unsigned char byte; /* 成员1：按“整字节”访问 */
    struct              /* 匿名结构体类型，作为成员 bits */
    {
        u8 bit0 : 1; /* 位域，8 个位拼成 1 字节 */
        u8 bit1 : 1;
        u8 bit2 : 1;
        u8 bit3 : 1;
        u8 bit4 : 1;
        u8 bit5 : 1;
        u8 bit6 : 1;
        u8 bit7 : 1;
    } bits; /* 成员2：按“单个位”访问 */
} bit_flag;
extern volatile bit_flag flag1, flag2, flag3, flag4, flag5, flag6, flag7, flag8, flag9;

#define g_b_task_jtim flag1.bits.bit0
#define g_b_100ms_jtim flag1.bits.bit1
#define g_b_2s_jtim flag1.bits.bit2
#define g_b_485_send_tick flag1.bits.bit3
#define g_b_485_send_Error flag1.bits.bit4
#define g_b_485_receive_Error flag1.bits.bit5
#define g_b_Bat_Check flag1.bits.bit6

#define g_b_power_onoff flag2.bits.bit0       /*电源开关*/
#define g_b_lock_machine flag2.bits.bit1      /*锁机*/
#define g_b_machine_tim_error flag2.bits.bit2 /*机器输超时错误*/
#define g_b_machine_outputtim_error flag2.bits.bit3 /*机器输出超时错误*/
#endif
