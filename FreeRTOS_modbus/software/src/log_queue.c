#include "log_queue.h"
#include <stdio.h>
#include <stdarg.h>
#include "task.h"

QueueHandle_t logQueue = NULL;

void LogQueue_Init(void)
{
    logQueue = xQueueCreate(10, sizeof(LogMsg_t));
    if (logQueue == NULL)
    {
        while (1)
            ;
    }
}

// 函数名改为 Log_Send
void Log_Send(LogLevel_t level, const char *format, ...)
{
    LogMsg_t msg;
    va_list args;

    if (logQueue == NULL)
        return;

    msg.level = level;
#if DEBUG_UART_TaskGetTick_ENABLE
    msg.tick_count = xTaskGetTickCount();
#endif

    va_start(args, format);
    vsnprintf(msg.msg, sizeof(msg.msg), format, args);
    va_end(args);

    xQueueSend(logQueue, &msg, 0);
}

/* 中断级发送（简化版，不使用格式化，避免 vsnprintf 在 ISR 中的问题） */
void Log_SendFromISR(LogLevel_t level, const char *format, ...)
{
    LogMsg_t msg;
    va_list args;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (logQueue == NULL)
        return;

    msg.level = level;
    msg.tick_count = xTaskGetTickCountFromISR();

    va_start(args, format);
    vsnprintf(msg.msg, sizeof(msg.msg), format, args);
    va_end(args);

    xQueueSendFromISR(logQueue, &msg, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
