/*
 * FreeRTOS Kernel V11.1.0
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 * 
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 * See http://www.freertos.org/a00110.html
 */

/*-----------------------------------------------------------
 * FreeRTOS 配置文件，用于配置 FreeRTOS 内核的行为和性能，
 * 包括任务、队列、信号量、互斥量、递归互斥量、定时器等
 * 以及内存管理、任务通知、同步原语等
 * 文件名 FreeRTOSConfig.h
 * 文件格式 GB2312 编码
 *----------------------------------------------------------*/
#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H
extern void vAssertCalled(const char *file, int line);
/*==============================================================
 * 一、中断向量重映射（Cube/HAL 环境必须，让内核用 HAL 的 Handler 名）
 *==============================================================*/
#define vPortSVCHandler SVC_Handler       /*SVC_Handler函数*/
#define xPortPendSVHandler PendSV_Handler /*PendSV_Handler函数*/

/*==============================================================
 * 二、调度器核心
 *==============================================================*/
/*======  调度器 ======*/
#define configUSE_PREEMPTION 1                         /*抢占式任务 */
#define configUSE_TIME_SLICING 1                       /*时间分片 */
#define configIDLE_SHOULD_YIELD 1                      /*空闲任务是否应该让出CPU */
#define configCPU_CLOCK_HZ ((unsigned long)72000000)   /*CPU时钟频率*/
#define configTICK_RATE_HZ 1000                        /* 定义系统时钟节拍频率, 单位: Hz, 无默认需定义 */
#define configMAX_PRIORITIES (10)                      /*最大优先级*/
#define configMINIMAL_STACK_SIZE ((unsigned short)128) /*最小栈大小*/
#define configMAX_TASK_NAME_LEN (16)                   /*任务名称最大长度*/
#define configUSE_16_BIT_TICKS 0                       /*16位Tick */

/*==============================================================
 * 三、内存管理
 *==============================================================*/
#define configSUPPORT_DYNAMIC_ALLOCATION 1          /* 动态内存分配 */
#define configTOTAL_HEAP_SIZE ((size_t)(12 * 1024)) /*总堆大小*/
#define configUSE_TASK_NOTIFICATIONS 1              /* 任务通知 */

/*==============================================================
 * 四、同步原语
 *==============================================================*/
#define configUSE_MUTEXES 1             /* 互斥量设置 */
#define configUSE_RECURSIVE_MUTEXES 0    /* 没嵌套加锁就关 *//*递归互斥量设置*/
#define configUSE_COUNTING_SEMAPHORES 1 /* Modbus中断→任务通知建议开 */
#define configUSE_QUEUE_SETS 1          /*队列设置*/
#define configQUEUE_REGISTRY_SIZE 10    /*队列注册表大小*/
#define configUSE_EVENT_GROUPS 1        /* 事件组设置 */

/*==============================================================
 * 五、软件定时器（FreeModbus 用硬件TIM，可关）
 *==============================================================*/
#define configUSE_TIMERS 0                                          /* 软件定时器：FreeModbus 自己用硬件TIM，可关 */
#define configTIMER_TASK_PRIORITY (configMAX_PRIORITIES - 2)        /* 定时器任务优先级 */
#define configTIMER_QUEUE_LENGTH 10                                 /* 定时器队列长度 */
#define configTIMER_TASK_STACK_DEPTH (configMINIMAL_STACK_SIZE * 2) /* 定时器任务栈深度 */

/*==============================================================
 * 六、中断优先级（Cortex-M / STM32F103）
 *==============================================================*/
#define configPRIO_BITS 4/*中断优先级位数*/
/* 逻辑值 */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY 15/*最低中断优先级*/
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5/*最大系统调用中断优先级*/
/* 硬件寄存器值（自动算，别手填） */
#define configKERNEL_INTERRUPT_PRIORITY \
    (configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))/*内核中断优先级*/
#define configMAX_SYSCALL_INTERRUPT_PRIORITY \
    (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))/*最大系统调用中断优先级*/
/* This is the value being used as per the ST library which permits 16
priority values, 0 to 15.  This must correspond to the
configKERNEL_INTERRUPT_PRIORITY setting.  Here 15 corresponds to the lowest
NVIC value of 255. */
#define configLIBRARY_KERNEL_INTERRUPT_PRIORITY 15 /*配置使用库内核中断优先级*/

/*==============================================================
 * 七、任务 API 开关（INCLUDE_ 系列）
 *==============================================================*/
#define INCLUDE_vTaskPrioritySet 1            /*任务优先级设置 */
#define INCLUDE_uxTaskPriorityGet 1           /*任务优先级获取 */
#define INCLUDE_vTaskDelete 1                 /*任务删除 */
#define INCLUDE_vTaskCleanUpResources 0       /*任务清理资源 */
#define INCLUDE_vTaskSuspend 1                /*任务挂起 */
#define INCLUDE_vTaskDelayUntil 1             /*绝对时间周期延时 */
#define INCLUDE_vTaskDelay 1                  /*任务中正常延时 */
#define INCLUDE_xTaskGetSchedulerState 1      /*获取任务器（调度器状态） */
#define INCLUDE_uxTaskGetStackHighWaterMark 1 /*获取任务栈水位 */
#define INCLUDE_xTaskAbortDelay 0             /*任务中止延时 */
#define INCLUDE_xTaskGetCurrentTaskHandle 1   /*获取当前任务句柄 */
#define INCLUDE_xTaskGetIdleTaskHandle    0/*IDLE 任务 包含 xTaskGetIdleTaskHandle 函数*/
#define INCLUDE_eTaskGetState 1    /* 获取任务状态 */

/*==============================================================
 * 八、钩子函数
 *==============================================================*/
#define configUSE_IDLE_HOOK 0     /*空闲钩子 */
#define configUSE_TICK_HOOK 0     /*Tick钩子 */
#define configUSE_TICKLESS_IDLE 0 /*无Tick空闲模式,休眠模式*/
#define configEXPECTED_IDLE_TIME_BEFORE_SLEEP 5 /*预期空闲时间，单位：ms*/

/*==============================================================
 * 九、调试与诊断（★ ====== 调试使用（按需开关） ======）
 *==============================================================*/
#define configASSERT(x)                        \
    do                                         \
    {                                          \
        if ((x) == 0)                          \
        {                                      \
            vAssertCalled(__FILE__, __LINE__); \
        }                                      \
    } while (0)

#define configCHECK_FOR_STACK_OVERFLOW 2  /* 方法2，能抓栈溢出 *//*检查堆栈溢出*/
#define configUSE_MALLOC_FAILED_HOOK 1    /* 堆分配失败回调 *//*malloc失败钩子*/
#define configRECORD_STACK_HIGH_ADDRESS 1 /* 调试器可视化栈用 *//*记录栈顶高地址（方便调试器可视化显示，可选）*/
#define configUSE_TRACE_FACILITY 1        /* uxTaskGetSystemState 等依赖 */ /*跟踪设施 */
#define configGENERATE_RUN_TIME_STATS 0   /* 需额外定时器，先关 */
#define configSTATS_ARRAY_SIZE 0 /*统计数组大小*/
#define configUSE_STATS_FORMATTING_FUNCTIONS 1 /*使用统计格式化函数*/

/*==============================================================
 * 十、杂项
 *==============================================================*/
#define configUSE_CO_ROUTINES 0 /* 协程已淘汰 */
#define configUSE_NEWLIB_REENTRANT 0/*是否使用新库重入模式*/
#define configENABLE_BACKWARD_COMPATIBILITY 0/*是否启用后兼容模式*/

// /* This is the raw value as per the Cortex-M3 NVIC.  Values can be 255
// (lowest) to 0 (1?) (highest). */
// #define configKERNEL_INTERRUPT_PRIORITY 255 /*配置使用内核中断优先级*/
// /* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
// See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
// /* 配置使用最大系统调用中断优先级 */ /* equivalent to 0xb0, or priority 11. */
// #define configMAX_SYSCALL_INTERRUPT_PRIORITY 191


#endif /* FREERTOS_CONFIG_H */
