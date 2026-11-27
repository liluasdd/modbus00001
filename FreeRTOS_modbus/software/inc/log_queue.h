#ifndef LOG_QUEUE_H
#define LOG_QUEUE_H

#include "FreeRTOS.h"
#include "queue.h"
#include <stdarg.h>
#include <stdint.h>

typedef enum { LOG_DEBUG = 0, LOG_INFO, LOG_WARN, LOG_ERROR } LogLevel_t;

typedef struct {
    LogLevel_t level;// 日志级别
    char msg[64];// 日志消息缓冲区
    uint32_t tick_count;// 记录发送时间戳
} LogMsg_t;

extern QueueHandle_t logQueue;
extern LogMsg_t logMsg;

void LogQueue_Init(void);
void Log_Send(LogLevel_t level, const char *format, ...);  // ← 改名！

#endif

