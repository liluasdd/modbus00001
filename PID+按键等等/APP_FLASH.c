#include "main.h"
#include "APP_FLASH.h"
#include "main_2.h"

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
