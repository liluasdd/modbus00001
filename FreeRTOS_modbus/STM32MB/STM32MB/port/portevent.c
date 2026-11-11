/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "gpio.h"
#include "mbport.h"
/* ----------------------- FreeRTOS includes --------------------------------*/
#include "FreeRTOS.h"
#include "queue.h"

/* ----------------------- Variables ----------------------------------------*/
#if freertos_modbus
// 使用 FreeRTOS 队列替代原来的单变量
static QueueHandle_t xEventQueue;
#else
static eMBEventType eQueuedEvent;
static BOOL xEventInQueue;
#endif
/* ----------------------- Start implementation -----------------------------*/

// static QueueHandle_t xEventQueue = NULL;

// BOOL xMBPortEventInit(void)
// {
//     xEventQueue = xQueueCreate(10, sizeof(eMBEventType));
//     return (xEventQueue != NULL) ? TRUE : FALSE;
// }

// BOOL xMBPortEventPost(eMBEventType eEvent)
// {
//     BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//     if (xPortIsInsideInterrupt()) 
//     {
//         xQueueSendFromISR(xEventQueue, &eEvent, &xHigherPriorityTaskWoken);
//         portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
//     } 
//     else 
//     {
//         xQueueSend(xEventQueue, &eEvent, 0);
//     }
//     return TRUE;
// }

// BOOL xMBPortEventGet(eMBEventType *eEvent)
// {
//     /* 核心：使用 portMAX_DELAY 阻塞等待！ */
//     if (xQueueReceive(xEventQueue, eEvent, portMAX_DELAY) == pdPASS) 
//     {
//         return TRUE;
//     }
//     return FALSE;
// }

// 事件队列初始化
BOOL xMBPortEventInit(void)
{
#if freertos_modbus
    // 创建深度为 4 的事件队列，大小为 eMBEventType 的大小
    xEventQueue = xQueueCreate(4, sizeof(eMBEventType));
    if (xEventQueue != NULL)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
#else
    xEventInQueue = FALSE;
    return TRUE;
#endif
}

// 事件队列发送事件
BOOL xMBPortEventPost(eMBEventType eEvent)
{
#if freertos_modbus
    // 判断是否在中断上下文中
    if (xPortIsInsideInterrupt())
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        // 在 ISR 中使用 FromISR 版本的 API
        xQueueSendFromISR(xEventQueue, &eEvent, &xHigherPriorityTaskWoken);

        // 如果发送事件唤醒了更高优先级的任务，退出中断时需要进行任务切换
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    else
    {
        // 在任务上下文中，使用普通版 API，不阻塞等待（超时为0）
        xQueueSend(xEventQueue, &eEvent, 0);
    }

    return TRUE;
#else
    xEventInQueue = TRUE;
    eQueuedEvent = eEvent;
    // 事件队列发送事件
    if (eEvent == EV_EXECUTE) // 执行事件
    {
        // 通知任务执行事件
        // xTaskNotify(xMBTaskHandle, (uint32_t)eEvent, eSetValueWithOverwrite);
    }
    else if (eEvent == EV_FRAME_RECEIVED) // 帧接收事件
    {
        // BaseType_t xHigherPriorityTaskWoken;
        // BaseType_t sendStatus;
        // sendStatus = xTaskNotifyFromISR(xMBTaskHandle, (uint32_t)eEvent, &xHigherPriorityTaskWoken);
        // portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    return TRUE;
#endif
}
// 事件队列获取事件
BOOL xMBPortEventGet(eMBEventType *eEvent)
{
#if freertos_modbus
    // 阻塞等待事件，如果队列为空，当前任务会被挂起（让出 CPU）
    // portMAX_DELAY 表示无限等待，直到有事件到来
    if (xQueueReceive(xEventQueue, eEvent, portMAX_DELAY) == pdTRUE)
    {
        return TRUE;
    }

    return FALSE;
#else
    BOOL xEventHappened = FALSE;

    if (xEventInQueue)
    {
        *eEvent = eQueuedEvent;
        xEventInQueue = FALSE;
        xEventHappened = TRUE;
    }
    return xEventHappened;
#endif
}
