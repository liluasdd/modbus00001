#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "key.h"
#include "freertos_test.h"
#include "log_queue.h"
// #include "queue.h"
#include <string.h> // ← 提供 strlen
#include "touch.h"

extern UART_HandleTypeDef huart1;

TaskHandle_t task1_handle;
TaskHandle_t task2_handle;
TaskHandle_t task3_handle;

QueueHandle_t queue;

char str_stack_high[10] = {0};
extern u8 s_tim_500ms;
// 在文件顶部定义一个全局变量，方便在 Watch 窗口监视
uint32_t debug_state = 0;
uint8_t debug_state2 = 0;
uint32_t stack_hightask = 0;
uint32_t stack_high_min = 0xFFFFFFFF;
// 以1000ms频率闪烁LED1
void task1(void *pvParams)
{
    /* Modbus从站初始化 */
    //    eMBInit(MB_RTU, MB_SAMPLE_TEST_SLAVE_ADDR, MB_MASTER_USARTx, 9600, MB_PAR_NONE); // MB_PAR_NONE无校验 MB_PAR_ODD奇校验 MB_PAR_EVEN偶校验

    //    eMBEnable(); // /* 启用Modbus从站 */
    //    debug_printf("[task1] 运行中...\r\n");
    // TickType_t xLastWakeTime = xTaskGetTickCount(); // 获取当前时间点
    debug_state2 = 1;

    // uint8_t debug_loop_count = 0;

    while (1)
    {
        if (debug_state2 < 255)
            debug_state2++;
        else
            debug_state2 = 0;

        // /* 发送超时故障处理 */
        // eMBsend_Error();
        /* 轮询Modbus从站 */
        eMBPoll();
        // stack_high_min = uxTaskGetStackHighWaterMark(NULL);// 获取任务栈高水位线，单位为字节

        vTaskDelay(pdMS_TO_TICKS(10)); // 10ms，轮询一次Modbus从站，处理数据交换
        // // /*周期延时：保证从 xLastWakeTime 开始，每隔 1000ms 唤醒一次*/
        // vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1000));
    }
}

extern u8 led_num;
char task_info_buffer[30];
// 以500ms频率闪烁LED2
void task2(void *pvParams)
{
    // BaseType_t ret = 0;
    // uint8_t num = 0;
    LCD_Clear(BLACK); // 清屏
    ADC_DMA_scan();
    LogQueue_Init();
    while (1)
    {
        //    ret = xQueueReceive(queue, &num, portMAX_DELAY);
        //    if(ret == pdPASS)
        //        debug_printf("消息队列queue读取成功，读取值为：%d\r\n", num);
        //    else
        //        debug_printf("消息队列queue读取失败\r\n");

        if (g_b_task_jtim == 1)
        {
            g_b_task_jtim = 0;
            // if (R_Task == 0)
            {
                TaskKey();
            }
            feed_watchdog();

            // a = KEY_R_ADD_SUB(&R_key_anxiaOpen, R_SetPower.ints, KEY_ADD, KEY_SUB, MaxPower, MinPower, bujin, 0, R_Key.LongCT);
            // if (a != R_SetPower.ints)
            // {
            //     R_SetPower.ints = a;
            // }
            if (g_b_2s_jtim == 1)
            {
                // xMBPortEventPost(EV_READY); // 塞入一个假事件
                g_b_2s_jtim = 0;
                // if (R_Task == 1)
                {
                    if (R_DisDuty == 0xFF) // 更新显示任务
                    {
                        R_DisDuty = 0;
                    }

                    Led_scan();
                    ADC_DMA_scan();
                    // Log_Send(LOG_INFO, "BAT:%d", g_bat_v);// 发送给任务三，打印电池电压
                    // Log_Printf("g_bat_v:%d\r\n", g_bat_v);// 打印电池电压
                }
                // if (R_Task == 2)
                {
                    u16_to_4digits(str_stack_high, debug_state2, 0);
                    Show_Str(10, 50, (u8 *)str_stack_high, 16, 0);
                }
                if (R_DisDuty < 0XF0)
                {
                    R_DisDuty++;
                }
            }

            if (R_key_anxiaOpen == 1)
            {
                led_num = 0;
            }
            if (R_key_anxiaOpen == 2)
            {
                led_num = 1;
            }

            if (R_Task < 0xF0) // 任务循环
                R_Task++;
        }

        key_value1 = 0;
        vTaskDelay(pdMS_TO_TICKS(10));
        // vTaskDelay(2);
    }
}

#if SYS_MONITOR_ENABLE

void task3(void *pvParams)
{
#if DEBUG_UART_ENABLE
    TaskStatus_t info;
    TickType_t lastWakeTime = xTaskGetTickCount(); /* 记录起点 */
    const TickType_t period = pdMS_TO_TICKS(500);  /* 周期500ms */

#endif
    LogMsg_t msg;
    const char *level_str[] = {"DEBUG", "INFO", "WARN", "ERROR"};

    while (1)
    {
#if DEBUG_UART_ENABLE
        vTaskDelay(period); /* 500ms，查一下Modbus任务的状态和栈水位 */

        /* 查 Modbus 任务的状态和栈水位 */
        vTaskGetInfo(task1_handle, &info, pdTRUE, eInvalid);

        Log_Printf("[DBG] Modbus: state=%s prio=%lu words\r\n",
                   task_state_str(info.eCurrentState),
                   (unsigned long)info.uxCurrentPriority);

        /* 查系统剩余堆，仪表跑久了最怕堆泄漏 */
        Log_Printf("[DBG] free_heap=%u bytes\r\n",
                   (unsigned)xPortGetFreeHeapSize());
#endif

#if DEBUG_UART_LOG_ENABLE
        if (xQueueReceive(logQueue, &msg, portMAX_DELAY) == pdPASS) // 从队列中获取日志消息
        {
#if DEBUG_UART_TaskGetTick_ENABLE // 打印时间戳
            Log_Printf("[%lu][%s] %s\r\n",
                       msg.tick_count, level_str[msg.level], msg.msg);
#else
            Log_Printf("[%s] %s\r\n", level_str[msg.level], msg.msg);
#endif

            //     snprintf(output, sizeof(output), "[%lu][%s] %s\r\n",
            //              msg.tick_count, level_str[msg.level], msg.msg);
            //     HAL_UART_Transmit(&huart1, (uint8_t *)output, strlen(output), HAL_MAX_DELAY);// 发送日志到串口1
        }
        vTaskDelay(pdMS_TO_TICKS(6));
#endif

        // vTaskDelayUntil(&lastWakeTime, period);
    }
}
#endif

void freertos_test(void)
{
    // queue = xQueueCreate(1, sizeof(ADC_Msg_t)); /* 长度必须是 1 */
    // xQueueOverwrite(queue, &msg);               /* 覆盖旧值，永不失败 */
    // queue = xQueueCreate(3, sizeof(uint8_t));   // 创建消息队列queue
    // if (queue != NULL)
    //     debug_printf("消息队列queue创建成功！\r\n");
    // else
    //     debug_printf("消息队列queue创建失败！\r\n");

    xTaskCreate(task1,          // 任务函数指针
                "task1",        // 任务名称
                128 * 6,        // 任务堆栈大小
                NULL,           // 传入任务函数的参数
                4,              // 任务优先级
                &task1_handle); // 任务句柄

    xTaskCreate(task2,          // 任务函数指针
                "task2",        // 任务名称
                128 * 4,        // 任务堆栈大小
                NULL,           // 传入任务函数的参数
                3,              // 任务优先级
                &task2_handle); // 任务句柄

    // xTaskCreate(task3,          // 任务函数指针
    //             "task3",        // 任务名称
    //             128 * 4,        // 任务堆栈大小
    //             NULL,           // 传入任务函数的参数
    //             2,              // 任务优先级
    //             &task3_handle); // 任务句柄
}
