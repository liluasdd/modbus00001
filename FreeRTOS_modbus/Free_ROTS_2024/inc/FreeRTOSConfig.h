/*
 * FreeRTOS V202212.01
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/
/*新增*/
#define vPortSVCHandler SVC_Handler       /*SVC_Handler函数*/
#define xPortPendSVHandler PendSV_Handler /*PendSV_Handler函数*/
#define configUSE_QUEUE_SETS 1            /*队列设置*/
// 队列注册表大小（用于调试时给队列命名查看，不用调试可设为 0 省内存）
#define configQUEUE_REGISTRY_SIZE 10          /*队列注册表大小*/
#define INCLUDE_xTaskGetSchedulerState 1      /*获取任务调度器状态*/
#define INCLUDE_uxTaskGetStackHighWaterMark 1 /*获取任务栈水位*/
#define configRECORD_STACK_HIGH_ADDRESS 1     /*记录栈顶高地址（方便调试器可视化显示，可选）*/
#define INCLUDE_xTaskAbortDelay 0             /*中断延时强制任务执行*/
#define configUSE_MALLOC_FAILED_HOOK 0        /*malloc失败钩子*/
#define configCHECK_FOR_STACK_OVERFLOW 0      /*检查堆栈溢出*/
#define configUSE_IDLE_HOOK 0                 /*空闲钩子 */
#define configUSE_TICK_HOOK 0                 /*Tick钩子 */
#define configUSE_MUTEXES             1 /*互斥量设置*/
#define configUSE_RECURSIVE_MUTEXS    0 /*递归互斥量设置*/
#define configUSE_COUNTING_SEMAPHORES 0 /*计数信号量设置*/



#define configTICK_RATE_HZ 1000                        /* 定义系统时钟节拍频率, 单位: Hz, 无默认需定义 */
#define configUSE_PREEMPTION 1                         /*抢占式任务 */
#define configCPU_CLOCK_HZ ((unsigned long)72000000)   /*CPU时钟频率*/
#define configMAX_PRIORITIES (10)                      /*最大优先级*/
#define configMINIMAL_STACK_SIZE ((unsigned short)128) /*最小栈大小*/
#define configTOTAL_HEAP_SIZE ((size_t)(7 * 1024))     /*总堆大小*/
#define configMAX_TASK_NAME_LEN (16)                   /*任务名称最大长度*/
#define configUSE_TRACE_FACILITY 0                     /*跟踪设施 */
#define configUSE_16_BIT_TICKS 0                       /*16位Tick */
#define configIDLE_SHOULD_YIELD 1                      /*空闲任务是否应该让出CPU */

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet 1      /*任务优先级设置 */
#define INCLUDE_uxTaskPriorityGet 1     /*任务优先级获取 */
#define INCLUDE_vTaskDelete 1           /*任务删除 */
#define INCLUDE_vTaskCleanUpResources 0 /*任务清理资源 */
#define INCLUDE_vTaskSuspend 1          /*任务挂起 */
#define INCLUDE_vTaskDelayUntil 1       /*绝对时间周期延时 */
#define INCLUDE_vTaskDelay 1            /*任务中正常延时 */

/* This is the raw value as per the Cortex-M3 NVIC.  Values can be 255
(lowest) to 0 (1?) (highest). */
#define configKERNEL_INTERRUPT_PRIORITY 255 /*配置使用内核中断优先级*/
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
/* 配置使用最大系统调用中断优先级 */ /* equivalent to 0xb0, or priority 11. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 191

/* This is the value being used as per the ST library which permits 16
priority values, 0 to 15.  This must correspond to the
configKERNEL_INTERRUPT_PRIORITY setting.  Here 15 corresponds to the lowest
NVIC value of 255. */
#define configLIBRARY_KERNEL_INTERRUPT_PRIORITY 15 /*配置使用库内核中断优先级*/

#endif /* FREERTOS_CONFIG_H */
