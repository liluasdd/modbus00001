#include "main.h"
#include "main_2.h"
// #include "APP_KEY.h"
#include "WM_Disp.h"

// #define	d_KEY_TOUCH

u8 B_KEY_JC;
u8 R_KEY_timMax;
vu8 R_KEY_timi;

/*按键过滤*/
u8 Key_LB_value;			// 当前按键状态
static u8 Key_LB_i;			// 按键计数
static u8 Key_LB_data;		// 当前按键数据
static u8 Key_LB_data_Q;	// 上一次按键状态
static u8 Key_LBUnstable_i; // 不稳定计数

void KEY_Filter_processing(void)
{
	Key_LB_data = 0;
	if (!READ_KEY1_STATE) //点火键被按下
		Key_LB_data |= 0x01;
	if (!READ_KEY2_STATE) // 加键被按下
		Key_LB_data |= 0x02;
	if (!READ_KEY3_STATE) // 减键被按下
		Key_LB_data |= 0x04;
	//   if(!READ_KEY3_STATE)
	//		Key_LB_data |= 0x08;

	// 消抖
	if (Key_LB_data_Q == Key_LB_data)
	{
		if (Key_LB_i < 10)
			Key_LB_i++;
		else
		{
			Key_LB_value = Key_LB_data;
			Key_LBUnstable_i = 0;
		}
	}
	else
	{
		Key_LB_i = 0;
		Key_LB_data_Q = Key_LB_data;
		if (Key_LBUnstable_i < 20)
			Key_LBUnstable_i++;
		else
			Key_LB_value = Key_LB_data;
	}
	// Seesaw_Switch_Filter();
	Smoking_KEY_anxia_JC();
}

u8 B_Smoking_KEY_anxia = 0;
static u8 B11 = 0;
void Smoking_KEY_anxia_JC(void)
{
	//       if(B_Smoking_mark==1 && R_key_keep==KEY_M)
	//          B11=1;
	//        else
	//         B11=0;

	if (B11)
		B_Smoking_KEY_anxia = Key_LB_value & KEY_M;
	else
		B_Smoking_KEY_anxia = !(Key_LB_value ^ KEY_M);
}

void Key_smoke(void)
{
	static u8 tim = 0, tim2 = 0, tim3 = 0;
	if ((B_System_off == d_System_Normal) && (!B_ADDSUB_LOCK) &&
		(Disp_windows == Main_interface ||
		 (Disp_windows == Warning_interface && Warning_Windows == KEY_LOCK_windows) ||
		 (Disp_windows == Warning_interface && Warning_Windows == MODE_TO_SET_windows)
		 /*||(Disp_windows_QQ==Main_interface && Disp_windows==Battery_charging_interface)*/))
	{
		if (B_Smoking_KEY_anxia)
		{
			if (!B_Smoking_mark_Q && !tim)
			{
				B_Smoking_mark = 1;
				OUTControl_Init();
			}
			if (tim3)
			{
				tim3--;
				if (!B_Sleep_YES) // 等于0则一定会初始化
					tim = 15;
			}
			else
			{
				tim = 0;
			}
			if (tim2)
			{
				tim2--;
			}
			else
			{
				// if(Disp_windows==Main_interface)
				{
					switch ((u8)R_Skinning_mode2)
					{
					case Short_circuit:
					case Open_circuit:
					case Low_Resistance:
					case High_Resistance:
					case Low_Battery:
					case PCB_Overheated:
						R_Skinning_mode = R_Skinning_mode2;
						break;
					}
					if (R_Skinning_mode2 != Skinning_mode_null)
						tim = 0;
					R_Skinning_mode2 = Skinning_mode_null;
				}
			}
		}
		else
		{
			R_Skinning_mode2 = Skinning_mode_null;
			if (tim)
				tim--;
			if (Disp_windows == Battery_charging_interface)
				tim2 = 50;
			else
				tim2 = 30;
			tim3 = 30;
			//			if(B_OLED_BL&&Disp_windows!=Battery_charging_interface)
			//			  tim2=8;
			//            else
			//              tim2=0;
			//			tim3=15;
			if (!(Key_LB_value & KEY_M))
				B_Smoking_mark_Q = 0;
			B_Smoking_mark = 0;
			B11 = 0;
		}
	}
	else
	{
		B_Smoking_mark_Q = 1;
		B_Smoking_mark = 0;
		R_Skinning_mode2 = Skinning_mode_null;
	}
}
/****************************************************************************/

u8 R_key_anxia;		 // 按下
u8 R_key_Open;		 // 松开
u8 R_key_anxiaOpen;	 // 短按松开
u8 R_key_anzhuOpen;	 // 长按松开
u8 R_key_keep;		 // 长按按下
u8 R_key_super_keep; // 超长按按下
u8 R_key_1C;		 // 按键1次
u8 R_key_2C;		 // 按键2次
u8 R_key_3C;		 // 按键3次
u8 R_key_4C;		 // 按键4次
u8 R_key_5C;		 // 按键5次
u8 R_key_7C;		 // 按键7次
u8 R_key_XC;
u8 R_key_XC_Data;
u8 R_key_anzhu; // 长按按住
u8 B_Key_release;	// 是否返回按键键值，按键全部释放才清零
u8 B_Key_Different; // 按下不同的键
u8 key_value = 0; // 当前按键值寄存器
u8 B_anxiaOpen_EN = 0;
u8 B_anxiaOpen_EN0 = 0;
// KEY_bit	R_KEY_UP=KEY_ADD;
// KEY_bit	R_KEY_DOWN=KEY_SUB;

void Key_scan(void) // uint8
{
	static u8 key_value1 = 0;		  // 按键释放后的按键值寄存器
	static u8 key_value2 = 0;		  // 按键释放后的按键值寄存器
	static u8 key_longtime = 0;		  // 长按时间累积寄存器1
	static u8 key_longtime1 = 0;	  // 长按时间累积寄存器2
	static u8 key_super_longtime = 0; // 超长按时间累积寄存器
	static u8 F_longkey = 0;		  // 长按
	static u8 F_super_longkey = 0;	  // 超长按
	static u8 f_onkey = 0;
	static u8 Key_Hda;
	static u8 KEY_data = 0;
	static u8 B_f_onkey = 0;
	//	Seesaw_Switch_Filter();
	key_value = Key_LB_value;
	Key_smoke();

	if (key_value > 0)
	{
		if (!f_onkey)
		{
			f_onkey = 1;
			B_Key_Different = 0;
			if (key_value != key_value2)
			{
				KEY_data = 0;
			}
			Key_Hda = 0;
			if (key_value2 == KEY_M && KEY_data == 6)
			{
				KEY_data = 0;
				R_key_7C = KEY_M;
				//                R_key_5C=KEY_M;
			}
			if (!B_Key_release)
				R_key_anxia = key_value;

			key_value1 = key_value;
			key_value2 = key_value;
		}
		else
		{						   // 按键长按
			if (key_longtime < 15) // 300
			{
				key_longtime++;
				if (key_value1 != key_value)
				{
					R_key_anxia = key_value;
					key_longtime1 = 0;
					B_anxiaOpen_EN = 1;
				}
				key_value1 = key_value;
				key_value2 = key_value;
			}
			if (key_value == key_value1 && key_value2 == key_value)
			{
				if (!F_longkey)
				{

					if (++key_longtime1 >= 15) // 300
					{
						key_longtime1 = 0; //

						F_longkey = 1;
						// 长按按键处理程序
						KEY_data = 0;
						if (!B_Key_release)
						{
							R_key_keep = key_value;
							if (B_Smoking_mark == 1 && R_key_keep == KEY_M)
							{
								B11 = 1;
							}
						}
					}
				}
				else
				{
					if (!B_Key_release)
						R_key_anzhu = key_value1;
					if (!F_super_longkey)
					{
						if (++key_super_longtime >= 20)
						{
							key_super_longtime = 0; //
							F_super_longkey = 1;
							// 超长按按键处理程序
							if (!B_Key_release)
								R_key_super_keep = key_value1;
						}
					}
					key_value2 = key_value;
				}
			}
			else if ((key_value & key_value1) == 0)
			{
				if (F_longkey)
				{
					if (!B_Key_release)
						R_key_anzhuOpen = key_value1;
				}
				key_longtime = 0;
				key_longtime1 = 0;
				key_super_longtime = 0;
				key_value1 = key_value;
				key_value2 = key_value;
			}
			else if ((key_value & key_value1) && (key_value != key_value1))
			{
				key_value2 = key_value;
				B_Key_Different = 1;
			}
		}
	}
	else //????
	{
		B_Key_Different = 0;
		key_longtime = 0;
		key_longtime1 = 0;
		key_super_longtime = 0;
		if (F_longkey)
		{ // 长按按键释放
			KEY_data = 0;
			F_super_longkey = 0;
			if (!B_Key_release && (!B_anxiaOpen_EN))
				R_key_anzhuOpen = key_value1;
			B_f_onkey = 0;
		}
		else if (f_onkey)
		{ // 短按按键释放
			B_f_onkey = 1;
			if (!B_Key_release)
			{
				if (key_value1 > 0)
				{
					if (KEY_data < 7)
					{
						KEY_data++;
					}
				}
			}
			if (!B_Key_release && !B_anxiaOpen_EN)
				R_key_anxiaOpen = key_value1;
		}

		if (key_value2 > 0)
		{
			if (Key_Hda < 18)
			{
				Key_Hda++;
			}
			else
			{

				if (!B_Key_release && B_f_onkey)
				{
					switch (KEY_data)
					{
					case 1:
						R_key_1C = key_value2;
						break;
					case 2:
						R_key_2C = key_value2;
						break;
					case 3:
						R_key_3C = key_value2;
						break;
					case 4:
						R_key_4C = key_value2;
						break;
					case 5:
						R_key_5C = key_value2;
						break;
					case 7:
						R_key_7C = key_value2;
						break;
					}
					if (!B_anxiaOpen_EN0)
					{
						R_key_XC = key_value2;
						R_key_XC_Data = KEY_data;
					}
				}
				B_anxiaOpen_EN0 = 0;
				KEY_data = 0;
				key_value2 = 0;
			}
		}

		f_onkey = 0;
		F_longkey = 0;
		if (!B_Key_release)
			R_key_Open = key_value1;
		else
		{
			B_Key_release = 0;
			KEY_data = 0;
		}
		B_anxiaOpen_EN = 0;
		key_value1 = 0;
	}
}
/*******************************************************************************/
u8 B_USB_RinBit;
//////////////////USB读键值//////////////////////
////        0:松开     1:插入
/////////////////////////////////////////////////
u8 B_USB_Low;
u8 B_USB_In_Flag = 0;
u16 Bat_V_diff_data = 0;
u16 USB_OVP;
u16 USB_V;
u8 Read_Usb_Stutas = 0;
// u8 B_BAT_Error_10Charge = 0;

// u8     USB_first_IN=0;
// u8     Check_usb_time=0;
void USB_T_data(void)
{
	static u8 Key_Lda = 0;	//,Key_Hda;
	static u8 Key_Lda1 = 0; //,Key_Hda;

	static u8 Key_USB_tim_Lda = 6;	   //,Key_Hda;
	static u8 Read_USB_IIC_Stutas = 1; //,Key_Hda;

	if (B_Out_Yes)
	{
		IP5355_IIC_Write(0x90, 0x22);
		IP5355_IIC_Write(0x51, 0x22);

		//		if (Read_USB_IIC_Stutas)
		//		{
		Read_USB_IIC_Stutas = 0;
		////			IP5355_IIC_Write(0x90, 0x22);
		////			IP5355_IIC_Write(0x51, 0x22);
		//		}
	}
	else if (B_System_off == d_System_Off && B_BAT_Charge == 0)
	{
		IP5355_IIC_Write(0x90, 0x22);
		IP5355_IIC_Write(0x52, 0x22);
	}
	else if (USB_OVP > d_USB_error_H && B_BAT_Charge) // 充电时，超压关闭充放电，保护电池一直强关
	{
		//		B_BAT_Error_10Charge = 10;
		IP5355_IIC_Write(0x90, 0x22);
		IP5355_IIC_Write(0x51, 0x22); // 关放电关充电
	}
	//	else if ((USB_OVP < (d_USB_error_H - 50)) && B_BAT_Charge && (B_BAT_Error_10Charge == 1 || B_BAT_Error_10Charge == 2)) // 延时开启,9*20ms
	//	{
	//		IP6106_Init();
	//		IP5355_IIC_Write(0x90, 0x22);
	//		IP5355_IIC_Write(0x52, 0x22); // 关放电开充电
	//	}
	else if (R_BattL_Proportion == 0 && Read_USB_IIC_Stutas != 2)
	{
		Read_USB_IIC_Stutas = 2;
		IP5355_IIC_Write(0x90, 0x22);
		IP5355_IIC_Write(0x52, 0x22); // 关放电开充电
	}
	else if (R_BattL_Proportion && Read_USB_IIC_Stutas != 3)
	{
		Read_USB_IIC_Stutas = 3;
		IP5355_IIC_Write(0x80, 0x22);
		IP5355_IIC_Write(0x40, 0x22); // 开放电开充电
	}

	//	if (B_BAT_Error_10Charge)
	//	{
	//		B_BAT_Error_10Charge--;
	//	}

	if (Key_USB_tim_Lda)
	{
		Key_USB_tim_Lda--;
	}
	if (Key_USB_tim_Lda == 0 && !B_Out_Yes)
	{
		Key_USB_tim_Lda = 6;
		Read_Usb_Stutas = IP5355_IIC_Read(0x11);
		if ((Read_Usb_Stutas & 0x10) == 0 && (Read_Usb_Stutas & 0x20) == 0)
		{
			Read_Usb_Stutas = IP5355_IIC_Read(0x11);
		}
		// B_BAT_OutCharge = Read_Usb_Stutas;
		if (B_BAT_OutCharge) // 放电中
		{
			// USB_OVP = Read_USB_OVP_Data();
			if (R_BattL_Proportion == 0)
			{
				B_BAT_OutCharge = 0;
				IP5355_IIC_Write(0x90, 0x22);
				IP5355_IIC_Write(0x52, 0x22); // 关放电开充电
			}
		}
	}

	if (!B_BAT_Charge && !B_USB_Low && !B_Out_Yes && ((Read_Usb_Stutas & 0x20) == 0))
	{

		USB_OVP = Read_USB_OVP_Data();
		if (USB_OVP > 330)
		{

			B_USB_RinBit = 1;
			// Key_Hda=0;
			if (R_BAT_MAX_Real < 440)
			{

				if (Key_Lda < 6)
				{
					Key_Lda++;
					if (R_SleepTim < 5)
						R_SleepTim = 5;
				}
				else
				{
					//					Ip6106_USB_IIC_Init();
					IP6106_Init();
					// GP_USB_EN();
					IP5355_IIC_Write(0x80, 0x22); // 开启手机充电
					// IP5355_IIC_Write(0x42, 0x22);
					IP5355_IIC_Write(0x40, 0x22);

					Key_Lda = 0;

					CHG_DISABLE();
					B_usbOnOff = 0; // 更新USB充电状态标志为关闭
					Decide_L = 0;
					Full_count = 0;
					Full_Cnt = 0;
					R_Batter_Interface_Tim = 0;
					BATT_Charging_Count_down_Init();
					First_In_USB = 1;
					F_BAT_Actual_Init();
					{
						if (USB_OVP < 420 || USB_OVP > d_USB_error_H)
							CHG_DISABLE();
						B_usbOnOff = 0; // 更新USB充电状态标志为关闭
						Delay_Nms(1);	// 消抖
						USB_OVP = Read_USB_OVP_Data();
						if (USB_OVP < 420 || USB_OVP > d_USB_error_H)
						{
							B_USB_Low = 1;
							CHG_DISABLE();
							IP5355_IIC_Write(0x90, 0x22);
							IP5355_IIC_Write(0x51, 0x22);
							R_Skinning_mode = USB_anomaly;
							return;
						}
					}
					if (Read_Usb_Stutas & 0x10)
					{
						Check_bat_time = 0;
						CHG_ENABLE();	// 使能充电
						B_usbOnOff = 1; // 更新USB充电状态标志为开启
						// CHG_DISABLE();
						B_USB_Low = 0;
						B_BAT_Charge = 1; // 充电中
						B_BAT_OutCharge = 0;
						B_USB_In_Flag = 0;
						R_Skinning_mode = USB_access;
						if (Read_Data != 0xab)
						{
							IP5355_IIC_Write(0xab, 0x3a); // 电流及温度设置
														  // IP5355_IIC_Write(0x1B, 0x3a); // 电流及温度设置
						}
					}

					//  R_Skinning_mode = USB_output;
				}
			}

			// IP5355_IIC_Write(0x03, 0x08); // 充满，清除中断
		}
		else
		{
			//			USB_first_IN=0;
			First_In_USB = 0;
			Key_Lda = 0;
			B_USB_RinBit = 0;
			B_USB_In_Flag = 0;
		}
	}

	if (B_BAT_OutCharge == 0 && (Read_Usb_Stutas & 0x20) && B_System_off != d_System_Off && !B_Out_Yes && USB_OVP > 400) // 放电
	{

		if (R_BattL_Proportion)
		{
			B_BAT_Charge = 0;
			B_BAT_OutCharge = 1; // 电池输出充电标志
			R_Skinning_mode = USB_access;
		}
		else
		{
			IP5355_IIC_Write(0x90, 0x22);
			IP5355_IIC_Write(0x52, 0x22); // 关放电开充电
		}
		// R_Skinning_mode= USB_anomaly;
		// Windows_Switch(Battery_BatOutput_interface);
	}
}

void WakeUp_Check_USB(void)
{
	USB_OVP = Read_USB_OVP_Data();
	if (USB_OVP > 300)
	{

		// 配置为模拟口
		//          USB_GPIO_Set(USB_AIN);

		Decide_L = 0;
		Full_count = 0;
		Full_Cnt = 0;
		BATT_Charging_Count_down_Init();
		F_BAT_Actual_Init();
		if (USB_OVP < 420 || USB_OVP > d_USB_error_H)
		{
			CHG_DISABLE();
			Delay_Nms(1); // 消抖
			USB_OVP = Read_USB_OVP_Data();
			if (USB_OVP < 420 || USB_OVP > d_USB_error_H)
			{
				B_BAT_Charge = 0;
				B_USB_Low = 1;
				CHG_DISABLE();
				if (Disp_windows != USB_Warning_interface)
					R_Skinning_mode = USB_anomaly;
				else
					R_Skinning_mode = Skinning_init;
				return;
			}
		}
		B_USB_RinBit = 1;
		B_BAT_Congman2 = 0;
		if (!B_Out_Yes)
		{
			CHG_ENABLE();
		}
		B_USB_Low = 0;
		B_USB_In_Flag = 0;
		R_Skinning_mode = USB_access;
		if (Disp_windows != Battery_charging_interface && !B_BAT_Charge)
			R_Skinning_mode = USB_access;
		else
			R_Skinning_mode = Skinning_init;
		B_BAT_Charge = 1;
	}
}

u8 B_InterfaceGOTO = 0;
/// 按键处理
void KEY_ALL_Task(void)
{
	if (B_KEY_JC)
	{
		B_KEY_JC = 0;
		feed_Watchdog();
		Key_scan();
		USB_T_data();
		B_InterfaceGOTO = 1;
		Interface_display();
		B_InterfaceGOTO = 0;
	}
}

void KEY_TFT_RESET(void)
{
	if (B_KEY_JC)
	{
		B_KEY_JC = 0;
		B_InterfaceGOTO = 0;
		Key_scan();
		Read_load();
		B_InterfaceGOTO = 1;
	}
}
/***********************************************************************************************/

// 加减操作  //xh=0:加减不做循环  xh=1:加减到最大再次按下循环 xh=2：循环
u16 KEY_R_ADD_SUB(u8 *key_value, s16 R, u8 KADD, u8 KSUB, s16 max, s16 min, s8 bu, u8 xh, u8 tim)
{
	static u8 B;

	if ((R == max || R == min) && xh == 1)
	{
		if (R_key_anxia == KADD || R_key_anxia == KSUB)
			B = 1;
	}
	else
	{
		B = 0;
	}

	// 短按
	if (*key_value == KADD)
	{
		if (R < max)
		{
			R += bu;
			R_Bright_screen_Time = R_Bright_screen_TimeMAX;
		}
		else if (xh)
		{
			R = min;
		}
	}
	else if (*key_value == KSUB)
	{
		if (R > min)
		{
			R -= bu;
			R_Bright_screen_Time = R_Bright_screen_TimeMAX;
		}
		else if (xh)
		{
			R = max;
		}
	}

	// 长按
	if (R_key_anzhu == KADD)
	{
		if (!R_KEY_timi)
		{
			if (R_KEY_timMax > tim)
			{
				R_KEY_timMax -= 5;
			}
			else
			{
				R_KEY_timMax = tim;
			}
			R_KEY_timi = R_KEY_timMax;
			if (R < max)
			{
				R += bu;
				R_Bright_screen_Time = R_Bright_screen_TimeMAX;
			}
			else if (xh == 2 || B)
			{
				R = min;
			}
		}
	}
	else if (R_key_anzhu == KSUB)
	{
		if (!R_KEY_timi)
		{
			if (R_KEY_timMax > tim)
			{
				R_KEY_timMax -= 5;
			}
			else
			{
				R_KEY_timMax = tim;
			}
			R_KEY_timi = R_KEY_timMax;
			if (R > min)
			{
				R -= bu;
				R_Bright_screen_Time = R_Bright_screen_TimeMAX;
			}
			else if (xh == 2 || B)
			{
				R = max;
			}
		}
	}
	else if (!R_key_anzhu)
	{
		R_KEY_timMax = 150; // 150ms
	}
	if (R > max)
		R = max;
	else if (R < min)
		R = min;
	return R;
}
