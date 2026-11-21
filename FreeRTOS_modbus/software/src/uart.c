#include "sys.h"
#include "uart.h"
#include "string.h"
#include "main.h"
#include "FreeRTOS.h"

extern UART_HandleTypeDef huart1; /* UART1句柄 */
extern UART_HandleTypeDef huart2; /* UART2句柄 */
extern UART_HandleTypeDef huart3; /* UART3句柄 */

/* 声明局部全局变量 */
uint8_t uart1_rx_buf[256];
uint16_t uart1_rx_len = 0;

static uint8_t log_buf[UART1_RX_BUF_SIZE];
static volatile uint16_t log_head = 0; // 写指针
static volatile uint16_t log_tail = 0; // 读指针

/**
 * @brief       重定义fputc函数
 * @note        printf函数最终会通过调用fputc输出字符串到 对应 串口
 */
int fputc(int ch, FILE *f)
{
    // uint16_t fputc_num = 0;
#if 1
    uint16_t next_head;
    uint16_t current_tail;

    // 极短的临界区：禁止调度和中断，防止读 tail 时被中断改了
    taskENTER_CRITICAL();
    next_head = (log_head + 1) % UART1_RX_BUF_SIZE;
    current_tail = log_tail;

    if (next_head == current_tail)
    {
        taskEXIT_CRITICAL();
        return ch; // 缓冲区满，丢弃
    }

    log_buf[log_head] = (uint8_t)ch;

    log_head = next_head;
    taskEXIT_CRITICAL();

    USART1->CR1 |= USART_CR1_TXEIE;
    return ch;

    // USART1->DR = (uint8_t)ch; /* 将要发送的字符 ch 写入到DR寄存器 */
#else

    while ((USART2->SR & 0X40) == 0) /* 等待上一个字符发送完成 */
    {
        fputc_num++;
        if (fputc_num > 65530)
        {
            return ch;
        }
    }

    USART2->DR = (uint8_t)ch; /* 将要发送的字符 ch 写入到DR寄存器 */
    ///* 串口3发送*/
    while ((USART3->SR & 0X40) == 0) /* 等待上一个字符发送完成 */
    {
        fputc_num++;
        if (fputc_num > 65530)
        {
            return ch;
        }
    }

    USART3->DR = (uint8_t)ch; /* 将要发送的字符 ch 写入到DR寄存器 */
		return ch;
#endif


}

/**
 * @brief       串口1初始化函数
 * @param       baudrate: 波特率, 根据自己需要设置波特率值
 * @retval      无
 */
void uart_init(uint32_t baudrate)
{
    /*UART1 初始化设置*/
    huart1.Instance = USART1;                    /* USART1 */
    huart1.Init.BaudRate = baudrate;             /* 波特率 */
    huart1.Init.WordLength = UART_WORDLENGTH_8B; /* 字长为8位数据格式 */
    huart1.Init.StopBits = UART_STOPBITS_1;      /* 一个停止位 */
    huart1.Init.Parity = UART_PARITY_NONE;       /* 无奇偶校验位 */
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE; /* 无硬件流控 */
    huart1.Init.Mode = UART_MODE_TX_RX;          /* 收发模式 */
    HAL_Delay(1);                                // 1ms稳定延时，专门解决115200首字符乱码

    HAL_UART_Init(&huart1);                      /* HAL_UART_Init()会使能UART1 */

    if (HAL_UART_Init(&huart1) != HAL_OK)/* 初始化失败 */
    {
        while (1);
    }
    HAL_NVIC_SetPriority(USART1_IRQn, 7, 0); // 同样要 >= 6
    HAL_NVIC_EnableIRQ(USART1_IRQn);

    //     /*UART2 初始化设置*/
    //     huart2.Instance = USART2;                    /* USART2 */
    //     huart2.Init.BaudRate = baudrate;             /* 波特率 */
    //     huart2.Init.WordLength = UART_WORDLENGTH_8B; /* 字长为8位数据格式 */
    //     huart2.Init.StopBits = UART_STOPBITS_1;      /* 一个停止位 */
    //     huart2.Init.Parity = UART_PARITY_NONE;       /* 无奇偶校验位 */
    //     huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE; /* 无硬件流控 */
    //     huart2.Init.Mode = UART_MODE_TX_RX;          /* 收发模式 */
    //     HAL_UART_Init(&huart2);                      /* HAL_UART_Init()会使能UART2 */

    //     /*UART3 初始化设置*/
    //     uart3_handle.Instance = USART3;                    /* USART3 */
    //     uart3_handle.Init.BaudRate = baudrate;             /* 波特率 */
    //     uart3_handle.Init.WordLength = UART_WORDLENGTH_8B; /* 字长为8位数据格式 */
    //     uart3_handle.Init.StopBits = UART_STOPBITS_1;      /* 一个停止位 */
    //     uart3_handle.Init.Parity = UART_PARITY_NONE;       /* 无奇偶校验位 */
    //     uart3_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE; /* 无硬件流控 */
    //     uart3_handle.Init.Mode = UART_MODE_TX_RX;          /* 收发模式 */
    //     HAL_UART_Init(&uart3_handle);                      /* HAL_UART_Init()会使能UART3 */
}

SemaphoreHandle_t xLogMutex = NULL;

void Log_Init(void)
{
    xLogMutex = xSemaphoreCreateMutex(); // 创建互斥锁
}

/**
 * @brief       UART1接收缓冲区清除
 * @param       无
 * @retval      无
 */
void uart1_rx_clear(void)
{
    memset(uart1_rx_buf, 0, sizeof(uart1_rx_buf)); /* 清空接收缓冲区 */
    uart1_rx_len = 0;                              /* 接收计数器清零 */
}

void Log_Printf(const char *format, ...)
{
    va_list args;

    // 1. 拿锁（阻塞等待，保证一条日志完整打印完，不被打断）
    if (xSemaphoreTake(xLogMutex, portMAX_DELAY) == pdPASS)
    {
        va_start(args, format);
        vprintf(format, args); // 会连续调用多次 fputc
        va_end(args);

        // 2. 放锁
        xSemaphoreGive(xLogMutex);
    }
}

/**
 * @brief       串口1中断服务函数
 * @note        在此使用接收中断及空闲中断，实现不定长数据收发
 * @param       无
 * @retval      无
 */
void USART1_IRQHandler(void)
{
    uint8_t receive_data = 0;

    /* 1. 先处理 ORE 溢出 */
    if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_ORE) != RESET)
    {
        __HAL_UART_CLEAR_OREFLAG(&huart1);
    }

    /* 2. TXE 发送处理 */
    if ((USART1->CR1 & USART_CR1_TXEIE) && (USART1->SR & USART_SR_TXE))
    {
        if (log_head != log_tail)
        {
            USART1->DR = log_buf[log_tail];
            log_tail = (log_tail + 1) % UART1_RX_BUF_SIZE; // 修正宏
        }
        else
        {
            USART1->CR1 &= ~USART_CR1_TXEIE;
        }
    }

    /* 3. RXNE 接收处理 */
    if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE) != RESET)
    {
        receive_data = (uint8_t)(USART1->DR);
        if (uart1_rx_len >= sizeof(uart1_rx_buf) - 1)
        {
            uart1_rx_len = 0;
        }
        uart1_rx_buf[uart1_rx_len++] = receive_data;
    }

    /* 4. IDLE 空闲处理 */
    if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE) != RESET)
    {
        __HAL_UART_CLEAR_IDLEFLAG(&huart1);
        uart1_rx_buf[uart1_rx_len] = '\0';
        uart1_rx_clear();
        // #if DEBUG_UART_ENABLE
        //         Log_Printf("recv: %s\r\n", uart1_rx_buf); /* 将接收到的数据打印出来 */
        // #endif
        //         uart1_rx_clear();
        //         __HAL_UART_CLEAR_IDLEFLAG(&huart1); /* 清除UART总线空闲中断 */
    }

    //     uint8_t receive_data = 0;
    //     if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE) != RESET)
    //     {                                                            /* 获取接收RXNE标志位是否被置位 */
    //         if (uart1_rx_len >= sizeof(uart1_rx_buf))                /* 如果接收的字符数大于接收缓冲区大小， */
    //             uart1_rx_len = 0;                                    /* 则将接收计数器清零 */
    //         HAL_UART_Receive(&huart1, &receive_data, 1, 1000); /* 接收一个字符 */
    //         uart1_rx_buf[uart1_rx_len++] = receive_data;             /* 将接收到的字符保存在接收缓冲区 */
    //     }
    //     if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE) != RESET) /* 获取接收空闲中断标志位是否被置位 */
    //     {
    // #if DEBUG_UART_ENABLE
    //         printf("recv: %s\r\n", uart1_rx_buf); /* 将接收到的数据打印出来 */
    // #endif
    //         uart1_rx_clear();
    //         __HAL_UART_CLEAR_IDLEFLAG(&huart1); /* 清除UART总线空闲中断 */
    //     }
}
