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
/* 离散输入缓冲区（只读） */
extern UCHAR ucSDiscInBuf[S_DISCRETE_INPUT_NDISCRETES / 8];
/* 线圈缓冲区（读写） */
extern UCHAR ucSCoilBuf[S_COIL_NCOILS / 8];
/* 输入寄存器缓冲区（只读） */
extern USHORT usSRegInBuf[S_REG_INPUT_NREGS];
/* 保持寄存器缓冲区（读写） */
extern USHORT usSRegHoldBuf[S_REG_HOLDING_NREGS];
int16_t adc_value = 0;

void Led_scan(void)
{
    static uint8_t led_state_num = 0;
    if (led_state_num == 0)
    {
        led_state_num = 1;
        Led1_on;
        Led2_on;
    }
    else
    {
        led_state_num = 0;
        Led1_off;
        Led2_off;
    }
}

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

    /* Modbus从站初始化 */
    eMBInit(MB_RTU, MB_SAMPLE_TEST_SLAVE_ADDR, MB_MASTER_USARTx, 9600, MB_PAR_NONE);
    //    eMBInit(MB_RTU, MB_SAMPLE_TEST_SLAVE_ADDR, MB_MASTER_USARTx, MB_MASTER_USART_BAUDRATE, MB_PAR_NONE);

    /* 启用Modbus从站 */
    eMBEnable();

    uart_init(115200); // 初始化串口1

    printf("hello world 66666\r\n"); //  串口1  打印 hello world 66666
    HAL_Delay(4000);  //  延时4秒

    while (1)
    {
        adc_value = Read_ADC(PT100_ADC, 1);
        usSRegHoldBuf[2] = adc_value;
        key_scan();
        Led_scan();

        // uint8_t test_byte = 0xAA;
        // HAL_UART_Transmit(&huart2, &test_byte, 1, 1000); // 串口2  打印 0xAA
        
        // /* 1秒轮询一次Modbus从站，处理数据交换 */
        HAL_Delay(1000);

        /* 轮询Modbus从站 */
        eMBPoll();
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