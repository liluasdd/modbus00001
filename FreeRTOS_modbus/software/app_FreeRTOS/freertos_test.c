#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "key.h"
#include "freertos_test.h"
#include "queue.h"
#include "touch.h"

TaskHandle_t task1_handle;
TaskHandle_t task2_handle;
TaskHandle_t task3_handle;

QueueHandle_t queue;

char str_stack_high[15] = {0};
extern u8 s_tim_500ms;
// 在文件顶部定义一个全局变量，方便在 Watch 窗口监视
uint32_t debug_state = 0;
uint8_t debug_state2 = 0;
// 以1000ms频率闪烁LED1
void task1(void *pvParams)
{
    /* Modbus从站初始化 */
    //    eMBInit(MB_RTU, MB_SAMPLE_TEST_SLAVE_ADDR, MB_MASTER_USARTx, 9600, MB_PAR_NONE); // MB_PAR_NONE无校验 MB_PAR_ODD奇校验 MB_PAR_EVEN偶校验

    //    eMBEnable(); // /* 启用Modbus从站 */
    //    debug_printf("[task1] 运行中...\r\n");
    uint32_t stack_hightask1 = 0;
    uint32_t stack_high_min1 = 0xFFFFFFFF;
    debug_state2++;
    while (1)
    {
        if (debug_state2 < 255)
        {
            debug_state2++;
        }
        else
        debug_state2 = 1;

        // // 在 task2 的 while(1) 循环里加上：
        // if (task1_handle != NULL)
        // {
        //     debug_state = eTaskGetState(task1_handle);
        // }
        // /* 发送超时故障处理 */
        // eMBsend_Error();
        /* 轮询Modbus从站 */
        eMBPoll();

        // stack_hightask1 = uxTaskGetStackHighWaterMark(NULL);
        // if (stack_hightask1 < stack_high_min1)
        // {
        //     stack_high_min1 = stack_hightask1;
        // }
        // if (s_tim_500ms >= 5)
        // {
        //     s_tim_500ms = 0;
        //     sprintf(str_stack_high, "task1:%luB", stack_high_min1);
        //     // u16_to_4digits(str_stack_high, stack_high_min1);
        //     Show_Str(10, 80, (u8 *)str_stack_high, 16, 0);
        // }
        vTaskDelay(pdMS_TO_TICKS(9)); // 永远等于 500ms，不管 tick 频率怎么改
        // vTaskDelay(500);                // 500毫秒轮询一次Modbus从站，处理数据交换

        // vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1000));
    }
}

extern u8 led_num;
char task_info_buffer[30];
// 以500ms频率闪烁LED2
void task2(void *pvParams)
{
    BaseType_t ret = 0;
    uint8_t num = 0;
    uint32_t stack_hightask2 = 0;
    uint32_t stack_high_min = 0xFFFFFFFF;
    uint32_t heap_min_free = 0xFFFFFFFF; // 记录历史最小剩余（最危险值）
    LCD_Clear(BLACK);                    // 清屏
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
            TaskKey();
            // a = KEY_R_ADD_SUB(&R_key_anxiaOpen, R_SetPower.ints, KEY_ADD, KEY_SUB, MaxPower, MinPower, bujin, 0, R_Key.LongCT);
            // if (a != R_SetPower.ints)
            // {
            //     R_SetPower.ints = a;
            // }
            if (g_b_2s_jtim == 1)
            {
                xMBPortEventPost(EV_READY); // 塞入一个假事件
                g_b_2s_jtim = 0;
                // if (R_Task != D_Task2)
                //     return;
                Led_scan();
                ADC_DMA_scan();
                // vTaskList(task_info_buffer); // 获取所有任务信息
                // sprintf(str_stack_high, "task1:%luB", task_info_buffer);
                // Show_Str(10, 110, (u8 *)str_stack_high, 16, 0);
            }

            if (R_key_anxiaOpen == 1)
            {
                led_num = 0;
            }
            if (R_key_anxiaOpen == 2)
            {
                led_num = 1;
            }

            // // 记录历史任务运行最小剩余量,NULL当前任务
            // stack_hightask2 = uxTaskGetStackHighWaterMark(NULL);
            // if (stack_hightask2 < stack_high_min)
            // {
            //     stack_high_min = stack_hightask2;
            // }
            // // 记录历史系统运行最小剩余量
            // stack_hightask2 = xPortGetFreeHeapSize();
            // if (stack_hightask2 < heap_min_free)
            // {
            //     heap_min_free = stack_hightask2;
            // }
            // if (g_b_500ms_jtim == 1)
            // {
            //     // stack_high = uxTaskGetStackHighWaterMark(task2_handle);
            //     // sprintf(str_stack_high, "task2:%luB", stack_high_min);
                u16_to_4digits(str_stack_high, debug_state2);
                Show_Str(10, 30, (u8 *)str_stack_high, 16, 0);
            // }
            if (R_Task < 0xF0) // 任务循环// if (R_Task < 0xC0) // 任务循环
                R_Task++;
        }

        key_value1 = 0;
        vTaskDelay(pdMS_TO_TICKS(10)); // 释放 CPU 控制权
        // vTaskDelay(2);
    }
}

// 检测按键1
void task3(void *pvParams)
{
    // debug_printf("[task3] 运行中...\r\n");
    BaseType_t ret = 0;
    uint8_t key = 0;
    uint8_t num = 0;
    BaseType_t space;
    while (1)
    {
        //    key = key_scan();
        // if (key != g_key_num)
        // {
        //     key = g_key_num;
        // }

        if (key == 1) // 检测到KEY按下
        {
            ret = xQueueSend(queue, &key, 10); // 发送消息到队列
            if (ret == pdTRUE)
            {
                // debug_printf("消息队列queue发送成功，发送值为：%d\r\n", key);
                space = uxQueueSpacesAvailable(queue);
                // debug_printf("队列剩余容量为：%ld\r\n", space);
                if (space == 0)
                {
                    // debug_printf("清空队列！\r\n");
                    xQueueReset(queue);
                }
            }
            // else
            //     debug_printf("消息队列queue发送失败\r\n");
        }
        else if (key == 2)
        {
            ret = xQueueReceive(queue, &num, 10);
            // if (ret == pdPASS)
            //     debug_printf("消息队列queue读取成功，读取值为：%d, 剩余消息数为：%ld\r\n", num, uxQueueMessagesWaiting(queue));
            // else
            //     debug_printf("消息队列queue读取失败\r\n");
        }
        // vTaskDelay(10);
        vTaskDelay(500);
    }
}

void freertos_test(void)
{
    //    queue = xQueueCreate(3, sizeof(uint8_t)); // 创建消息队列queue
    // if (queue != NULL)
    //     debug_printf("消息队列queue创建成功！\r\n");
    // else
    //     debug_printf("消息队列queue创建失败！\r\n");

    xTaskCreate(task1,          // 任务函数指针
                "task1",        // 任务名称
                128 * 10,       // 任务堆栈大小
                NULL,           // 传入任务函数的参数
                3,              // 任务优先级
                &task1_handle); // 任务句柄

    xTaskCreate(task2,          // 任务函数指针
                "task2",        // 任务名称
                128 * 4,        // 任务堆栈大小
                NULL,           // 传入任务函数的参数
                2,              // 任务优先级
                &task2_handle); // 任务句柄

    // xTaskCreate(task3,          // 任务函数指针
    //             "task3",        // 任务名称
    //             128,            // 任务堆栈大小
    //             NULL,           // 传入任务函数的参数
    //             1,              // 任务优先级
    //             &task3_handle); // 任务句柄
}
