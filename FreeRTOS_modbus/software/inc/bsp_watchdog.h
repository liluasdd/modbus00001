
#if 1 // 全局宏开关

#ifndef __BSP_WATCHDOG_H
#define __BSP_WATCHDOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

extern void feed_watchdog(void);
extern void watchdog_on(void);
extern void watchdog_off(void);
extern void watchdog_reset(void);
extern void bsp_iwdgconfig(void);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_WATCHDOG_H */

#endif
