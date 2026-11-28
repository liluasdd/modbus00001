#include "main.h"
#include "APP_FLASH.h"
//#include "main_2.h"

#include "stm32f1xx_hal.h" // 按你的芯片系列修改
#include <stdbool.h>
#include <stdint.h>

/* ================= AT24C64 参数 ================= */
#define AT24C64_BASE_ADDR 0x50u /* 7-bit: 1010 A2 A1 A0，A2/A1/A0 接地 */
#define AT24C64_DEV_WRITE ((AT24C64_BASE_ADDR << 1) | 0u)
#define AT24C64_DEV_READ ((AT24C64_BASE_ADDR << 1) | 1u)
#define AT24C64_PAGE_SIZE 32u
#define AT24C64_MEM_SIZE 8192u
#define AT24C64_delay 3u

// static inline void scl_high(void)
// {
// 	// SCL 高电平
//     HAL_GPIO_WritePin(I2C_SCL_PORT, I2C_SCL_PIN, GPIO_PIN_SET);
// }
// static inline void scl_low(void)
// {
//     HAL_GPIO_WritePin(I2C_SCL_PORT, I2C_SCL_PIN, GPIO_PIN_RESET);
// }
// static inline void sda_high(void)
// {
//     HAL_GPIO_WritePin(I2C_SDA_PORT, I2C_SDA_PIN, GPIO_PIN_SET);
// }
// static inline void sda_low(void)
// {
//     HAL_GPIO_WritePin(I2C_SDA_PORT, I2C_SDA_PIN, GPIO_PIN_RESET);
// }
// static inline uint8_t sda_read(void)
// {
// 	// 读取 SDA 状态
//     return HAL_GPIO_ReadPin(I2C_SDA_PORT, I2C_SDA_PIN) == GPIO_PIN_SET ? 1u : 0u;
// }

/* 软件延时，按主频和示波器实测调整。默认约适合 100kHz 左右 */
// static void i2c_delay(uint16_t delay) // static void i2c_delay(void)
// {
// 	for (volatile uint16_t i = 0; i < delay; i++) // for (volatile uint32_t i = 0; i < 12u; i++)
// 	{
// 		__NOP();
// 	}
// }

void soft_i2c_init(void)
{
	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = I2C_SCL_PIN | I2C_SDA_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD; /* 开漏输出 */
	GPIO_InitStruct.Pull = GPIO_PULLUP;			/* 外部 4.7k 上拉仍然推荐 */
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(I2C_SCL_PORT, &GPIO_InitStruct);

	scl_high;
	sda_high;
}

/* ================= 软件 I2C 基本时序 ================= */
static void i2c_start(void)
{
	sda_high;
	scl_high;
	delay_us(AT24C64_delay);

	sda_low; /* SCL 高电平期间 SDA 下降：起始条件 */
	delay_us(AT24C64_delay);

	scl_low;
	delay_us(AT24C64_delay);
}

static void i2c_stop(void)
{
	scl_low;
	sda_low;
	delay_us(AT24C64_delay);

	scl_high;
	delay_us(AT24C64_delay);

	sda_high; /* SCL 高电平期间 SDA 上升：停止条件 */
	delay_us(AT24C64_delay);
}

/* 返回 true 表示收到 ACK */
static bool i2c_write_byte(uint8_t data)
{
	uint8_t ack;

	for (uint8_t i = 0; i < 8u; i++)
	{
		if (data & 0x80u)
		{
			sda_high;
		}
		else
		{
			sda_low;
		}
		data <<= 1u;

		delay_us(AT24C64_delay);
		scl_high;
		delay_us(AT24C64_delay);
		scl_low;
		delay_us(AT24C64_delay);
	}

	/* 第 9 个时钟：释放 SDA，读取从机 ACK */
	sda_high;
	delay_us(AT24C64_delay);

	scl_high;
	delay_us(AT24C64_delay);

	// ack = sda_read();   /* 0 表示 ACK */
	ack = sda_state; /* 0 表示 ACK */

	scl_low;
	delay_us(AT24C64_delay);
	sda_high;
	delay_us(AT24C64_delay);

	return ack == 0u;
}

/* send_ack = true 表示主机发送 ACK；false 表示 NACK */
static uint8_t i2c_read_byte(bool send_ack)
{
	uint8_t data = 0;

	sda_high; /* 释放 SDA，由从机驱动 */

	for (uint8_t i = 0; i < 8u; i++)
	{
		delay_us(AT24C64_delay);
		scl_high;
		delay_us(AT24C64_delay);

		data <<= 1u;
		// if (sda_read())
		if (sda_state)
		{
			data |= 0x01u;
		}

		scl_low;
		delay_us(AT24C64_delay);
	}

	/* 第 9 个时钟：主机 ACK / NACK */
	if (send_ack)
	{
		sda_low;
	}
	else
	{
		sda_high;
	}
	delay_us(AT24C64_delay);

	scl_high;
	delay_us(AT24C64_delay);
	scl_low;
	delay_us(AT24C64_delay);

	sda_high;
	delay_us(AT24C64_delay);

	return data;
}

/* ================= AT24C64 操作 ================= */

/* 写周期 ACK 轮询：比固定 delay 更可靠 */
bool AT24C64_WaitReady(void)
{
	for (uint32_t retry = 0; retry < 1000u; retry++)
	{
		i2c_start();

		if (i2c_write_byte(AT24C64_DEV_WRITE))
		{
			i2c_stop();
			return true;
		}

		i2c_stop();
		delay_us(AT24C64_delay * 5);
	}

	return false;
}

/* 发起写操作并发送 2 字节字地址 */
static bool at24_start_write(uint16_t addr)
{
	i2c_start();

	if (!i2c_write_byte(AT24C64_DEV_WRITE))
	{
		i2c_stop();
		return false;
	}

	/* AT24C64 为 8KB，高字节只用低 5 位 */
	if (!i2c_write_byte((uint8_t)((addr >> 8) & 0x1Fu)))
	{
		i2c_stop();
		return false;
	}

	if (!i2c_write_byte((uint8_t)(addr & 0xFFu)))
	{
		i2c_stop();
		return false;
	}

	return true;
}

/* 页写：len <= 32，且不能跨页 */
bool AT24C64_PageWrite(uint16_t addr, const uint8_t *data, uint16_t len)
{
	if (len == 0u)
	{
		return true;
	}

	if (addr >= AT24C64_MEM_SIZE)
	{
		return false;
	}

	if (len > AT24C64_PAGE_SIZE)
	{
		return false;
	}

	if (((uint32_t)addr + len) > AT24C64_MEM_SIZE)
	{
		return false;
	}

	/* 不允许跨页，否则 EEPROM 会在页内回卷 */
	if (((addr & (AT24C64_PAGE_SIZE - 1u)) + len) > AT24C64_PAGE_SIZE)
	{
		return false;
	}

	if (!at24_start_write(addr))
	{
		return false;
	}

	for (uint16_t i = 0; i < len; i++)
	{
		if (!i2c_write_byte(data[i]))
		{
			i2c_stop();
			return false;
		}
	}

	i2c_stop();

	return AT24C64_WaitReady();
}

bool AT24C64_WriteByte(uint16_t addr, uint8_t data)
{
	return AT24C64_PageWrite(addr, &data, 1u);
}

/* 自动按页拆分，适合任意长度写入 */
/*  addr: 写入地址  data: 数据指针  len: 数据长度  */
bool AT24C64_WriteBuffer(uint16_t addr, const uint8_t *data, uint16_t len)
{
	if ((uint32_t)addr + len > AT24C64_MEM_SIZE)
	{
		return false;
	}

	while (len > 0u)
	{
		// 计算当前页剩余空间
		uint16_t page_left = AT24C64_PAGE_SIZE - (addr & (AT24C64_PAGE_SIZE - 1u));
		// 取当前页剩余空间和剩余数据长度的较小值作为写入长度
		uint16_t chunk = (len < page_left) ? len : page_left;

		if (!AT24C64_PageWrite(addr, data, chunk))
		{
			return false;
		}

		addr += chunk;
		data += chunk;
		len -= chunk;
	}

	return true;
}

/* 随机读，支持连续读；最后 1 字节发送 NACK */
bool AT24C64_ReadBuffer(uint16_t addr, uint8_t *data, uint16_t len)
{
	if (len == 0u)
	{
		return true;
	}

	if ((uint32_t)addr + len > AT24C64_MEM_SIZE)
	{
		return false;
	}

	i2c_start();

	if (!i2c_write_byte(AT24C64_DEV_WRITE))
	{
		i2c_stop();
		return false;
	}

	if (!i2c_write_byte((uint8_t)((addr >> 8) & 0x1Fu)))
	{
		i2c_stop();
		return false;
	}

	if (!i2c_write_byte((uint8_t)(addr & 0xFFu)))
	{
		i2c_stop();
		return false;
	}

	/* Repeated START */
	i2c_start();

	if (!i2c_write_byte(AT24C64_DEV_READ))
	{
		i2c_stop();
		return false;
	}

	for (uint16_t i = 0; i < len; i++)
	{
		bool ack = (i != (len - 1u)); /* 最后一个字节 NACK */
		data[i] = i2c_read_byte(ack);
	}

	i2c_stop();
	return true;
}

bool AT24C64_ReadByte(uint16_t addr, uint8_t *data)
{
	return AT24C64_ReadBuffer(addr, data, 1u);
}

/* 使用案例和注意事项
uint8_t wr_buf[5] = {0x11, 0x22, 0x33, 0x44, 0x55};
uint8_t rd_buf[5] = {0};

soft_i2c_init();

if (AT24C64_WriteBuffer(0x0000, wr_buf, sizeof(wr_buf))) {
   //  写入成功
}

if (AT24C64_ReadBuffer(0x0000, rd_buf, sizeof(rd_buf))) {
   // rd_buf 理论上应等于 wr_buf
}

必须使用开漏输出
SCL/SDA 配置为 GPIO_MODE_OUTPUT_OD 或 GPIO_Mode_Out_OD。如果配成推挽输出，总线可能冲突。

外部上拉电阻必须接
建议 SCL、SDA 各接 4.7?kΩ
4.7kΩ 到 3.3V。

AT24C64 页大小为 32 字节
页写不能跨页；跨页时 EEPROM 会在当前页内回卷，导致数据覆盖。上面的 AT24C64_WriteBuffer() 已自动按页拆分。

AT24C64 使用 2 字节字地址
高字节只使用低 5 位，因为容量为 8KB。

写周期用 ACK 轮询更可靠
AT24C64 写周期最大约 5ms，AT24C64_WaitReady() 通过 ACK 轮询等待完成，比固定延时更稳。

读操作使用 Repeated START
先写设备地址和字地址，再重复起始，再发读地址，最后读数据；最后 1 字节必须 NACK，然后 STOP。

调试建议
用逻辑分析仪观察 START、ACK、STOP 和 SCL 频率。I2C 时序问题很容易卡在 ACK 和页写跨页上

*/
#if 0
u16 R_Flash_i;

u8 R_Flash_WB;
const FLASH_Power_typedef FLASH_Power_down_save __attribute__((section(".ARM.__at_0x800F000"))) = {0};

#define d_FLASH_Date_maxbyte 32 // 字节
#define d_FLASH_Date_maxword 8	// 字

u8 R_0xA5 = 0xA5;
u8 R_0xFF = 0xFF;
u8 R_0x5A = 0x5A;
u8 *const FLASH_Date[] = {
	(u8 *)&R_User_Watt_Set,
	(u8 *)((u8 *)&R_User_Watt_Set + 1),
	(u8 *)&R_User_Vout_Set,
	(u8 *)((u8 *)&R_User_Vout_Set + 1),

	(u8 *)&R_User_Temp_Set_C,
	(u8 *)((u8 *)&R_User_Temp_Set_C + 1),
	(u8 *)&R_User_Temp_Set_F,
	(u8 *)((u8 *)&R_User_Temp_Set_F + 1),

	(u8 *)&Mode_Color_index,
	(u8 *)&R_Statistics_number,
	(u8 *)((u8 *)&R_Statistics_number + 1),
	(u8 *)((u8 *)&R_Statistics_number + 2),

	(u8 *)((u8 *)&R_Statistics_number + 3),
	(u8 *)((u8 *)&R_Statistics_number + 4),
	(u8 *)&R_Temp_metric,
	(u8 *)&R_Atomizer,

	(u8 *)&Load_Ss316.WattsMAX,
	(u8 *)((u8 *)&Load_Ss316.WattsMAX + 1),
	(u8 *)&Load_Ni200.WattsMAX,
	(u8 *)((u8 *)&Load_Ni200.WattsMAX + 1),
	(u8 *)&Load_Ti.WattsMAX,

	(u8 *)((u8 *)&Load_Ti.WattsMAX + 1),
	(u8 *)&Load_M1.Gradient,
	(u8 *)((u8 *)&Load_M1.Gradient + 1),
	(u8 *)&Load_M1.WattsMAX,

	(u8 *)((u8 *)&Load_M1.WattsMAX + 1),
	(u8 *)&R_Work_mode,
	(u8 *)&B_ADDSUB_LOCK,
	(u8 *)&R_Res_Initial_jiyi,

	(u8 *)((u8 *)&R_Res_Initial_jiyi + 1),
	(u8 *)&R_0xA5,
	(u8 *)&R_0xA5,
	(u8 *)&R_0xA5,

};

void Rflash(void)
{
	u16 i;

	R_Flash_i = 0;
	for (i = 0; i < 1024; i += d_FLASH_Date_maxbyte) // 查找这2K数据里有没有已经初始化的
	{
		if (((FLASH_Power_typedef *)FLASH_Power_down_save_ADDR)->R_Flash16[(i + d_FLASH_Date_maxbyte) / 2 - 1] == 0xA5A5)
		{
			R_Flash_i = i;
			break;
		}
	}

	if (((FLASH_Power_typedef *)FLASH_Power_down_save_ADDR)->R_Flash16[d_FLASH_Date_maxbyte / 2 - 1] != 0xA5A5 && R_Flash_i == 0)
	{
		__set_PRIMASK(1); /* 关中断 */
		flash_unlock();
		__FLASH_FLAG_CLEAR(FLASH_FLAG_ENDF | FLASH_FLAG_PGERR | FLASH_FLAG_WPERR);
		flash_page_erase((uint32_t)(FLASH_Power_down_save_ADDR));
		flash_lock();
		__set_PRIMASK(0); /* 开中断 */
	}
	else // 有已经初始化的
	{
		for (i = 0; i < d_FLASH_Date_maxbyte; i++)
		{
			*FLASH_Date[i] = FLASH_Power_down_save.R_Flash8[R_Flash_i + i]; // 读取flash的值(读取一个结构体的大小40个byte)
		}
		// 设定瓦数

		if (R_User_Watt_Set > d_User_Watt_max || R_User_Watt_Set < d_User_Watt_min)
			R_User_Watt_Set = d_User_Watt_min;

		if (R_User_Vout_Set > d_User_Vout_max || R_User_Vout_Set < d_User_Vout_min)
			R_User_Vout_Set = d_User_Vout_min;

		if (R_User_Vout_Set > d_User_Vout_max || R_User_Vout_Set < d_User_Vout_min)
			R_User_Vout_Set = d_User_Vout_min;

		if (R_User_Temp_Set_C > d_User_TempC_max || R_User_Temp_Set_C < d_User_TempC_min)
			R_User_Temp_Set_C = d_User_TempC_min;

		if (R_User_Temp_Set_F > d_User_TempF_max || R_User_Temp_Set_F < d_User_TempF_min)
			R_User_Temp_Set_F = d_User_TempF_min;

		R_TCR_term = M1;

		if (R_Atomizer >= Atomizer_NC)
			R_Atomizer = Ss;

		if (R_Temp_metric >= Temp_metric_NC)
			R_Temp_metric = F_metric;

		if (R_Work_mode >= d_NOP_mode)
			R_Work_mode = d_Watt_mode;

		if (B_ADDSUB_LOCK > 1)
			B_ADDSUB_LOCK = 0;

		if (Mode_Color_index > 2)
		{
			Mode_Color_index = 0;
		}
		if (Load_Ni200.WattsMAX > d_User_Watt_max / 10 || Load_Ni200.WattsMAX < d_User_Watt_min / 10)
			Load_Ni200.WattsMAX = 10;
		if (Load_Ti.WattsMAX > d_User_Watt_max / 10 || Load_Ti.WattsMAX < d_User_Watt_min / 10)
			Load_Ti.WattsMAX = 10;
		if (Load_Ss316.WattsMAX > d_User_Watt_max / 10 || Load_Ss316.WattsMAX < d_User_Watt_min / 10)
			Load_Ss316.WattsMAX = 10;
		if (Load_M1.WattsMAX > d_User_Watt_max / 10 || Load_M1.WattsMAX < d_User_Watt_min / 10)
			Load_M1.WattsMAX = 10;

		if (Load_M1.Gradient > 1000 || Load_M1.Gradient < 100)
			Load_M1.Gradient = 100;

		if (R_Res_Initial_jiyi > d_Res_usable)
			R_Res_Initial_jiyi = 0;

		if (R_Statistics_number > d_Statistics_number_MAX)
		{
			R_Statistics_number = 0;
		}
	}

	/////////////////////////////////////////////////////////////////////////
}

void Data_retention(void)
{
	u8 i;
	u8 j;
	volatile u32 Addr0 = 0;
	volatile u32 Addr1 = 0;
	uint16_t buff[1] = {0};
	if (!B_Out_Yes && !key_value) // 正在输出标志(没有输出) //当前按键值寄存器(没有按键按下)
	{

		if (!R_Flash_WB)
		{
			R_Flash_WB = 50;

			for (i = 0; i < d_FLASH_Date_maxbyte; i++)
			{
				if (*FLASH_Date[i] != ((FLASH_Power_typedef *)FLASH_Power_down_save_ADDR)->R_Flash8[R_Flash_i + i])
				{
					__set_PRIMASK(1); /* 关中断 */
					flash_unlock();
					__FLASH_FLAG_CLEAR(FLASH_FLAG_ENDF | FLASH_FLAG_PGERR | FLASH_FLAG_WPERR);
					flash_half_word_program((uint32_t)(&(((FLASH_Power_typedef *)FLASH_Power_down_save_ADDR)->R_Flash16[(R_Flash_i + d_FLASH_Date_maxbyte) / 2 - 1])), buff[0]);
					R_Flash_i += d_FLASH_Date_maxbyte; // 每次写完R_Flash_i的值不能变，所以得放在前面
					if (R_Flash_i + d_FLASH_Date_maxbyte > 1024)
					{
						R_Flash_i = 0;

						flash_page_erase((uint32_t)(FLASH_Power_down_save_ADDR));
					}

					for (i = 0; i < d_FLASH_Date_maxword; i++)
					{
						j = i * 4;
						Addr0 = (uint32_t)(&(FLASH_Power_down_save.R_Flash32[R_Flash_i / 4 + i]));
						if (Addr0 >= FLASH_Power_down_save_ADDR && Addr0 <= (FLASH_Power_down_save_ADDR + 0x400))
							flash_word_program(Addr0, *FLASH_Date[j + 3] << 24 | *FLASH_Date[j + 2] << 16 | *FLASH_Date[j + 1] << 8 | *FLASH_Date[j]);
					}
					flash_lock();
					__set_PRIMASK(0); /* 开中断 */
					break;
				}
			}

			/////////////////////////////////////
		}
	}
	else
	{
		R_Flash_WB = 50;
	}
}
#endif
