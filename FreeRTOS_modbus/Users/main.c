
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

u8 led_num = 0;
void Led_scan(void)
{
    static uint8_t led_state_num = 0;

    // led_num = !led_num; // 切换led状态位

    if (led_state_num == 0)
    {
        led_state_num = 1;
        if (led_num)
            Led1_on; // 蓝灯
        else
            Led2_on; // 黄灯
    }
    else
    {
        led_state_num = 0;
        // if (led_num)
        Led1_off;
        // else
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

uint8_t R_Task = 0;
uint16_t adc_dma_result_min1 = 0xFFFF;
uint16_t adc_dma_result_max1 = 0;
uint16_t adc_dma_result_min2 = 0xFFFF;
uint16_t adc_dma_result_max2 = 0;

extern volatile u8 adc_done_flag;
u16 ADC_CHi_NUM1 = 100;
u16 ADC_CHi_NUM2 = 200;
u16 ADC_CHi_tmp1 = 0;
u16 ADC_CHi_tmp2 = 0;
u8 oled_ADC_CHi_num = 1;

void u16_to_4digits(char *s, uint16_t v)
{
    s[0] = '0' + (v / 1000) % 10;
    s[1] = '0' + (v / 100) % 10;
    s[2] = '0' + (v / 10) % 10;
    s[3] = '0' + v % 10;
    s[4] = '\0';
}

void ADC_DMA_scan(void)
{
    char str[5];
    uint8_t i = 0;
    uint16_t base, v0, v1;

    // if (R_Task != D_Task2)
    //     return;

    if (oled_ADC_CHi_num)
    {
        oled_ADC_CHi_num = 0;
        Show_Str(10, 10, "AD1:0000", 16, 0);
        Show_Str(90, 10, "AD2:0000", 16, 0);
    }
    if (adc_done_flag != 1 && adc_done_flag != 2)
        return;
    base = (adc_done_flag == 1) ? 0 : ADC_GROUP_PER_HALF;

    adc_dma_result_max1 = 0;
    adc_dma_result_min1 = 0xFFFF;
    adc_dma_result_max2 = 0;
    adc_dma_result_min2 = 0xFFFF;
    ADC_CHi_NUM1 = 0;
    ADC_CHi_NUM2 = 0;

    for (i = 0; i < ADC_GROUP_PER_HALF; i++)
    {
        v0 = adc_result[base + i][0];
        v1 = adc_result[base + i][1];

        ADC_CHi_NUM1 += v0;
        ADC_CHi_NUM2 += v1;

        if (v0 < adc_dma_result_min1)
            adc_dma_result_min1 = v0;
        if (v0 > adc_dma_result_max1)
            adc_dma_result_max1 = v0;
        if (v1 < adc_dma_result_min2)
            adc_dma_result_min2 = v1;
        if (v1 > adc_dma_result_max2)
            adc_dma_result_max2 = v1;
    }

    ADC_CHi_NUM1 = (ADC_CHi_NUM1 - adc_dma_result_max1 - adc_dma_result_min1) / (ADC_GROUP_PER_HALF - 2);
    ADC_CHi_NUM2 = (ADC_CHi_NUM2 - adc_dma_result_max2 - adc_dma_result_min2) / (ADC_GROUP_PER_HALF - 2);

    if (ADC_CHi_tmp1 != ADC_CHi_NUM1)
    {
        ADC_CHi_tmp1 = ADC_CHi_NUM1;
        u16_to_4digits(str, ADC_CHi_NUM1);
        // sprintf(str, "%04u", ADC_CHi_NUM1);
        Show_Str(42, 10, (u8 *)str, 16, 0);
        // Show_Str((base == 0 ? 58 : 168), 10, (u8 *)str, 24, 0);
    }
    if (ADC_CHi_tmp2 != ADC_CHi_NUM2)
    {
        ADC_CHi_tmp2 = ADC_CHi_NUM2;
        u16_to_4digits(str, ADC_CHi_NUM2);
        Show_Str(122, 10, (u8 *)str, 16, 0);
        //        Show_Str(42, 30, (u8 *)str, 16, 0);
        // Show_Str((base == 0 ? 58 : 168), 40, (u8 *)str, 24, 0);
    }
}

int main(void)
{
    // uint8_t num = 0;

    HAL_Init();                     /* 初始化HAL */
    stm32_clock_init(RCC_PLL_MUL9); /* 初始化时钟为72Mhz */

    delay_init(); // 初始化延时
    DWT_Init();   // 初始化 DWT

    /* 初始化GPIO */
    APPGPIO_INIT();
    /* ADC初始化 */
    MX_ADC1_Init();
#if ADC_DMA_OPEN
    adc_dma_init((uint16_t *)&adc_result); // 初始化ADC DMA模式
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
    freertos_test(); // 测试FreeRTOS

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
