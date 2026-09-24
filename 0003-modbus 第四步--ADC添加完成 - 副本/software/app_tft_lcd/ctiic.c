#include "ctiic.h"
#include "delay.h"

// 控制I2C速度的延时
void CT_Delay(void)
{
	delay_us(5);
}
// 电容触摸芯片IIC接口初始化
void CT_IIC_Init(void)
{
	    // 初始化GPIO结构体
    GPIO_InitTypeDef gpio_initstruct;
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

	// gpio_initstruct.Mode = GPIO_MODE_OUTPUT_OD; // 改为开漏输出
	gpio_initstruct.Mode = GPIO_MODE_OUTPUT_PP; // 改为推挽输出
	gpio_initstruct.Pull = GPIO_NOPULL;			// 不上下拉
	gpio_initstruct.Speed = GPIO_SPEED_FREQ_HIGH;

	gpio_initstruct.Pin = T_CS_pin; // B1
	HAL_GPIO_Init(SPI_led_port, &gpio_initstruct);
	SPI_led_HIGH_ON;

	gpio_initstruct.Pin = T_CLK_pin; // B10
	HAL_GPIO_Init(T_CLK_port, &gpio_initstruct);
	T_CLK_HIGH_ON;

	gpio_initstruct.Pin = T_SDI_pin; // B11
	HAL_GPIO_Init(T_SDI_port, &gpio_initstruct);
	T_SDI_HIGH_ON;

	gpio_initstruct.Pin = SPI_reset_pin; // B13
	HAL_GPIO_Init(SPI_reset_port, &gpio_initstruct);
	SPI_reset_HIGH_ON;

	gpio_initstruct.Pin = T_IRQ_pin; // B14
	HAL_GPIO_Init(T_IRQ_port, &gpio_initstruct);
	T_IRQ_HIGH_ON;

	// // 一次性把 PB1、PB10、PB11、PB13、PB14 全部置高
	// GPIOB->ODR |= (1 << 1) | (1 << 10) | (1 << 11) | (1 << 13) | (1 << 14);
}
// 产生IIC起始信号
void CT_IIC_Start(void)
{
	T_SDA_OUT(); // sda线输出
	T_SDI_HIGH_ON;
	T_CLK_HIGH_ON;
	delay_us(30);
	T_SDI_LOW_OFF; // START:when CLK is high,DATA change form high to low
	CT_Delay();
	T_CLK_LOW_OFF; // 钳住I2C总线，准备发送或接收数据
}
// 产生IIC停止信号
void CT_IIC_Stop(void)
{
	T_SDA_OUT(); // sda线输出
	T_CLK_HIGH_ON;
	delay_us(30);
	T_SDI_LOW_OFF; // STOP:when CLK is high DATA change form low to high
	CT_Delay();
	T_SDI_HIGH_ON; // 发送I2C总线结束信号
}
// 等待应答信号到来
// 返回值：1，接收应答失败
//         0，接收应答成功
u8 CT_IIC_Wait_Ack(void)
{
	u8 ucErrTime = 0;
	T_SDA_IN(); // SDA设置为输入
	T_SDI_HIGH_ON;
	T_CLK_HIGH_ON;
	CT_Delay();
	while (T_SDI_state)//T_SDI_state
	{
		ucErrTime++;
		if (ucErrTime > 250)
		{
			CT_IIC_Stop();
			return 1;
		}
		CT_Delay();
	}
	T_CLK_LOW_OFF; // 时钟输出0
	return 0;
}
// 产生ACK应答
void CT_IIC_Ack(void)
{
	T_CLK_LOW_OFF;
	T_SDA_OUT();
	CT_Delay();
	T_SDI_LOW_OFF;
	CT_Delay();
	T_CLK_HIGH_ON;
	CT_Delay();
	T_CLK_LOW_OFF;
}
// 不产生ACK应答
void CT_IIC_NAck(void)
{
	T_CLK_LOW_OFF;
	T_SDA_OUT();
	CT_Delay();
	T_SDI_HIGH_ON;
	CT_Delay();
	T_CLK_HIGH_ON;
	CT_Delay();
	T_CLK_LOW_OFF;
}
// IIC发送一个字节
// 返回从机有无应答
// 1，有应答
// 0，无应答
void CT_IIC_Send_Byte(u8 txd)
{
	u8 t;
	T_SDA_OUT();
	T_CLK_LOW_OFF; // 拉低时钟开始数据传输
	CT_Delay();
	for (t = 0; t < 8; t++)
	{
		// CT_IIC_SDA = (txd & 0x80) >> 7;
		if ((txd & 0x80) >> 7)
		{
			T_SDI_HIGH_ON;
		}
		else
		{
			T_SDI_LOW_OFF;
		}

		txd <<= 1;
		T_CLK_HIGH_ON;
		CT_Delay();
		T_CLK_LOW_OFF;
		CT_Delay();
	}
}
// 读1个字节，ack=1时，发送ACK，ack=0，发送nACK
u8 CT_IIC_Read_Byte(unsigned char ack)
{
	u8 i, receive = 0;
	T_SDA_IN(); // SDA设置为输入
	delay_us(30);
	for (i = 0; i < 8; i++)
	{
		T_CLK_LOW_OFF;
		CT_Delay();
		T_CLK_HIGH_ON;
		receive <<= 1;
		if (T_SDI_state)//T_SDI_state
			receive++;
	}
	if (!ack)
		CT_IIC_NAck(); // 发送nACK
	else
		CT_IIC_Ack(); // 发送ACK
	return receive;
}
