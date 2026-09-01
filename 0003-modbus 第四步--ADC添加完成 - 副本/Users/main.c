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
/* 离散输入变量 */
extern UCHAR ucSDiscInBuf[S_DISCRETE_INPUT_NDISCRETES / 8];
/* 线圈 */
extern UCHAR ucSCoilBuf[S_COIL_NCOILS / 8];
/* 输入寄存器 */
extern USHORT usSRegInBuf[S_REG_INPUT_NREGS];
/* 保持寄存器 */
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

    HAL_Init();                     /* 初始化HAL库 */
    stm32_clock_init(RCC_PLL_MUL9); /* 设置时钟, 72Mhz */

    /* 管脚时钟及GPIO初始化 */
    APPGPIO_INIT();
    /* ADC初始化 */
    MX_ADC1_Init();
    /* 定时器4初始化 */
    MX_TIM4_Init();

    /* Modbus初始化 */
    eMBInit(MB_RTU, MB_SAMPLE_TEST_SLAVE_ADDR, MB_MASTER_USARTx, 9600, MB_PAR_NONE);
    //    eMBInit(MB_RTU, MB_SAMPLE_TEST_SLAVE_ADDR, MB_MASTER_USARTx, MB_MASTER_USART_BAUDRATE, MB_PAR_NONE);

    /* 启动Mdobus */
    eMBEnable();

    uart_init(115200); // 初始化串口1

    printf("hello world 66666\r\n"); // 发送hello world 66666到串口1
    HAL_Delay(4000);

    while (1)
    {
        adc_value = Read_ADC(PT100_ADC, 1);
        usSRegHoldBuf[2] = adc_value;
        key_scan();
        Led_scan();
        //  //  // 在 main.c 中添加测试代码
        // uint8_t test_byte = 0xAA;
        // HAL_UART_Transmit(&huart2, &test_byte, 1, 1000); // 直接发送测试字节
        // /* 更新保持寄存器值 */       // usSRegHoldBuf[0] = HAL_GetTick() & 0xff;               // 获取时间戳 提出1至8位
        // /* 更新输入寄存器值 */       // usSRegInBuf[0] = HAL_GetTick() & 0xff;               // 获取时间戳 提出1至8位
        // /* 更新线圈 */              // ucSCoilBuf[0] = HAL_GetTick() & 0xff;               // 获取时间戳 提出1至8位
        // /* 离散输入变量 */          // ucSDiscInBuf[0] = HAL_GetTick() & 0xff;          // 获取时间戳 提出1至8位
        
        // /* 可以不用延时，如果延时时间过长主机会timeout */
        HAL_Delay(1000);

        /*从机轮询*/
        eMBPoll();
    }
}

// 解决 __aeabi_assert 未定义问题
void __aeabi_assert(const char *expr, const char *file, int line)
{
    // 空实现，断言失败时进入死循环
    while (1)
        ;
}

void Error_Handler(void)
{
    /* 用户错误处理，出现HAL初始化失败就卡死在这里 */
    while (1)
    {
        // 可以在这里加LED闪烁，提示出错
    }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
    while (1)
        ;
}
#endif
