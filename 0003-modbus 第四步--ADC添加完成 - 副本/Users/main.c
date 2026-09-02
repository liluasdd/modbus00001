#include "sys.h"
#include "delay.h"

#include "uart.h"

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/

#include "mb.h"
#include "mbport.h"
#include "user_mb_app.h"

/* Private user code ---------------------------------------------------------*/

volatile bit_flag flag1 = {0}, flag2 = {0}, flag3 = {0}, flag4 = {0}, flag5 = {0}, flag6 = {0}, flag7 = {0};

/* 离散输入缓冲区（只读） */
extern UCHAR ucSDiscInBuf[S_DISCRETE_INPUT_NDISCRETES / 8];
/* 线圈缓冲区（读写） */
extern UCHAR ucSCoilBuf[S_COIL_NCOILS / 8];
/* 输入寄存器缓冲区（只读） */
extern USHORT usSRegInBuf[S_REG_INPUT_NREGS];
/* 保持寄存器缓冲区（读写） */
extern USHORT usSRegHoldBuf[S_REG_HOLDING_NREGS];
int16_t adc_value = 0;

u8 led_num = 0;
void Led_scan(void)
{
    static uint8_t led_state_num = 0;

    // led_num = !led_num; // 切换led状态位

    if (led_state_num == 0)
    {
        led_state_num = 1;
        if (led_num)
            Led1_on;
        else
            Led2_on;
    }
    else
    {
        led_state_num = 0;
        if (led_num)
            Led1_off;
        else
            Led2_off;
    }
}

/*
//索引对应真实波特率
const uint32_t baud_table[] = {2400,4800,9600,19200,38400,115200};

// g_dev_baud_idx 就是Flash读出来的uint8_t索引
if(g_dev_baud_idx < sizeof(baud_table)/sizeof(uint32_t))
{
    BAUD_num = baud_table[g_dev_baud_idx];
}
else
{
    BAUD_num = 9600; //索引非法，恢复默认
}
 */

int main(void)
{
    // uint8_t num = 0;

    HAL_Init();                     /* 初始化HAL */
    stm32_clock_init(RCC_PLL_MUL9); /* 初始化时钟为72Mhz */

    /* 初始化GPIO */
    APPGPIO_INIT();
    /* ADC初始化 */
    MX_ADC1_Init();
    /* 定时器4初始化 */
    MX_TIM4_Init();
    /* 定时器3初始化 - 100us定时 */
    MX_TIM3_Init();
    HAL_TIM_Base_Start_IT(&htim3); /* 启动TIM3定时器中断 */

    /* Modbus从站初始化 */
    eMBInit(MB_RTU, MB_SAMPLE_TEST_SLAVE_ADDR, MB_MASTER_USARTx, 9600, MB_PAR_NONE); // MB_PAR_NONE无校验 MB_PAR_ODD奇校验 MB_PAR_EVEN偶校验

    /*
    MB_RTU modbus选择。  MB_SAMPLE_TEST_SLAVE_ADDR 从站地址为0x01 。MB_MASTER_USARTx 串口2 。 BAUD_num 波特率 。 MB_PAR_NONE 校验选择
    */
    // eMBInit(MB_RTU, MB_SAMPLE_TEST_SLAVE_ADDR, MB_MASTER_USARTx, BAUD_num, MB_PAR_NONE); // MB_PAR_NONE无校验 MB_PAR_ODD奇校验 MB_PAR_EVEN偶校验
    //    eMBInit(MB_RTU, MB_SAMPLE_TEST_SLAVE_ADDR, MB_MASTER_USARTx, MB_MASTER_USART_BAUDRATE, MB_PAR_NONE);

    /* 启用Modbus从站 */
    eMBEnable();

    uart_init(115200); // 初始化串口1

    printf("hello world 66666\r\n"); //  串口1  打印 hello world 66666
    HAL_Delay(4000);                 //  延时4秒

    while (1)
    {
        if (g_b_task_jtim == 1)
        {
            g_b_task_jtim = 0;

            key_scan();
            if (g_b_2s_jtim == 1)
            {
                g_b_2s_jtim = 0;
                Led_scan();
            }

            // uint8_t test_byte = 0xAA;
            // HAL_UART_Transmit(&huart2, &test_byte, 1, 1000); // 串口2  打印 0xAA

            // // /* 1秒轮询一次Modbus从站，处理数据交换 */
            // HAL_Delay(1000);

            /* 发送超时故障处理 */
            eMBsend_Error();
            /* 轮询Modbus从站 */
            eMBPoll();
        }
        if (g_b_100ms_jtim == 1)
        {
            g_b_100ms_jtim = 0;
            adc_value = Read_ADC(PT100_ADC, 1);
            usSRegHoldBuf[1] = adc_value;
        }
    }
}

//  __aeabi_assert 函数，用于断言检查
void __aeabi_assert(const char *expr, const char *file, int line)
{
    // 断言检查失败时循环等待
    while (1)
        ;
}

void Error_Handler(void)
{
    /* 初始化HAL失败 */
    while (1)
    {
        // 闪烁LED闪烁错误信息
    }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
    while (1)
        ;
}
#endif
