#include "sys_monitor.h"
#include <stdio.h>
#include <string.h>
#include "usart.h" // 你的串口

#if SYS_MONITOR_ENABLE

// ── 全局记录变量 ──
static uint32_t stack_high_min_task1 = 0xFFFFFFFF;
static uint32_t stack_high_min_task2 = 0xFFFFFFFF;
static size_t heap_min_free = 0xFFFFFFFF;

// ── 状态枚举转字符串 ──
const char *task_state_str(eTaskState state)
{
    switch (state)
    {
    case eRunning:
        return "Run";//0 正在运行
    case eReady:
        return "Rdy";//1 就绪
    case eBlocked:
        return "Blk";//2 阻塞
    case eSuspended:
        return "Sus";//3 挂起
    case eDeleted:
        return "Del";//4 已删除
    default:
        return "???";//未知状态
    }
}

// // ── vTaskList 状态字母转字符串 ──
// const char *task_state_char_to_str(char state)
// {
//     switch (state)
//     {
//     case 'X':
//         return "Run";
//     case 'R':
//         return "Rdy";
//     case 'B':
//         return "Blk";
//     case 'S':
//         return "Sus";
//     case 'D':
//         return "Del";
//     default:
//         return "???";
//     }
// }

// ── LCD 显示：任务状态（轻量，用 eTaskGetState）──
void sys_monitor_show_states(void)
{
    char buf[32];
    eTaskState s;
    //自己查自己永远是 eRunning  0 正在运行 1 就绪 2 阻塞 3 挂起 4 已删除
    // task1
    s = eTaskGetState(task1_handle);
    snprintf(buf, sizeof(buf), "T1:%s", task_state_str(s));
    Show_Str(10, 80, (u8 *)buf, 16, 0);

    // task2
    s = eTaskGetState(task2_handle);
    snprintf(buf, sizeof(buf), "T2:%s", task_state_str(s));
    Show_Str(10, 96, (u8 *)buf, 16, 0);

    // // IDLE
    // s = eTaskGetState(xTaskGetIdleTaskHandle());
    // snprintf(buf, sizeof(buf), "IDL:%s", task_state_str(s));
    // Show_Str(10, 112, (u8 *)buf, 16, 0);
}

// ── LCD 显示：栈高水位线 ──
void sys_monitor_show_stack(void)
{
    char buf[32];

    snprintf(buf, sizeof(buf), "S1:%luB S2:%luB",
             stack_high_min_task1 * 4,
             stack_high_min_task2 * 4);
    Show_Str(10, 128, (u8 *)buf, 16, 0);
}

// ── LCD 显示：堆剩余 ──
void sys_monitor_show_heap(void)
{
    char buf[32];
    size_t free;

    free = xPortGetFreeHeapSize();
    snprintf(buf, sizeof(buf), "Heap:%d/%d", free, configTOTAL_HEAP_SIZE);
    Show_Str(10, 140, (u8 *)buf, 16, 0);
}

#if (INCLUDE_uxTaskGetStackHighWaterMark == 1)
// ── 记录指定任务栈余量 ──
void sys_monitor_record_stack(TaskHandle_t handle)
{
    uint32_t water = uxTaskGetStackHighWaterMark(handle);

    if (handle == task1_handle)
    {
        if (water < stack_high_min_task1)
            stack_high_min_task1 = water;
    }
    else if (handle == task2_handle)
    {
        if (water < stack_high_min_task2)
            stack_high_min_task2 = water;
    }
}
#endif

// ── 记录堆余量 ──
void sys_monitor_record_heap(void)
{
    size_t free = xPortGetFreeHeapSize();
    if (free < heap_min_free)
        heap_min_free = free;
}

// ── 串口打印：任务列表全量快照 ──
// #if (configUSE_TRACE_FACILITY == 1)
#if (configUSE_TRACE_FACILITY == 1 && configUSE_STATS_FORMATTING_FUNCTIONS == 1)
void sys_monitor_dump_task_list(void)
{
    static char task_list_buf[512];
    vTaskList(task_list_buf);
    printf("==== Task List ====\r\n%s\r\n", task_list_buf);
}
#endif

// ── 串口打印：全量系统信息 ──
void sys_monitor_dump_all(void)
{
    printf("==== System Monitor ====\r\n");
    printf("Task1 stack min: %lu words (%lu bytes)\r\n",
           stack_high_min_task1, stack_high_min_task1 * 4);
    printf("Task2 stack min: %lu words (%lu bytes)\r\n",
           stack_high_min_task2, stack_high_min_task2 * 4);
    printf("Heap min free: %d bytes\r\n", heap_min_free);
    printf("========================\r\n");
// #if (configUSE_TRACE_FACILITY == 1)
#if (configUSE_TRACE_FACILITY == 1 && configUSE_STATS_FORMATTING_FUNCTIONS == 1)
    sys_monitor_dump_task_list();
#endif
}

/*查询任务状态 */
// #define configUSE_TRACE_FACILITY    1   // ← 必须开，vTaskList 依赖它
// #define configUSE_STATS_FORMATTING_FUNCTIONS    1   // ← vTaskList/vTaskGetRunTimeStats 格式化输出需要
// vTaskList(task_info_buffer); // 获取所有任务信息
// 例如：task2  X   2   442   2
// 任务名：task2
// 任务状态：X 正在运行 X eRunning 正在运行 R Ready 就绪  B Blocked 阻塞  S Suspended 挂起  D Deleted 已删除
// 任务优先级：2级  0-255，数字越小优先级越高，0是最高优先级，255是最低优先级
// 栈剩余：442 字 × 4 = 1768 字节
// 编号：2
// 方式1：串口打印
// printf("Task List:\r\n%s\r\n", task_info_buffer);
// // 方式2：LCD 显示（需要滚动或分页，因为内容可能很长）
// Show_Str(10, 110, (u8 *)task_info_buffer, 16, 0);
// 方式3：调试器里直接看 task_info_buffer 字符串内容
// 查询任务状态
// eTaskState state = eTaskGetState(task1_handle);  // 0 正在运行 1 就绪 2 阻塞 3 挂起 4 已删除
// /* u16_to_4digits(str_stack_high, state); */
// Show_Str(10, 50, (u8 *)str_stack_high, 16, 0);
// // 记录历史任务运行最小剩余量,NULL当前任务  栈余量  //INCLUDE_uxTaskGetStackHighWaterMark  要开
// stack_hightask = uxTaskGetStackHighWaterMark(task1_handle);
// if (stack_hightask < stack_high_min)
// {
//     stack_high_min = stack_hightask;
// }
// // 记录历史系统运行最小剩余量  堆余量
// stack_hightask = xPortGetFreeHeapSize();
// if (stack_hightask < stack_high_min)
// {
//     stack_high_min = stack_hightask;
// }
// // sprintf(str_stack_high, "task2:%luB", stack_high_min);
// // Show_Str(10, 50, (u8 *)str_stack_high, 16, 0);

#endif
