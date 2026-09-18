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
#include "mbport.h"

/* ----------------------- Variables ----------------------------------------*/
static eMBEventType eQueuedEvent;
static BOOL xEventInQueue;

/* ----------------------- Start implementation -----------------------------*/
// 事件队列初始化
BOOL xMBPortEventInit(void)
{
    xEventInQueue = FALSE;
    return TRUE;
}

// 事件队列发送事件
BOOL xMBPortEventPost(eMBEventType eEvent)
{
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
}
// 事件队列获取事件
BOOL xMBPortEventGet(eMBEventType *eEvent)
{
    BOOL xEventHappened = FALSE;

    if (xEventInQueue)
    {
        *eEvent = eQueuedEvent;
        xEventInQueue = FALSE;
        xEventHappened = TRUE;
    }
    return xEventHappened;
}
