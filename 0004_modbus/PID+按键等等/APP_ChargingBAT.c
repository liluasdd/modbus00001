#include "main.h"
#include "main_2.h"
#include "APP_ChargingBAT.h"
#include "WM_Disp.h"

u8 B_Charg_Jianche;
u8 B_BAT_Jianche;
u8 B_BAT_error = 1; // 电池异常标志
					// 电池异常标志
u16 B_BAT_error_LB_i = 0;
u8 B_BAT_OK_LB_i = 0;

u8 R_USB_error_i;
u8 R_USB_error_Tim;

u8 B_Proportion_Init = 0;

u16 Decide_L = 0;
u16 Full_count = 0;
u8 Full_Cnt = 0;

u8 B_BAT_Congman;  /// 第一节充电充满标志
u8 B_BAT_Congman2; /// 第二节充电充满标志
u8 B_BAT_Charge;
u8 B_BAT_OutCharge = 0;

u16 Last_Out_Bat_Power;
u16 R_BATT_Actual;				// 总电池电压
u16 R_BATT_L_Actual = 0;		// 低端电池电压
u16 R_BATT_H_Actual;			// 高端电池电压
u16 R_Charging_current_average; // 平均充电电流

u16 R_BAT_L_Real;			 // 实时值
u16 R_BAT_H_Real;			 // 实时值
u16 R_BAT_Real;				 // 实时值
u16 R_BAT_MAX_Real;			 // 实时值
u16 R_BATZ_Real;			 // 实时值
u16 R_Charging_current_Real; // 实时充电电流
u16 R_Batter_Interface_Tim;	 // 屏幕充电计时

static u8 R_USB_L = 0;

u8 B_usbOnOff;
u16 R_VCC_Voltage;
u8 OUT_12V_ONOFF = 0;
u8 B_Chongman_JC = 0;
u16 chargeTemp = 0;
u16 batTemp = 0;
uc16 R_BAT_VIP[] = {d_Bat_Volt_min, 365, 371, 383, 397, d_Bat_Volt_max - 5};

// uc16	R_BAT_VIP[]={d_Bat_Volt_min,357,362,367,372,379,385,390,398,405,d_Bat_Volt_max-2};
//  uc16	R_BAT_VIP[]={d_Bat_Volt_min,357,362,370,376,384,389,395,400,406,d_Bat_Volt_max-2};
static void BATT_Charging_Count_down(void);
void BatterGrade_Calculate(void);

// USB检测口设置为数字口
USB_GPIO_term R_USB_GPIO = USB_IN;
void USB_GPIO_Set(USB_GPIO_term R)
{
	if (R != R_USB_GPIO)
	{
		R_USB_GPIO = R;
		if (R_USB_GPIO == USB_IN)
		{
			gpio_mode_set(SEN_USB_T_GPIO_Port, SEN_USB_T_Pin, GPIO_MODE_IN_FLOAT);
		}
		else
		{
			gpio_mode_set(SEN_USB_T_GPIO_Port, SEN_USB_T_Pin, GPIO_MODE_ANALOG);
		}
	}
}

void CHG_ENABLE(void)
{
	// IP5355_IIC_Write(0x90, 0x22); // 关闭手机充电
	// IP5355_IIC_Write(0x52, 0x22);

	//	if (R_BAT_Real < 320) // 电池电压低于320mv，不开启充电充电  R_BATT_Actual总电池
	//	{
	//		IP5355_IIC_Write(0x90, 0x22); // 关闭手机充电
	//		IP5355_IIC_Write(0x52, 0x22);
	//	}
	//	else
	//	{
	//		IP5355_IIC_Write(0x80, 0x22); // 开启手机充电
	//		IP5355_IIC_Write(0x42, 0x22);
	//	}
	// PG_STAT_TEMP=0;
	//	  Check_usb_time=5;
	//	  USB_first_IN=1;
	// CX25890H_enable_charger();
	//      gpio_mode_set(EN_CHG_GPIO_Port, EN_CHG_Pin, GPIO_MODE_IN_FLOAT);
	B_usbOnOff = 1;
}
void CHG_DISABLE(void)
{
	//	if (R_BAT_Real < 320)
	//	{
	// IP5355_IIC_Write(0x90, 0x22); // 关闭手机充电
	// IP5355_IIC_Write(0x51, 0x22);
	//	}
	//	else
	//	{
	//		IP5355_IIC_Write(0x80, 0x22); // 开启手机充电
	//		IP5355_IIC_Write(0x41, 0x22);
	//	}
	//	update_07_reg(0);
	//	CX25890H_SET_Vds_Bat(false);
	// CX25890H_disable_charger(B);
	//	gpio_mode_set(EN_CHG_GPIO_Port,EN_CHG_Pin,GPIO_MODE_OUT_PP(GPIO_SPEED_HIGH));
	//    EN_CHG_GPIO_Port->DO&=~EN_CHG_Pin;
	B_usbOnOff = 0;
}

// 统计平均电压电流
static u16 R_BAT_sum_i = 0;
static u32 R_BAT_sum_bat = 0;
static u32 R_BAT_sum_L_bat = 0;
static u32 R_BAT_sum_H_bat = 0;

void F_BAT_Actual_Init(void)
{
	R_BAT_sum_i = 0;
	R_BAT_sum_bat = 0;
	R_BAT_sum_L_bat = 0;
	R_BAT_sum_H_bat = 0;
}
void F_BAT_Actual(void)
{
	R_BAT_sum_i++;
	R_BAT_sum_bat += R_BAT_Real;
	R_BAT_sum_L_bat += R_BAT_L_Real;
	R_BAT_sum_H_bat += R_BAT_H_Real;
	if (R_BAT_sum_i > 4)
	{
		R_BATT_Actual = R_BAT_sum_bat / R_BAT_sum_i;
		R_BATT_L_Actual = R_BAT_sum_L_bat / R_BAT_sum_i;
		R_BATT_H_Actual = R_BAT_sum_H_bat / R_BAT_sum_i;
		F_BAT_Actual_Init();
	}
}

void Decide_BAT_Congman(void)
{

	if (R_BATT_Actual >= 418) // 4分钟倒计时 250*4*60*2
	{
		Decide_L = 0;
		if (Full_count++ > 480 * 3)
		{
			B_BAT_Congman = 1;
		}
	}
	else if (R_BATT_Actual > 413 && R_BATT_Actual < 418)
	{
		Full_count = 0;
		if (Decide_L < 960 * 3) // 8分钟倒计时   //250*4*60*4
		{
			Decide_L++;
		}
		else
		{
			if (!B_BAT_Congman) // 更新充满标志
			{
				B_BAT_Congman = 1;
			}
		}
	}
	else
	{
		Full_count = 0;
		Decide_L = 0;
	}
}

u16 R_USB_ADC_data;
u16 R_VCC_Voltage;
u16 R_BAT_Real0_Q = 0;
u8 full_flag = 0;
u16 Check_bat_time = 0;

//////////电池管理/////////
void BAT_Management(void)
{
	static u8 R8_BAT_Charging420_num = 0;

	if (B_BAT_Charge)
	{
		if ((R_BAT_Real < 300 && !B_Out_Yes && B_OLED_BL && R_Screen_brightness_NOW == 0) || B_BAT_error)
		{
			BOOST_12V_DISABLE();
		}
		else if (R_Bright_screen_Time)
		{
			if (!OUT_12V_ONOFF)
				BOOST_12V_ENABLE();
		}
	}
	else
	{
		if (!OUT_12V_ONOFF)
			BOOST_12V_ENABLE();
	}

	if (B_BAT_error)
	{
		if (B_Charg_Jianche)
		{
			B_Charg_Jianche = 0;
			u16 R_BAT_Real0 = ReadBATz_Data();
			if (R_BAT_Real0 > R_BAT_Real0_Q)
			{
				R_BAT_Real0_Q = R_BAT_Real0 - R_BAT_Real0_Q;
			}
			else
			{
				R_BAT_Real0_Q = R_BAT_Real0_Q - R_BAT_Real0;
			}

			if ((R_BAT_Real0 > 250 || (B_USB_RinBit && R_BAT_Real0 > 250)) && R_BAT_MAX_Real < 450 && R_BAT_Real0_Q < 10)
			{
				if ((!First_Power_On && B_BAT_error_LB_i < 1000) || (First_Power_On && B_BAT_error_LB_i < 500))
					B_BAT_error_LB_i++;
				else
				{
					B_BAT_error_LB_i = 0;
					B_BAT_error = 0;
					B_Proportion_Init = 0;
					F_BAT_Actual_Init();
				}
			}
			else
			{
				B_BAT_error_LB_i = 0;
			}
			R_BAT_Real0_Q = R_BAT_Real0;
		}
	}
	////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////
	if (B_BAT_Jianche) // 500ms
	{
		B_BAT_Jianche = 0;

		R_BAT_Real = ReadBATz_Data();
		////////////////////////////////////////////////////////
		F_BAT_Actual();
		if (R_BAT_Real > R_BATT_Actual + 5 || R_BAT_Real + 5 < R_BATT_Actual) // 相差直接赋值
		{
			R_BATT_Actual = R_BAT_Real;
			R_BATT_L_Actual = R_BAT_Real; // R_BAT_L_Real;
			F_BAT_Actual_Init();
		}
		if (!B_BAT_error && !R_XY_Tim_yanshi)
		{
			Batt_Proportion();
			// BatterGrade_Calculate();
		}
		if (B_BAT_Charge)
		{
			IP5355_IIC_Read(0x09); // 读取充满标志位
			Read_Data = Read_Data & 0x08;
			if (Read_Data != 0x08)
				IP5355_IIC_Write(0x08, 0x09); // 开启充满中断
			IP5355_IIC_Read(0x0d);
			Read_Data = Read_Data & 0x01;
			if (Read_Data != 0x01)
				IP5355_IIC_Write(0x01, 0x0d); // 充满使能
			if (R_BattL_Proportion >= 50)
			{
				Read_Data = IP5355_IIC_Read(0x08); // 充满判断
				if (Read_Data != 0xff)
					Read_Data = Read_Data & 0x01;
				else
					Read_Data = 0;
				if (Read_Data == 0x01) // 充满
				{
					R8_BAT_Charging420_num++;
					if (R8_BAT_Charging420_num > 6) // 3秒
					{
						IP5355_IIC_Write(0x03, 0x08);
						R8_BAT_Charging420_num = 0;
					}
				}
				// else
				// {
				// 	R_SleepTim = 20;//没有充满，偷偷充电，不进入休眠
				// }
			}
		}
		/* 充电完成检测条件判断
		 * 条件包括：
		 * - Read_Data == 0x01 : 充电管理芯片返回特定状态值
		 * - B_usbOnOff : USB连接状态
		 * - R_BATT_Actual > 410 : 电池电压大于410单位（表示接近充满）
		 * - !B_Out_Yes : 非输出状态
		 * - !B_BAT_Congman : 未标记为充电完成
		 * - B_BAT_Charge : 正在充电状态
		 * - !B_USB_Low : USB电压非低压状态
		 */
		if ((Read_Data == 0x01) && B_usbOnOff && R_BATT_Actual > 410 && !B_Out_Yes && !B_BAT_Congman && B_BAT_Charge && !B_USB_Low)
		{
			if (Full_Cnt < 2)
			{
				Full_Cnt++; // 充电完成计数器增加
			}
			else
			{
				Full_Cnt = 0;	   // 重置计数器
				B_BAT_Congman = 1; // 标记电池已充满
			}
		}
		else
		{
			Full_Cnt = 0; // 不满足充电完成条件时重置计数器
			/* 在某些条件下重新开启充电
			 * - !B_Out_Yes : 非输出状态
			 * - B_BAT_Charge : 处于充电模式
			 * - !B_usbOnOff : USB未连接（可能是指需要充电但未连接）
			 * - !B_USB_Low : USB电压非低压
			 */
			if ((!B_Out_Yes) && B_BAT_Charge && (!B_usbOnOff) && (!B_USB_Low))
			{
				CHG_ENABLE(); // 开启充电
			}
		}

		/////////////////////////////////////
		//			  if(USB_first_IN)
		//			  {
		//				  if(!Check_usb_time)
		//				  {
		//					  USB_first_IN=0;
		//					  update_07_reg(1);
		//					  CX25890H_SET_Vds_Bat(true);
		//				  }
		//			  }
		//////////////////////////////////////////////////////
		//			  if(PG_STAT_TEMP!=2)
		//			 {
		//				if((CX25890H_Read_Byte(CX25890H_REG_11)&0x80)==0x80)//适配器插入
		//				{
		//					 if(PG_STAT_TEMP==0)
		//					 {
		//							CX25890H_SET_Vds_Bat(false);
		//							PG_STAT_TEMP=1;
		//					 }
		//					else
		//					 {
		//							PG_STAT_TEMP=2;
		//							CX25890H_SET_Vds_Bat(true);
		//					 }
		//				}
		//		    }
		// if(B_usbOnOff&&B_BAT_Charge&&!B_USB_Low&&!B_Out_Yes&&!R_XY_Tim_yanshi)
		// {
		//    if(!Check_bat_time)
		//   {
		// 	Check_bat_time=300;
		// 	  full_flag=CX25890H_Get_Full();
		// 	  if(!full_flag)
		// 	  {
		// 		  CHG_DISABLE(1); //关充电,不限流
		// 		  Delay_Nus(200);
		// 		  u16 bat_Real=ReadBATz_Data();
		// 		  if(R_BAT_Real>bat_Real)
		// 		  {
		// 			Bat_V_diff_data=R_BAT_Real-bat_Real; //更新差压差值
		// 			if(Bat_V_diff_data>10) Bat_V_diff_data=10;
		// 		  }
		// 		  CHG_ENABLE();        //开启充电
		// 	  }
		// 	  else
		// 	  {
		// 		  Bat_V_diff_data=0;
		// 	  }
		// 	}
		// }

		if (R_BAT_Real < 220 || R_BAT_MAX_Real > 440 /*|| R_VCC_Voltage<310 || R_VCC_Voltage>350*/)
		{

			if (B_BAT_OK_LB_i < 4)
				B_BAT_OK_LB_i++;
			else
			{
				B_Proportion_Init = 0;
				B_BAT_error = 1;
				BOOST_12V_DISABLE();
				B_BAT_OK_LB_i = 0;
			}
		}
		else
		{
			B_BAT_OK_LB_i = 0;
		}

		// 回充判断
		if (R_BATT_Actual < 410)
		{
			if (B_BAT_Congman && !R_XY_Tim_yanshi)
			{
				if (B_BAT_Charge)
				{
					CHG_DISABLE();
					Delay_Nus(100);
					CHG_ENABLE();
				}
				B_BAT_Congman = 0;
				Decide_L = 0;
				Full_count = 0;
				Full_Cnt = 0;
				if (!B_OLED_BL) // 回充直接更新
				{
					B_Proportion_Init = 0;
					Batt_Proportion();
				}
			}
		}

		// 恢复充电
		if (!B_BAT_Charge) // 电池恢复正常让你逃出来
		{
			USB_OVP = Read_USB_OVP_Data(); // 读取usb输入电压
			if (USB_OVP >= 450 && USB_OVP <= d_USB_error_H - 20)
			{
				if (B_USB_Low)
				{
					// USB_GPIO_Set(USB_IN);
					IP6106_Init();
					//					IP5355_IIC_Write(0x80, 0x22); // 开启手机充电
					//					IP5355_IIC_Write(0x40, 0x22);
					B_USB_Low = 0;
					R_USB_L = 0;
					B_USB_RinBit = 0;
					First_In_USB = 1;
				}
			}
		}

		if (B_BAT_Charge || B_USB_Low || B_BAT_OutCharge)
		{
			// 坏电池充满判断
			if (B_BAT_Charge)
			{
				//////////////////////温度调节///////////////////////////////////////
				Decide_BAT_Congman();
			}
			USB_OVP = Read_USB_OVP_Data();
			if (USB_OVP < d_USB_error_L2) // 250
			// if(!READ_USB_STATE)
			{
				// if (B_BAT_Charge)	//充电时
				// {
				B_BAT_OutCharge = 0;
				// }
				// if (R_BattL_Proportion == 0 && B_BAT_OutCharge) // 放电状态且电量为0时
				// {
				// 	B_BAT_OutCharge = 0;
				// 	IP5355_IIC_Write(0x90, 0x22);
				// 	IP5355_IIC_Write(0x52, 0x22); // 关放电开充电
				// }

				B_BAT_Charge = 0;
				if (R_SleepTim < 5)
					R_SleepTim = 5;
				B_USB_Low = 0;
				First_In_USB = 0;
				R_Bright_screen_TimeMAX = 100;
				R_Bright_screen_Time = R_Bright_screen_TimeMAX;

				// USB_GPIO_Set(USB_IN);
				R_USB_L = 0;
				B_USB_RinBit = 0;
				CHG_DISABLE(); // 关闭充电
				if (B_System_off == d_System_Off)
				{
					if (Disp_windows != System_off_Charge_interface && Disp_windows != System_off_interface)
					{
						Windows_Switch(System_off_Charge_interface);
					}
				}
			}
			else if (((USB_OVP < d_USB_error_L || USB_OVP > d_USB_error_H)) && (B_BAT_Charge))
			{
				if (!B_USB_Low && !B_Out_Yes)
				{
					R_USB_L++;
					if (R_USB_L > 6)
					{
						R_USB_L = 0;
						if (B_usbOnOff && USB_OVP < 420)
						{
							CHG_DISABLE();
							Delay_Nms(10);
							USB_OVP = Read_USB_OVP_Data();
							if (USB_OVP > 450)
							{

								CHG_ENABLE();
								return;
							}
						}
						B_USB_Low = 1;
						R_USB_error_i++;
						R_USB_error_Tim = 35;
						B_BAT_Charge = 0;
						if (R_SleepTim < 5)
							R_SleepTim = 5;
						CHG_DISABLE();
						R_Skinning_mode = USB_anomaly;
					}
				}
			}
			else
			{
				R_USB_L = 0;
			}
		}
	}
	if (B_Chongman_JC)
	{
		BATT_Charging_Count_down(); /// 充电倒计时
		B_Chongman_JC = 0;
	}
}

////////////////////////////统计电池电量百分比
u8 R_BattL_Proportion; // 低节电池百分比
u8 R_BattH_Proportion; // 高节电池百分比

u16 Bat_Volt_Data = 0;
u8 output_2s_mark = 0;
void Batt_Proportion(void)
{
	u8 i;
	u8 a;
	static s16 LB[2] = {100, 100};
	static u8 count_charge_finish[2] = {20, 20};
	static u8 R_BattL_Proportion_Q = 0xFF;
	// static	u16	BatterGrade_Q=0xFF;

	Bat_Volt_Data = R_BATT_L_Actual;
	if (Bat_Volt_Data > d_Bat_Volt_min && B_BAT_Charge)
	{
		Bat_Volt_Data -= Bat_V_diff_data;
	}

	if (B_BAT_Congman) // 充满了直接是百分百,倒计时为0
	{
		if (B_BAT_Charge)
		{
			if ((R_BattL_Proportion < 100) || (Charging_Over_Time[0]))
			{
				if (R_BattL_Proportion < 80)
				{
					R_BattL_Proportion = 100;
					Charging_Over_Time[0] = 0;
				}
				else
				{
					if (count_charge_finish[0] < 40) // 15S加1
						count_charge_finish[0]++;
					else
					{
						count_charge_finish[0] = 0;
						if (R_BattL_Proportion < 100)
							R_BattL_Proportion++;
						if (Charging_Over_Time[0])
							Charging_Over_Time[0]--;
					}
				}
			}
		}
		else
		{
			if (R_BattL_Proportion_Q > R_BattL_Proportion)
			{
				R_BattL_Proportion = 100;
				Charging_Over_Time[0] = 0;
			}
		}
	}
	else
	{
		//		if(R_BattL_Proportion==100)
		//		{
		//			if(B_BAT_Charge)
		//			{
		//				R_BattL_Proportion=99;
		//				Charging_Over_Time[0]=2;
		//			}
		//			else
		//			{
		//				R_BattL_Proportion=100;
		//			}

		//		}
		if (Bat_Volt_Data < d_Bat_Volt_min)
		{
			a = 0;
			LB[0] = 0;
		}
		else if (Bat_Volt_Data >= R_BAT_VIP[5])
		{
			if (!B_BAT_Charge)
				a = 100; // 99
			else
				a = 99;
		}
		else
		{
			for (i = 1; i < 6; i++)
			{
				if (Bat_Volt_Data < R_BAT_VIP[i])
				{
					break;
				}
			}
			a = (Bat_Volt_Data - R_BAT_VIP[i - 1]) * 20 / (R_BAT_VIP[i] - R_BAT_VIP[i - 1]);
			a += (i - 1) * 20;
		}
		// 充电状态不减
		if (B_BAT_Charge)
		{
			output_2s_mark = 0;
		}

		if (!B_Proportion_Init)
		{
			R_BattL_Proportion = a;
		}
		if (a + 15 < R_BattL_Proportion) // 500S减1
		{
			LB[0] = 0;
		}
		else if (a + 10 < R_BattL_Proportion) // 10S减1
		{
			LB[0] -= 5;
			if (output_2s_mark && !B_BAT_Charge) // 有输出直接更新一次
			{
				LB[0] = 0;
				output_2s_mark = 0;
			}
		}
		else if (a + 5 < R_BattL_Proportion) // 30S减1
		{
			LB[0] -= 2;
			if (output_2s_mark && !B_BAT_Charge) // 有输出直接更新一次
			{
				LB[0] = 0;
				output_2s_mark = 0;
			}
		}
		else if (a + 3 < R_BattL_Proportion) // 1分钟减1
		{
			LB[0] -= 1;
			if (output_2s_mark && !B_BAT_Charge) // 有输出直接更新一次
			{
				LB[0] = 0;
				output_2s_mark = 0;
			}
		}
		else if (a > R_BattL_Proportion + 10) // 6S加1
		{
			LB[0] += 10;
		}
		else if (a > R_BattL_Proportion + 5) // 30S加1
		{
			LB[0] += 2;
		}
		else if (a > R_BattL_Proportion) // 1分钟加1
		{
			LB[0] += 1;
		}
		else if (a)
		{
			LB[0] = 100;
		}
		if (LB[0] <= 0)
		{
			LB[0] = 100;
			if (R_BattL_Proportion)
				R_BattL_Proportion--;
		}
		else if (LB[0] >= 200 && B_BAT_Charge)
		{
			LB[0] = 100;
			if (R_BattL_Proportion < 100) // 99
				R_BattL_Proportion++;
		}
	}
	//    if(BatterGrade&&!R_BattL_Proportion)  //防止百分比为0还有一格电
	//    {
	//        R_BattL_Proportion=1;
	//    }
	// 变化大直接更新
	B_Proportion_Init = 1;
	if (/*(R_BattL_Proportion+30<a&&B_BAT_Charge)||*/ R_BattL_Proportion > a + 30)
	{
		B_Proportion_Init = 0;
	}

	if (R_BattL_Proportion_Q != R_BattL_Proportion)
	{
		if (R_BattL_Proportion >= 81)
		{
			BatterGrade = 5;
		}
		else if (R_BattL_Proportion >= 61)
		{
			BatterGrade = 4;
		}
		else if (R_BattL_Proportion >= 41)
		{
			BatterGrade = 3;
		}
		else if (R_BattL_Proportion >= 21)
		{
			BatterGrade = 2;
		}
		else if (R_BattL_Proportion > 0)
		{
			BatterGrade = 1;
		}
		else
		{
			BatterGrade = 0;
		}
		//    if(R_BattL_Proportion>=90)
		//        BatterGrade=10;
		// 	else if(R_BattL_Proportion>=80)
		//        BatterGrade=9;
		// 	else if(R_BattL_Proportion>=70)
		//        BatterGrade=8;
		//     else  if(R_BattL_Proportion>=60)
		//        BatterGrade=7;
		// 	else if(R_BattL_Proportion>=50)
		//        BatterGrade=6;
		// 	else if(R_BattL_Proportion>=40)
		//        BatterGrade=5;
		// 	else if(R_BattL_Proportion>=30)
		//        BatterGrade=4;
		//     else if(R_BattL_Proportion>=20)
		//         BatterGrade=3;
		//    else if(R_BattL_Proportion>=10)
		//        BatterGrade=2;
		//    else if(R_BattL_Proportion>0)
		//       BatterGrade=1;
		//    else
		//       BatterGrade=0;
	}
	R_BattL_Proportion_Q = R_BattL_Proportion;

	/////////////////////
}

// 充电倒计时
u8 Charging_Count; // 吸烟倒计时计数器

u16 Charging_Over_Time[2];

// 充电时间倒计时显示
u16 BATT_Charging_Count_down_JS(u8 CM, u8 Bat, u16 R)
{
	u16 a;
	if (CM && !Bat) //////充满才给标志就好了
		return 0;
	else
	{
		if (Bat <= 0)
		{
			a = 120;
		}

		else if (Bat > 0 && Bat <= 20) // 分三个电压阶段 1.8到2A阶段  12分钟
		{
			a = 120 - Bat * 24 / 20;
		}
		else if (Bat > 20 && Bat <= 40) // 分四个电压阶段 1.8到2A阶段  12分钟
		{
			a = 96 - (Bat - 20) * 24 / 20;
		}
		else if (Bat > 40 && Bat <= 60) // 分五个电压阶段 1.8到2A阶段  12分钟
		{
			a = 72 - (Bat - 40) * 24 / 20;
		}
		else if (Bat > 60 && Bat <= 80) // 分六个电压阶段 1.8到2A阶段
		{
			a = 48 - (Bat - 60) * 24 / 20;
		}
		else if (Bat > 80 && Bat <= 100) // 分七个电压阶段 1.8到2A阶段  12分钟
		{
			a = 24 - (Bat - 80) * 24 / 20;
		}
		if (a <= 0)
			return 0;
		else if (a > 120)
			return 120;
		else
			return a;
	}
}

void BATT_Charging_Count_down_Init(void)
{
	Charging_Count = 20;
	Charging_Over_Time[0] = BATT_Charging_Count_down_JS(B_BAT_Congman, R_BattL_Proportion, 100);
}

static void BATT_Charging_Count_down(void)
{
	u16 BATT_Current;
	u8 t1;
	if (Charging_Count)
	{
		Charging_Count--;
	}
	else
	{
		Charging_Count = 50;
		t1 = BATT_Charging_Count_down_JS(B_BAT_Congman, R_BattL_Proportion, BATT_Current);
		if (t1 >= Charging_Over_Time[0] || t1 < Charging_Over_Time[0] || t1 == 0 || t1 == 3)
		{
			Charging_Over_Time[0] = t1;
		}
	}
}

// 计算电池格数
u8 BatterGrade;
u8 BatterGrade2;
// void	BatterGrade_Calculate(void)
//{
//	u8	i;
//	static	u16	Batter_Vatt_Q=0;
//	static	u16	BatterGrade_Q=0xFF;
//	if(Batter_Vatt_Q!=R_BATT_L_Actual)
//	{
//		Batter_Vatt_Q=R_BATT_L_Actual;
//		for(i=0;i<5;i++)
//		{
//			if(Batter_Vatt_Q<R_BAT_VIP[i])
//			{
//				break;
//			}
//		}
//		BatterGrade=i;
//		if(BatterGrade+1==BatterGrade_Q && R_BATT_L_Actual+3>R_BAT_VIP[i])
//			BatterGrade=BatterGrade_Q;
//		else if(BatterGrade==BatterGrade_Q+1 && R_BATT_L_Actual<R_BAT_VIP[i-1]+3&&B_BAT_Charge)
//			BatterGrade=BatterGrade_Q;
//
//
//		if(!B_BAT_Charge)
//		{
//			if(BatterGrade>BatterGrade_Q&&BatterGrade_Q!=0xFF)
//				BatterGrade=BatterGrade_Q;
//		}
//
//		BatterGrade_Q=BatterGrade;
//

//	}

//}
