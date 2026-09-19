#ifndef __USER_TYPE_H
#define __USER_TYPE_H

#include "stdint.h"

#define s64 int64_t  //////* 64位有符号整数*/
#define s32 int32_t
#define s16 int16_t
#define s8 int8_t

#define sc64 const int64_t /*!< Read Only */  //////* 64位 只读有符号整数*/
#define sc32 const int32_t /*!< Read Only */
#define sc16 const int16_t /*!< Read Only */
#define sc8 const int8_t   /*!< Read Only */

#define u64 uint64_t  //////* 64位无符号整数*/
#define u32 uint32_t
#define u16 uint16_t
#define u8 uint8_t  //普通字节数据、计数器， 8位无符号整数

#define uc64 const uint64_t /*!< Read Only */  //////*64位无符号整数*/
#define uc32 const uint32_t /*!< Read Only */
#define uc16 const uint16_t /*!< Read Only */
#define uc8 const uint8_t   /*!< Read Only */  //////* 8位只读 保护重要数据不被意外修改，如配置参数、查找表。*/

#define vu64 volatile u64  //////* 64位 无优化 无符号整数*/
#define vu32 volatile u32
#define vu16 volatile u16
#define vu8 volatile u8  //////* 8位 易失性变量，每次都从内存读取*/


void APP_ErrorHandler(void);

typedef union
{
    unsigned char byte;
    struct
    {
        u8 bit0 : 1;
        u8 bit1 : 1;
        u8 bit2 : 1;
        u8 bit3 : 1;
        u8 bit4 : 1;
        u8 bit5 : 1;
        u8 bit6 : 1;
        u8 bit7 : 1;
    } bits;
} bit_flag;
extern volatile bit_flag flag1, flag2, flag3, flag4, flag5, flag6, flag7;

#define g_b_task_jtim flag1.bits.bit0
#define g_b_100ms_jtim flag1.bits.bit1
#define g_b_2s_jtim flag1.bits.bit2
#define g_b_485_send_tick flag1.bits.bit3
#define g_b_485_send_Error flag1.bits.bit4
#define g_b_485_receive_Error flag1.bits.bit5



#endif
