#include "gpio.h"
#include "main.h"

void APPGPIO_INIT(void)
{
    // 初始化GPIO结构体
    GPIO_InitTypeDef gpio_initstruct;

    // 打开时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /////*===== PA6: IIC_SCL  PA7: IIC_SDA  =====*/
    gpio_initstruct.Pin = IIC_scl_pin;
    gpio_initstruct.Mode = GPIO_MODE_OUTPUT_OD; // 改为开漏输出
    gpio_initstruct.Pull = GPIO_PULLUP;         // 需要外部上拉
    gpio_initstruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(IIC_scl_port, &gpio_initstruct);

    gpio_initstruct.Pin = IIC_sda_pin;
    gpio_initstruct.Mode = GPIO_MODE_OUTPUT_OD; // 改为开漏输出
    gpio_initstruct.Pull = GPIO_PULLUP;
    gpio_initstruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(IIC_sda_port, &gpio_initstruct);

    /////*=====输出=====*/
    // 调用GPIO初始化函数
    // gpio_initstruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;  // 两个LED对应的引脚
    gpio_initstruct.Pin = Led1_pin | Led2_pin;    // GPIO_PIN_8 // GPIO_PIN_9
    gpio_initstruct.Mode = GPIO_MODE_OUTPUT_PP;   // 推挽输出
    gpio_initstruct.Pull = GPIO_PULLUP;           // 上拉
    gpio_initstruct.Speed = GPIO_SPEED_FREQ_HIGH; // 高速
    HAL_GPIO_Init(Led1_port, &gpio_initstruct);   // GPIOB
    // 关闭LED
    Led1_off;
    Led2_off;

    /////*=====复用推挽输出=====*/
    /* PA9: USART1_TX */
    gpio_initstruct.Pin = USART1_TX_pin;
    gpio_initstruct.Mode = GPIO_MODE_AF_PP; // ? 复用推挽
    gpio_initstruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(USART1_TX_port, &gpio_initstruct);

    /* PA2: USART2_TX */
    gpio_initstruct.Pin = USART2_TX_pin;    // GPIO_PIN_2
    gpio_initstruct.Mode = GPIO_MODE_AF_PP; // ? 复用推挽输出
    gpio_initstruct.Pull = GPIO_NOPULL;
    gpio_initstruct.Speed = GPIO_SPEED_FREQ_HIGH;
    //    gpio_initstruct.Alternate = GPIO_AF7_USART2; // 复用推挽输出
    HAL_GPIO_Init(USART2_TX_port, &gpio_initstruct); // GPIOA

    /* PB10: USART3_TX */
    gpio_initstruct.Pin = USART3_TX_pin;    // GPIO_PIN_2
    gpio_initstruct.Mode = GPIO_MODE_AF_PP; // ? 复用推挽输出
    gpio_initstruct.Pull = GPIO_NOPULL;
    gpio_initstruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(USART3_TX_port, &gpio_initstruct); // GPIOA

    /* PA4: RS485 DE/RE 控制（普通推挽输出） */
    gpio_initstruct.Pin = RS485_DE_pin; // GPIO_PIN_4
    gpio_initstruct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_initstruct.Pull = GPIO_NOPULL;
    gpio_initstruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(RS485_DE_port, &gpio_initstruct); // GPIOB

    ///////*=====输入=====*/
    /*  PA10: USART1_RX */
    gpio_initstruct.Pin = USART1_RX_pin;
    gpio_initstruct.Mode = GPIO_MODE_INPUT;          // ? 输入
    gpio_initstruct.Pull = GPIO_PULLUP;              // ? 上拉（推荐）
    HAL_GPIO_Init(USART1_RX_port, &gpio_initstruct); // GPIOA

    /* PA3: USART2_RX */
    gpio_initstruct.Pin = USART2_RX_pin; // GPIO_PIN_3
    gpio_initstruct.Mode = GPIO_MODE_INPUT;
    gpio_initstruct.Pull = GPIO_PULLUP;
    // gpio_initstruct.Alternate = GPIO_AF7_USART2;//复用输入
    HAL_GPIO_Init(USART2_RX_port, &gpio_initstruct); // GPIOA

    /* PB11: USART3_RX */
    gpio_initstruct.Pin = USART3_RX_pin; // GPIO_PIN_3
    gpio_initstruct.Mode = GPIO_MODE_INPUT;
    gpio_initstruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(USART3_RX_port, &gpio_initstruct); // GPIOB

    // 调用GPIO初始化函数  GPIO_PIN_0 GPIO_PIN_1
    gpio_initstruct.Pin = key1_pin | key2_pin;    // 两个按键对应的引脚
    gpio_initstruct.Mode = GPIO_MODE_INPUT;       // 输入
    gpio_initstruct.Pull = GPIO_NOPULL;           // 无拉
    gpio_initstruct.Speed = GPIO_SPEED_FREQ_HIGH; // 高速
    HAL_GPIO_Init(key1_port, &gpio_initstruct);

    /* ADC 模拟输入 */
    gpio_initstruct.Pin = PT100_ADC_pin;             // GPIO_PIN_5
    gpio_initstruct.Mode = GPIO_MODE_ANALOG;         // 模拟输入
    gpio_initstruct.Pull = GPIO_NOPULL;              // 无拉
    gpio_initstruct.Speed = GPIO_SPEED_FREQ_HIGH;    // 高速
    HAL_GPIO_Init(PT100_ADC_port, &gpio_initstruct); // GPIOA
}

// void SWDIO_INIT(void)
// {

// }
