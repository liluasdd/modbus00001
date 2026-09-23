#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* USER CODE END Private defines */
#include "main.h"

#define DEBUG_UART_ENABLE 0 //1 开启调试串口打印，0关闭
#define CONFIG_NEED_MESSAGESMG 0 //1 开启消息管理，0关闭
#define CONFIG_NEED_PWM 0 //1 开启PWM，0关闭

    // 函数声明
    extern void APPGPIO_INIT(void);

    /* USER CODE BEGIN Prototypes */
    /* USER CODE END Private defines */

#define Led1_pin GPIO_PIN_8
#define Led1_port GPIOB
// LED1 操作宏定义（直接操作寄存器）
#define Led1_on Led1_port->BRR = Led1_pin      // 拉低引脚（低电平点亮）
#define Led1_off Led1_port->BSRR = Led1_pin    // 拉高引脚
#define Led1_toggle Led1_port->ODR ^= Led1_pin // 翻转引脚状态
#define Led1_state (Led1_port->IDR & Led1_pin) // 读取引脚状态
// LED1 上拉/下拉配置宏（直接操作PUPDR寄存器）
/*
led1_pin = GPIO_PIN_8, PUPDR[17:16]控制引脚8
低位为2n,高位为2n+1
00 无上拉下拉
01 上拉
10 下拉
11 保留（未使用）
*/
#define LED1_PULL_UP                    \
    {                                   \
        led1_port->PUPDR &= ~(1 << 17); \
        led1_port->PUPDR |= (1 << 16);  \
    } // 上拉
#define LED1_PULL_DOWN                  \
    {                                   \
        led1_port->PUPDR |= (1 << 17);  \
        led1_port->PUPDR &= ~(1 << 16); \
    } // 下拉
#define LED1_PULL_NONE                  \
    {                                   \
        led1_port->PUPDR &= ~(1 << 17); \
        led1_port->PUPDR &= ~(1 << 16); \
    } // 无上拉下拉

// LED1 HAL库模式配置宏 - 输入模式
#define LED1_T_Int                                  \
    {                                               \
        GPIO_InitTypeDef GPIO_InitStruct = {0};     \
        GPIO_InitStruct.Pin = Led1_pin;             \
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;     \
        GPIO_InitStruct.Pull = GPIO_NOPULL;         \
        HAL_GPIO_Init(Led1_port, &GPIO_InitStruct); \
    }
// LED1 HAL库模式配置宏 - 模拟模式
#define LED1_T_Ang                                  \
    {                                               \
        GPIO_InitTypeDef GPIO_InitStruct = {0};     \
        GPIO_InitStruct.Pin = Led1_pin;             \
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;    \
        HAL_GPIO_Init(Led1_port, &GPIO_InitStruct); \
    }
// LED1 HAL库模式配置宏 - 推挽输出
#define LED1_T_Out                                    \
    {                                                 \
        GPIO_InitTypeDef GPIO_InitStruct = {0};       \
        GPIO_InitStruct.Pin = Led1_pin;               \
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;   \
        GPIO_InitStruct.Pull = GPIO_NOPULL;           \
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; \
        HAL_GPIO_Init(Led1_port, &GPIO_InitStruct);   \
    }

#define Led2_pin GPIO_PIN_9
#define Led2_port GPIOB
// LED2 操作宏定义（直接操作寄存器）
#define Led2_on Led2_port->BRR = Led2_pin      // 拉低引脚（低电平点亮）
#define Led2_off Led2_port->BSRR = Led2_pin    // 拉高引脚
#define Led2_toggle Led2_port->ODR ^= Led2_pin // 翻转引脚状态
#define Led2_state (Led2_port->IDR & Led2_pin) // 读取引脚状态
    // LED2 上拉/下拉配置宏（直接操作PUPDR寄存器）
    // Led2_pin = GPIO_PIN_9, PUPDR[19:18]控制引脚9   低位为2n,高位为2n+1

#define key1_pin GPIO_PIN_0
#define key1_port GPIOA
// KEY1 读取宏
#define KEY1_STATE (key1_port->IDR & key1_pin)
#define KEY1_PRESSED (!KEY1_STATE) // 假设按键按下为低电平
// KEY1 上拉/下拉配置宏（直接操作PUPDR寄存器）
// ley1_pin = GPIO_PIN_0, PUPDR[1:0]控制引脚0
#define KEY1_PULL_UP                   \
    {                                  \
        key1_port->PUPDR &= ~(1 << 1); \
        key1_port->PUPDR |= (1 << 0);  \
    } // 上拉
#define KEY1_PULL_DOWN                 \
    {                                  \
        key1_port->PUPDR |= (1 << 1);  \
        key1_port->PUPDR &= ~(1 << 0); \
    } // 下拉
#define KEY1_PULL_NONE                 \
    {                                  \
        key1_port->PUPDR &= ~(1 << 1); \
        key1_port->PUPDR &= ~(1 << 0); \
    } // 无上拉下拉
// KEY1 HAL库模式配置宏
#define KEY1_T_Int                                  \
    {                                               \
        GPIO_InitTypeDef GPIO_InitStruct = {0};     \
        GPIO_InitStruct.Pin = key1_pin;             \
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;     \
        GPIO_InitStruct.Pull = GPIO_PULLUP;         \
        HAL_GPIO_Init(ley1_port, &GPIO_InitStruct); \
    }
#define KEY1_T_Ang                                  \
    {                                               \
        GPIO_InitTypeDef GPIO_InitStruct = {0};     \
        GPIO_InitStruct.Pin = key1_pin;             \
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;    \
        HAL_GPIO_Init(key1_port, &GPIO_InitStruct); \
    }
#define KEY1_T_Out                                    \
    {                                                 \
        GPIO_InitTypeDef GPIO_InitStruct = {0};       \
        GPIO_InitStruct.Pin = key1_pin;               \
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;   \
        GPIO_InitStruct.Pull = GPIO_NOPULL;           \
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; \
        HAL_GPIO_Init(ley1_port, &GPIO_InitStruct);   \
    }

#define key2_pin GPIO_PIN_1
#define key2_port GPIOA
// KEY2 读取宏
#define KEY2_STATE (key2_port->IDR & key2_pin)
#define KEY2_PRESSED (!KEY2_STATE) // 假设按键按下为低电平

#define USART1_TX_pin GPIO_PIN_9
#define USART1_TX_port GPIOA
#define USART1_RX_pin GPIO_PIN_10
#define USART1_RX_port GPIOA

#define USART2_TX_pin GPIO_PIN_2
#define USART2_TX_port GPIOA
#define USART2_RX_pin GPIO_PIN_3
#define USART2_RX_port GPIOA

#define RS485_DE_pin GPIO_PIN_4
#define RS485_DE_port GPIOA
#define RS485_DE_HIGH_ON RS485_DE_port->BSRR = RS485_DE_pin // 发送模式 (DE/RE 高电平使能发送)
#define RS485_DE_LOW_OFF RS485_DE_port->BRR = RS485_DE_pin  // 接收模式 (DE/RE 低电平使能接收)

#define PT100_ADC_pin GPIO_PIN_5
#define PT100_ADC_port GPIOA

#define USART3_TX_pin GPIO_PIN_10
#define USART3_TX_port GPIOB
#define USART3_RX_pin GPIO_PIN_11
#define USART3_RX_port GPIOB

#define SWDIO_pin GPIO_PIN_13
#define SWDIO_port GPIOA
#define SWCLK_pin GPIO_PIN_14
#define SWCLK_port GPIOA

#define IIC_scl_pin GPIO_PIN_6
#define IIC_scl_port GPIOB
#define IIC_sda_pin GPIO_PIN_7
#define IIC_sda_port GPIOB

// Modbus 485收发控制脚
#define RS485_EN_PIN GPIO_PIN_4
#define RS485_EN_PORT GPIOA
#define RS485_TX_EN RS485_EN_PORT->BSRR = RS485_EN_PIN
#define RS485_RX_EN RS485_EN_PORT->BRR = RS485_EN_PIN
// 485推挽输出初始化宏
#define RS485_EN_OUT_INIT                               \
    {                                                   \
        GPIO_InitTypeDef GPIO_InitStruct = {0};         \
        GPIO_InitStruct.Pin = RS485_EN_PIN;             \
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;     \
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;   \
        GPIO_InitStruct.Pull = GPIO_NOPULL;             \
        HAL_GPIO_Init(RS485_EN_PORT, &GPIO_InitStruct); \
    }

// ADC通道定义（HAL库风格）- STM32F103C8T6
#define AP_CHN_VBAT ADC_CHANNEL_VREFINT    // 内部参考电压通道
#define AP_CHN_TEMP ADC_CHANNEL_TEMPSENSOR // 内部温度传感器通道
#define USB_ADC ADC_CHANNEL_5              // PA5
#define AP_CIRCUIT ADC_CHANNEL_3           // PA3
#define AP_CIRCUIT2 ADC_CHANNEL_7          // PA7

// 标准库完整配置
#define led1_pin GPIO_Pin_8
#define led1_port GPIOB
//// 模式配置宏
// #def ine  LED1_T_Int                                            \
//    {                                                         \
//        GPIO_InitTypeDef GPIO_InitStructure;                  \
//        GPIO_InitStructure.GPIO_Pin = led1_pin;               \
//        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; \
//        GPIO_Init(led1_port, &GPIO_InitStructure);            \
//    }
// #def ine  LED1_T_Out                                        \
//    {                                                     \
//        GPIO_InitTypeDef GPIO_InitStructure;              \
//        GPIO_InitStructure.GPIO_Pin = led1_pin;           \
//        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  \
//        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; \
//        GPIO_Init(led1_port, &GPIO_InitStructure);        \
//    }
//  操作宏
#define LED1_ON GPIO_ResetBits(led1_port, led1_pin)
#define LED1_OFF GPIO_SetBits(led1_port, led1_pin)
#define LED1_TOGGLE GPIO_WriteBit(led1_port, led1_pin, \
                                  (BitAction)(1 - GPIO_ReadOutputDataBit(led1_port, led1_pin)))

#define PT100_ADC ADC_CHANNEL_5
/* USER CODE BEGIN Prototypes */
#ifdef __cplusplus
}
#endif
#endif
