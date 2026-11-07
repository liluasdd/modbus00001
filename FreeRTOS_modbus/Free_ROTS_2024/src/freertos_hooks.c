#include "FreeRTOS.h"
#include "task.h"

#if configUSE_MALLOC_FAILED_HOOK
void vApplicationMallocFailedHook(void)
{
    /* pvPortMalloc 分配失败时进入这里 */
    taskDISABLE_INTERRUPTS();
    for (;;)
        ;
}
#endif // 新增

#if configCHECK_FOR_STACK_OVERFLOW
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    /* 任务栈溢出时进入这里 */
    (void)xTask;
    (void)pcTaskName;
    taskDISABLE_INTERRUPTS();
    for (;;)
        ;
}
#endif // 新增
