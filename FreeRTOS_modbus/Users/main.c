
/* Includes ------------------------------------------------------------------*/
// #include "stm32f1xx.h"
#include "main.h"
#include "touch.h"

/* Private includes ----------------------------------------------------------*/

// #include "mb.h"
// #include "mbport.h"
// #include "user_mb_app.h"

// volatile uint16_t adc_result[2 * ADC_GROUP_PER_HALF * ADC_CH_NUM] = {0}; // [20][2]
volatile uint16_t adc_result[2 * ADC_GROUP_PER_HALF][ADC_CH_NUM];
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

int main(void)
{
    // uint8_t num = 0;

    HAL_Init();                     /* 初始化HAL */
    stm32_clock_init(RCC_PLL_MUL9); /* 初始化时钟为72Mhz */

    delay_init(); // 初始化延时
    DWT_Init();   // 初始化 DWT

    /* 初始化GPIO */
    APPGPIO_INIT();

    uart_init(115200); // 初始化串口1
    Log_Init();        // 创建互斥锁
#if ADC_DMA_OPEN
    adc_dma_init((uint16_t *)&adc_result); // 初始化ADC DMA模式
#else
    MX_ADC1_Init();//ADC初始化
#endif

    /* 定时器4初始化 */
    MX_TIM4_Init();
    /* 定时器3初始化 - 100us定时 */
    MX_TIM2_Init();
    MX_TIM3_Init();
#if ADC_DMA_OPEN
    start_adc();
#endif
    /*  Modbus_RTU从站初始化
    MB_SAMPLE_TEST_SLAVE_ADDR 从站地址为0x01    MB_MASTER_USARTx 串口2   MB_MASTER_USART_BAUDRATE 波特率
    MB_PAR_NONE 校验选择    MB_PAR_NONE 无校验 MB_PAR_ODD奇校验 MB_PAR_EVEN偶校验 */
    eMBInit(MB_RTU, MB_SAMPLE_TEST_SLAVE_ADDR, MB_MASTER_USARTx, MB_MASTER_USART_BAUDRATE, MB_PAR_NONE);
    eMBEnable();                     /* 启用Modbus_RTU从站 */
    LCD_Init();                      // 初始化LCD
    tp_dev.touchtype |= USE_TP_TYPE; //// 电屏容触摸&电阻触摸

    LCD_Display_Dir(1);  // 屏幕方向    // LCD_Display_Dir(USE_LCM_DIR);	幕方向
    POINT_COLOR = WHITE; // 设置字体为白色
    BACK_COLOR = BLACK;  // 背景色
    LCD_Clear(BLACK);    // 清屏

#if DEBUG_UART_ENABLE
    uart_init(115200);               // 初始化串口1
    printf("hello world 66666\r\n"); //  串口1  打印 hello world 66666
    HAL_Delay(400);                  //  延时400ms
#endif
    HAL_Delay(200); //  延时200ms    // delay_ms(3000);
    /* Init scheduler */
    freertos_test(); // 初始化FreeRTOS

    vTaskStartScheduler(); // 启动任务调度器
    while (1)
    {
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

/* 声明为 weak，防止和其他库冲突 */
__weak void vAssertCalled(const char *pcFile, int line)
{
    // 1. 如果你有调试串口，可以把错误打印出来
    // printf("FreeRTOS Assert Failed! File: %s, Line: %d\r\n", pcFile, line);

    // 2. 关闭中断
    taskDISABLE_INTERRUPTS();

    // 3. 死循环，等待调试器接管
    for (;;)
        ;
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
    while (1)
        ;
}
#endif

////////////////////////////////////////////////////////////////////
//                          _ooOoo_                               //
//                         o8888888o                              //
//                         88" . "88                              //
//                         (| ^_^ |)                              //
//                         O\  =  /O                              //
//                      ____/`---'\____                           //
//                    .'  \\|     |//  `.                         //
//                   /  \\|||  :  |||//  \                        //
//                  /  _||||| -:- |||||-  \                       //
//                  |   | \\\  -  /// |   |                       //
//                  | \_|  ''\---/''  |   |                       //
//                  \  .-\__  `-`  ___/-. /                       //
//                ___`. .'  /--.--\  `. . ___                     //
//              ."" '<  `.___\_<|>_/___.'  >'"".                  //
//            | | :  `- \`.;`\ _ /`;.`/ - ` : | |                 //
//            \  \ `-.   \_ __\ /__ _/   .-` /  /                 //
//      ========`-.____`-.___\_____/___.-`____.-'========         //
//                           `=---='                              //
//      ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^        //
//         佛祖保佑       永无BUG     永不修改                    //
////////////////////////////////////////////////////////////////////
