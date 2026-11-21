#ifndef __SYS_MONITOR_H
#define __SYS_MONITOR_H

#include "FreeRTOS.h"
#include "task.h"

// 调试总开关：0 = 全关，1 = 开启
#define SYS_MONITOR_ENABLE    1

#if SYS_MONITOR_ENABLE

// ── 状态转字符串 ──
const char *task_state_str(eTaskState state);
const char *task_state_char_to_str(char state);

// ── LCD 轻量显示（用 eTaskGetState，不占大缓冲区）──
void sys_monitor_show_states(void);          // 显示各任务状态
void sys_monitor_show_stack(void);           // 显示栈高水位线
void sys_monitor_show_heap(void);            // 显示堆剩余

// ── 串口调试用（vTaskList 全量快照）──
void sys_monitor_dump_task_list(void);       // 打印所有任务信息
void sys_monitor_dump_all(void);             // 全量系统信息

// ── 运行时记录（在业务任务里周期性调用）──
void sys_monitor_record_stack(TaskHandle_t handle);  // 记录指定任务栈余量
void sys_monitor_record_heap(void);                   // 记录堆余量

// ── 外部句柄声明（如果 task1/2 句柄在别的文件定义）──
extern TaskHandle_t task1_handle;
extern TaskHandle_t task2_handle;

#endif

#endif
