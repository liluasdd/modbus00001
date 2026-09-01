#ifndef        __USER_TYPE_H
#define        __USER_TYPE_H

#include "stdint.h"
 
                              
#define      s64            int64_t
#define      s32            int32_t
#define     s16            int16_t
#define         s8            int8_t  

#define     sc64        const int64_t   /*!< Read Only */
#define     sc32        const int32_t   /*!< Read Only */
#define     sc16        const int16_t   /*!< Read Only */
#define     sc8            const int8_t    /*!< Read Only */

#define     u64            uint64_t  
#define     u32            uint32_t  
#define     u16            uint16_t 
#define     u8            uint8_t  

#define     uc64        const uint64_t   /*!< Read Only */
#define     uc32        const uint32_t   /*!< Read Only */
#define     uc16        const uint16_t   /*!< Read Only */
#define     uc8            const uint8_t    /*!< Read Only */

#define     vu64        volatile    u64
#define     vu32        volatile    u32
#define     vu16        volatile    u16
#define     vu8         volatile    u8

#endif
