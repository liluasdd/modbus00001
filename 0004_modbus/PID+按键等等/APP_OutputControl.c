#include "APP_Outputcontrol.h"
#include "main_2.h"
#include "WM_Disp.h"

// ó??§éè?¨?μ

//////////////////////////////////////////////////////////////

u8 B_Smoking_mark;		 ///
u8 B_Smoking_mark_Q = 1; ///
u8 B_Power_down;		 ///

u8 R_Smoke_Tim;		 // 吸烟计时			//100ms		00.0
u16 R_Smoke_Tim10ms; // 吸烟计时			//10ms		00.00
u16 R_Smoke_Tim1ms;

u8 B_Out_Yes;		/// 正在输出标志
u8 B_Out_JC;		/// 输出电压检测
u8 B_load_JC;		/// 电阻检测
u8 B_load_OK;		/// 电阻检测
u8 B_yule = 0;		/// 预热
u8 B_AUOT_MODE = 1; // 麦克维尔的自动模式

u16 R_MODE_Out_ResMin;

////////最大允许输出功率 放大100倍
u16 R_PowerMAX; // 10mW	00.00
////////最大允许输出功率	放大100倍
u16 R_PowerMAXMAX; // 10mW	00.00
////////温控最大输出功率	放大100倍
u16 R_Temp_PowerMAX; // 10mW	00.00
u16 R_Temp_MAX;		 //
/*******************************************/
u16 R_Out_W_Set;	/// ê?3?Wêy		00.00
u16 R_Out_V_Set;	/// ê?3?V		0.00
u16 R_Out_Temp_Set; /// ê?3????è	000

u16 R_Res_Actual;		// 平均阻值		0.000
u16 R_Out_I_Actual;		// 平均电流		0.00
u16 R_Out_V_Actual;		// 平均电压		0.00
u16 R_Out_W_Actual;		// 平均瓦数		0.00
u16 R_Out_TempC_Actual; // 平均温度		000
u16 R_Out_TempF_Actual; // 平均温度		000
u32 R_Out_I_SUM;		///
u32 R_Out_V_SUM;		///
u32 R_Out_W_SUM;		///
u8 R_Out_V_i;			///
u8 R_Out_V_i_max;		///

u32 R_Out_Temp_SUM; ///
u8 R_Out_Temp_i;	///
u32 R_Real_SUM;		///
u8 R_Real_i;		///

u32 R_T_Gradient_out; // 电阻温度系数_out
u8 R_Efficiency;	  // 转换效率

/*******************************************/

u16 R_real_Temp; /// 实时温度		//000
u16 R_real_BAT;	 /// 实时电池电压	0.00
u16 R_real_WOUT; /// 实时输出瓦数	0.00
u16 R_real_VOUT; /// 实时输出电压	0.00
u16 R_real_IOUT; /// 实时输出电流	0.00
u16 R_real_Res;	 /// 实时电阻			0.000

u8 R_Lbat_LB; /// 低电量滤波

u8 R_Temp_compensate;		   // 温度补偿值
u8 R_Temp_compensate_MAX = 15; // 温度补偿最大值

u8 R_XY_Tim_yanshi = 0;
u8 Temp_Greater_Than_50 = 0;
Skinning_mode R_Skinning_modeOUT; // 窗口状态
/************************************************/
void (*Output_control_program)(void); // 输出控制程序

void PID_init(void);
void PID_realize(u16 speed, u16 Aspeed);
void Fault_handling(Skinning_mode R);

/// 统计平均输出电压电流功率
void F_Average_Vout(void)
{
	R_Out_V_i++;
	R_Out_V_SUM += R_real_VOUT;
	R_Out_I_SUM += R_real_IOUT;
	R_Out_W_SUM += R_real_WOUT;
	if (R_Out_V_i >= R_Out_V_i_max)
	{
		/*统计平均电压*/
		R_Out_V_Actual = R_Out_V_SUM / R_Out_V_i_max;
		if (R_Out_V_Actual > d_User_Vout_max)
			R_Out_V_Actual = d_User_Vout_max;
		//////////////////////////////////////////

		/*统计平均电流*/
		R_Out_I_Actual = R_Out_I_SUM / R_Out_V_i_max;
		if (R_Out_I_Actual > d_User_Iout_max_x100)
			R_Out_I_Actual = d_User_Iout_max_x100;
		/////////////////////////////////
		/*统计平均瓦数*/
		R_Out_W_Actual = R_Out_W_SUM / R_Out_V_i_max / 10;
		if (R_Out_W_Actual > d_User_Watt_max)
			R_Out_W_Actual = d_User_Watt_max;

		R_Out_W_SUM = 0;
		R_Out_V_SUM = 0;
		R_Out_I_SUM = 0;
		R_Out_V_i = 0;
		if (R_Out_V_i_max < 200)
			R_Out_V_i_max = 200;
	}
}

/// 统计平均温度
void F_Average_Temp(void)
{
	R_Out_Temp_i++;
	R_Out_Temp_SUM += R_real_Temp;
	if (R_Out_Temp_i >= 10)
	{
		R_Out_TempC_Actual = R_Out_Temp_SUM / 10;
		if (R_Out_TempC_Actual > 500)
		{
			R_Out_TempC_Actual = 500;
		}
		R_Out_TempF_Actual = R_Out_TempC_Actual * 9 / 5 + 32;
		R_Out_Temp_SUM = 0;
		R_Out_Temp_i = 0;
	}
}

// 求平均电阻
void F_Average_Real(void)
{
	R_Real_i++;
	R_Real_SUM += R_real_Res;
	if (R_Real_i >= 20)
	{
		R_Res_Actual = R_Real_SUM / 20;
		if (R_Res_Actual > 9999)
			R_Res_Actual = 9999;
		R_Real_i = 0;
		R_Real_SUM = 0;
	}
}

// 判断是否过温
u8 Over_PCB_Temp(void)
{
	u16 PcbTemp = 0;
	//	u16  batTemp0=0;
	//    s16  CPUTemp=0;
	static u8 B_Overheated = 0;
	PcbTemp = ADC_TDx_Data(ADC_CHANNEL_PCBTEMP, 12);
	//	CPUTemp=ReadTemp_Data();
	// batTemp0=Read_BAT_TEMP();
	if (PcbTemp < d_T50 /* || CPUTemp>50*/)
		Temp_Greater_Than_50 = 1;
	else
		Temp_Greater_Than_50 = 0;

	if (B_Overheated)
	{
		if (PcbTemp < d_T60 /*|| batTemp0<d_T50*/)
			return 1;
		else
			B_Overheated = 0;
	}
	else
	{
		if (PcbTemp < d_T80 /*|| batTemp0<d_T70*/)
		{
			B_Overheated = 1;
			return 1;
		}
	}
	return 0;
}

/////////////
void Out_Read_load(void)
{
	// 如果需要检测负载电阻
	if (B_load_JC) //20ms检测一次负载电阻
	{
		// 清除检测标志，只执行一次检测
		B_load_JC = 0;
		// 读取一次当前负载电阻值
		R_real_Res = Read_L_1C();
		
		// 如果电阻值大于等于最大允许电阻，判定为断路
		if (R_real_Res >= d_Res_max)
		{
			// 再读取一次做防抖确认
			u16 R_Res = Read_L_1C();
			// 延时等待稳定
			Delay_Nus(500);
			// 两次都检测到高电阻，确认断路故障
			if (R_Res >= d_Res_max)
			{
				// 处理断路故障，关闭输出并设置对应故障码
				Fault_handling(Open_circuit);
			}
		}
		// 如果电阻值小于当前模式允许的最小电阻（留20的容差），判定为短路
		else if (R_real_Res < R_MODE_Out_ResMin - 20)
		{
			// 处理短路故障，关闭输出并设置对应故障码
			Fault_handling(Short_circuit);
		}
		// 电阻在正常范围内
		else
		{
			// 标记负载检测正常通过
			B_load_OK = 1;
			// 更新平均电阻统计
			F_Average_Real();
			// 检查PCB温度，如果过温触发过温故障
			if (Over_PCB_Temp())
				Fault_handling(PCB_Overheated);
		}
	}
}


////////////////
void Fault_handling(Skinning_mode R)
{
	PWM_OFF();
	COMP_ONorOFF(0);
	B_Smoking_mark = 0;
	R_Skinning_modeOUT = R;
}

void Fault_handling2(Skinning_mode R)
{
	B_Smoking_mark = 0;
	R_Skinning_mode2 = R;
}

////////////////
void Count_Wout(void)
{
	if (R_Out_W_Set > R_PowerMAX)
	{
		R_Out_W_Set = R_PowerMAX;
	}
}

///////////电压调节
void Vout_adjust(u8 bu)
{
	if (R_real_BAT > d_Bat_OUT_Volt_min && R_real_VOUT + 5 < R_Out_V_Set && R_real_WOUT < d_User_Watt_max_x10 + 50 && R_real_IOUT < d_User_Iout_max_x100 - 30)
	{
		R_OUT_PWM_data += bu;
		OUT_PWM_Wdata(&R_OUT_PWM_data);
	}
	else if (R_real_BAT < d_Bat_OUT_Volt_min - 10 || R_real_VOUT > R_Out_V_Set + 5 || R_real_WOUT > d_User_Watt_max_x10 + 50 || R_real_IOUT > d_User_Iout_max_x100)
	{
		R_OUT_PWM_data -= bu;
		OUT_PWM_Wdata(&R_OUT_PWM_data);
	}
}
///////////瓦数调节
void Ft_out_adjust(s8 bu)
{
	static u8 i;
	static u8 j;
	if (R_real_BAT > d_Bat_OUT_Volt_min && R_real_WOUT + 5 < R_Out_W_Set && R_real_VOUT < d_User_Vout_max + 5 && R_real_IOUT < d_User_Iout_max_x100 - 30)
	{
		j = 0;
		i++;
		if (R_real_WOUT + 50 < R_Out_W_Set)
			i = 2;
		if (i >= 2)
		{
			i = 0;
			R_OUT_PWM_data += bu;
			OUT_PWM_Wdata(&R_OUT_PWM_data);
		}
	}
	else
	{
		if (R_real_BAT < d_Bat_OUT_Volt_min - 10 || R_real_VOUT > d_User_Vout_max + 5 || R_real_IOUT > d_User_Iout_max_x100)
		{
			j = 2;
		}
		else if (R_real_WOUT > R_Out_W_Set + 20)
		{
			i = 0;
			j++;
			if (R_real_WOUT > R_Out_W_Set + 50)
				j = 2;
		}
		else
		{
			i = 0;
			j = 0;
		}
		if (j >= 2)
		{
			j = 0;
			R_OUT_PWM_data -= bu;
			OUT_PWM_Wdata(&R_OUT_PWM_data);
		}
	}
}
///////////瓦数调节

void Wout_adjust(u8 bu)
{
	static u8 i;
	static u8 j;
	if (R_real_BAT > d_Bat_OUT_Volt_min && R_real_WOUT + 50 < R_Out_W_Set && R_real_VOUT < d_User_Vout_max + 10 && R_real_IOUT < d_User_Iout_max_x100 - 30)
	{
		j = 0;
		i++;
		if (R_real_WOUT + 100 < R_Out_W_Set)
		{
			i = 20;
		}

		if (i >= 20)
		{
			i = 0;
			R_OUT_PWM_data += bu;
			OUT_PWM_Wdata(&R_OUT_PWM_data);
		}
	}
	else
	{
		if (R_real_BAT < d_Bat_OUT_Volt_min - 10 || R_real_VOUT > d_User_Vout_max + 10 || R_real_IOUT > d_User_Iout_max_x100)
		{
			j = 20;
		}
		else if (R_real_WOUT > R_Out_W_Set + 50)
		{
			i = 0;
			j++;
			if (R_real_WOUT > R_Out_W_Set + 100)
				j = 20;
		}
		else
		{
			i = 0;
			j = 0;
		}
		if (j >= 20)
		{
			j = 0;
			R_OUT_PWM_data -= bu;
			OUT_PWM_Wdata(&R_OUT_PWM_data);
		}
	}
}

////////////////?μ1|?ê
u8 Power_down_control(void)
{
	u8 B;
	static u8 Power_i;
	B = 0;
	if (R_real_BAT < d_Bat_OUT_Volt_min)
	{
		Power_i = 0;
		if (R_real_WOUT > 1000 && R_PowerMAXMAX > 1000)
		{
			if (R_real_WOUT < R_PowerMAX + 200)
			{
				B = 1;
				B_Power_down = 1;
				R_PowerMAXMAX -= 200;
				if (R_PowerMAXMAX < 1000)
					R_PowerMAXMAX = 1000;
				R_PowerMAX -= 500;
				if (R_PowerMAX < 1000)
					R_PowerMAX = 1000;
			}
		}
		else
		{
			R_Lbat_LB++;
			if (R_Lbat_LB >= 5)
			{
				//				Fault_handling(Low_Battery);
				//                Windows_Switch(Main_interface);
				R_BattL_Proportion = 0;
				BatterGrade = 0;
				Fault_handling(Skinning_mode_null);
				u16 R_Res = Read_L_1C();

				if (R_Res < 50)
				{
					Fault_handling(Short_circuit);
				}
				else
				{
					R_XY_Tim_yanshi = 15;
				}
			}
		}
	}
	else
	{
		R_Lbat_LB = 0;
		if (R_real_BAT > d_Bat_Volt_min)
		{
			Power_i++;
			if (Power_i > 10)
			{
				Power_i = 0;
				if (R_PowerMAX < R_PowerMAXMAX)
				{
					R_PowerMAX += 100;
					B = 1;
				}
			}
		}
		else
		{
			Power_i = 0;
		}
	}
	return B;
}

/////////ê?3?3?ê??ˉ
u8 B_again;
u8 R_again_Tim = 100;
u8 Over_I = 0;

static u8 MODE_OUT_int(u16 ResMin)
{
	u8 R_return;
	u8 B = 0;
	u16 R_L;
	R_return = 0;

	R_MODE_Out_ResMin = ResMin;
	if (!R_XY_Tim_yanshi)
	{
		if (BatterGrade)
			B = 1;
	}
	else
	{
		if (R_BATT_Actual > (d_Bat_OUT_Volt_min - 10))
			B = 1;
	}
	if (B)
	{
		//////////////////////////////////
		if (Sleep_Before_Res)
		{
			u8 BB = 0;
			if (R_Res_average < ResMin)
			{
				Enable_VoutADC_Use();
				Delay_Nus(500);
				u16 B_Data = Read_outV_Data();
				if (B_Data < 30)
				{
					BB = 1;
				}
			}
			if (!B_have_Atomizer || BB) // 输出前如果没有雾化器 快速读雾化器
			{
				u16 Before_out_res_Data = 0;
				Enable_VoutADC_Use();
				for (u8 i = 0; i < 3; i++)
				{
					u16 R_L0 = Read_L_1C();//读取电阻值
					Before_out_res_Data += R_L0;
				}
				Before_out_res_Data = Before_out_res_Data / 3;//取平均值作为电阻值
				if (Before_out_res_Data >= d_Res_min && Before_out_res_Data <= d_Res_usable)//大于最小电阻值且小于等于最大可用电阻值
				{
					R_Res_average = Before_out_res_Data;
					// B_Atomizer_inquiry=1;
					// B_have_Atomizer=1;
					R_Res_average_QQ = R_Res_average;
					if (B_LOCK_Res)
						R_Res_Initial = R_Res_Initial_jiyi;
					else
						R_Res_Initial = R_Res_average;
					if (R_Res_average <= d_Res_usable)
						B_Atomizer_inquiry = 1;//雾化器存在
					B_have_Atomizer = 1;
				}
			}
			Sleep_Before_Res = 0;
		}
		////////////////////////////////
		if (R_Res_average >= ResMin && R_Res_average < d_Res_max)
		{
			Enable_VoutADC_Use();
			if (!B_again)
			{
				B_again = 1;
				R_L = Read_L_1C();
				if ((R_L + R_L * 5 / 100) < R_Res_average && (R_L + 5) < R_Res_average)
				{
					B_Sleep_YES = 4;
					R_Read_Rtim = 50;
					return 0;
				}
			}
		}
		if (R_Res_average < ResMin / 2)
		{
			Fault_handling2(Short_circuit);
		}
		else if (R_Res_average < ResMin)
		{
			Fault_handling2(Low_Resistance);
		}
		else if (R_Res_average >= d_Res_max)
		{
			Fault_handling2(Open_circuit);
		}
		else if (R_Res_average > d_Res_usable)
		{
			Fault_handling2(High_Resistance);
		}
		else if (Over_PCB_Temp())
		{
			Fault_handling2(PCB_Overheated);
		}
		else
		{
			if (B_BAT_Charge)
			{
				CHG_DISABLE(); // 关闭充电
			}
			R_again_Tim = 100;
			if (R_Res_average >= (d_User_Watt_max_x10 / (d_User_Iout_max * d_User_Iout_max / 10)))
			{
				R_PowerMAXMAX = d_User_Watt_max_x10 - 500;
			}
			else
			{
				R_PowerMAXMAX = (d_User_Iout_max * d_User_Iout_max / 10) * R_Res_average;
			}
			B_Power_down = 0; /// 降功率
			R_Smoke_Tim = 0;  // 吸烟计时			//100ms		00.0
			R_Smoke_Tim10ms = 0;
			R_Smoke_Tim1ms = 0;
			B_Out_JC = 0;				  /// 输出电压检测
			B_load_JC = 0;				  /// 电阻检测
			B_load_OK = 0;				  /// 电阻检测
			B_yule = 0;					  /// 预热
			R_Res_Actual = R_Res_average; //???ù×è?μ		0.000
			R_Out_I_Actual = 0;			  // 平均电流		0.00
			R_Out_V_Actual = 0;			  // 平均电压		0.00
			R_Out_W_Actual = 0;			  // 平均瓦数		0.0
			R_Out_TempC_Actual = 0;		  // 平均温度		000
			R_Out_TempF_Actual = 0;		  // 平均温度		000
			R_Out_I_SUM = 0;			  ///
			R_Out_V_SUM = 0;			  ///
			R_Out_W_SUM = 0;			  ///
			R_Out_V_i = 0;				  ///
			R_Out_V_i_max = 10;
			R_Out_Temp_SUM = 0; ///
			R_Out_Temp_i = 0;	///
			R_Real_SUM = 0;		///
			R_Real_i = 0;		///
			Decide_L = 0;
			Full_count = 0;
			Full_Cnt = 0;
			R_Efficiency = 100;
			output_2s_mark = 0;
			
			R_Skinning_modeOUT = Skinning_mode_null;

			//            if(R_Work_mode==d_FT_mode||R_Work_mode==d_PUL_mode||  R_Work_mode==d_ECO_mode)
			//            {
			//
			//                  Over_I=sqrt_16(R_User_Watt_Set*130/R_Res_average);
			//
			//                 if(Over_I<20)
			//                 {
			//                     Over_I+=8;
			//                 }
			//                 else if(Over_I<25)
			//                 {
			//                     Over_I+=7;
			//                 }
			//                 else if(Over_I<30)
			//                 {
			//                     Over_I+=6;
			//                 }
			//                 else
			//                 {
			//                     Over_I=d_OVER_Iout_max;
			//                 }
			//                 Update_Short_Volage(Over_I);
			//            }
			//            else
			//            {
			//                Update_Short_Volage(d_OVER_Iout_max);
			//            }
			R_XY_Tim_yanshi = 30;
			COMP_ONorOFF(1);
			PWM_ON();
			B_Out_Yes = 1;
			R_return = 1;
		}
	}
	else
	{
		//		Fault_handling2(Low_Battery);
		if (Disp_windows != Main_interface)
		{
			Windows_Switch(Main_interface);
		}
	}
	return R_return;
}
void Out_int(void)
{
	switch ((u8)R_Work_mode)
	{
		//		case	d_Bypass_mode:
		//            if(MODE_OUT_int(d_Res_min))Bypass_mode_int();break;
	case d_Temp_mode:
		if (MODE_OUT_int(d_Res_min))
			Temp_mode_int();
		break;
	case d_TCR_mode:
		if (MODE_OUT_int(d_Res_min))
			TCR_mode_int();
		break;
		//		case	d_Custom_mode:
		//            if(MODE_OUT_int(d_Res_min))Custom_mode_int();break;
	case d_Voltage_mode:
		if (MODE_OUT_int(d_Res_min))
			Voltage_mode_int();
		break;
		//		case	d_SPP_mode:
		//            if(MODE_OUT_int(d_Res_min))SPP_mode_int();break;
		//		case    d_STC_mode:
		//            if(MODE_OUT_int(d_Res_min+50))STC_mode_int();break;
		// case    d_PUL_mode:
		//    if(MODE_OUT_int(d_Res_min))Pull_mode_int(); break;
		// case    d_FT_mode:
		//      if(MODE_OUT_int(d_Res_min)) Ft_mode_int();break;
	case d_Watt_mode:
		if (MODE_OUT_int(d_Res_min))
			Watt_mode_int();
		break;
	default:
		if (MODE_OUT_int(d_Res_min))
			Watt_mode_int();
		break;
	}
}

void OUT_YES_Control(void)
{
	if (B_Smoking_mark)
	{
		if (B_Out_Yes)
		{
			if (B_Out_JC) //20ms检测一次输出电压
			{

				B_Out_JC = 0;
				R_XY_Tim_yanshi = 35;
				R_real_BAT = ReadBATz_Data();//读取电池电压
				ReadLoad_Data();//读取电阻数据
				R_real_VOUT = R_ReadLoad_outV;//读取输出电压
				R_real_IOUT = R_ReadLoad_outI;//读取输出电流
				if (Temp_Greater_Than_50)
				{
					if (R_real_VOUT > 120 && R_real_VOUT < 190)
					{
						R_real_IOUT = R_real_VOUT * 1000 / R_Res_average;
					}
				}
				if (R_real_IOUT > d_OVER_Iout_max_x100)
				{
					Fault_handling(Short_circuit);//电流过大报短路
				}
				R_real_WOUT = R_real_VOUT * R_real_IOUT / R_Efficiency;
				Out_Read_load();
				F_Average_Vout();
				(*Output_control_program)();
			}
			if (R_Smoke_Tim10ms >= R_User_Smoke_Tim_max * 10 && R_Smoke_Tim >= R_User_Smoke_Tim_max)
			{
				Fault_handling(Smoking_timeout);//吸烟超时报超时
			}

			Statistics_numberANDlengthTim(R_Smoke_Tim);//统计吸烟时间
		}
		if (!B_Smoking_KEY_anxia)
		{
			Fault_handling(Skinning_mode_null);//吸烟按键未按下，松开时
		}
	}
}

void OUTControl_Init(void)
{
	if (B_Sleep_YES)
		return;
	if (B_Smoking_mark)
	{
		B_Smoking_mark_Q = 1;
		if (!B_Out_Yes)
		{
			Out_int();
		}
	}
}

///////////输出总控制
void OUTControl(void)
{

	if (R_SleepTim && !B_BAT_error) // 电池正常
	{

		OUTControl_Init();
		OUT_YES_Control();
	}
	else
	{
		B_Smoking_mark = 0;
	}
	if (!B_Smoking_mark)
	{
		if (B_Out_Yes)
		{
			// if (R_BattL_Proportion == 0)
			// {
			// 	IP5355_IIC_Write(0x90, 0x22);
			// 	IP5355_IIC_Write(0x52, 0x22); // 强制关放电,开充电
			// }
			// else
			// {
			// 	IP5355_IIC_Write(0x80, 0x22);
			// 	IP5355_IIC_Write(0x40, 0x22); // 开自动充放电
			// }

			if (R_Skinning_modeOUT != Skinning_mode_null)
			{
				R_Skinning_mode = R_Skinning_modeOUT;
				R_Skinning_modeOUT = Skinning_mode_null;
			}
			if (B_BAT_Charge)
			{
				CHG_ENABLE(); // 开充电电流
			}
			// 大于40W,电量显示小于80%，吸烟超2S
			if (R_Work_mode != d_Temp_mode && R_Smoke_Tim10ms > 190 && R_BattL_Proportion < 80 && R_User_Watt_Set > 400)
			{
				output_2s_mark = 1;
			}
			B_Out_Yes = 0;
			B_again = 0;
			PWM_OFF();
			COMP_ONorOFF(0);
			R_Out_I_Actual = 0;		// 平均电流		0.00
			R_Out_V_Actual = 0;		// 平均电压		0.00
			R_Out_W_Actual = 0;		// 平均瓦数		0.0
			R_Out_TempC_Actual = 0; // 平均温度		000
			R_Out_TempF_Actual = 0; // 平均温度		000
		}
	}
}

typedef struct
{
	s16 SetSpeed;	 // 定义设定值
	s16 ActualSpeed; // 定义实际值
	s16 err;		 // 定义偏差值
	s16 err_last;	 // 定义上一个偏差值
	s16 err_last2;	 // 定义上上一个偏差值
	u8 Kp, Ki, Kd;	 // 定义比例、积分、微分系数
	float voltage;	 // 定义瓦数值
	//	s16	integral;//定义积分值
	u8 L_Power_i;
	u8 H_Power_i;

	u8 B_Temp_state;
	u8 B_state_i;
} _PID;

_PID pid;

void PID_init(void)
{
	pid.SetSpeed = 0;
	pid.ActualSpeed = 0;
	pid.err = 0;
	pid.err_last = 0;
	pid.voltage = 0;
	//	pid.integral=0;
	pid.Kp = 2;
	pid.Ki = 3;
	pid.Kd = 1;
	pid.L_Power_i = 0;
	pid.H_Power_i = 0;

	pid.B_Temp_state = 0;
	pid.B_state_i = 0;
}

void PID_realize(u16 speed, u16 Aspeed)
{
	pid.SetSpeed = speed;
	pid.ActualSpeed = Aspeed;
	pid.err = pid.SetSpeed - pid.ActualSpeed;

	if (!B_yule)
	{
		if (pid.ActualSpeed > pid.SetSpeed || R_Smoke_Tim > 2)
		{
			B_yule = 1;
			pid.Ki = 5;

			R_Out_W_Set = R_Out_W_Set * 2 / 3;
		}
	}
	if (pid.ActualSpeed > 350)
	{
		R_Out_W_Set = 0;
		R_Temp_PowerMAX = 0;
		pid.B_Temp_state = 1;
	}
	else if (pid.ActualSpeed > 340)
	{
		R_Out_W_Set = R_Out_W_Set * 2 / 3;
		R_Out_W_Set -= 500;
		if (R_Temp_PowerMAX > 500)
			R_Temp_PowerMAX -= 500;
		else
			R_Temp_PowerMAX = 0;
	}
	else if (pid.err < -15)
	{
		pid.H_Power_i = 0;

		if (pid.L_Power_i < 2)
		{
			pid.L_Power_i++;
		}
		else
		{
			pid.L_Power_i = 0;
			R_Out_W_Set -= 400;
			if (R_Temp_PowerMAX > 500)
				R_Temp_PowerMAX -= 500;
			else
				R_Temp_PowerMAX = 0;
		}
	}
	else
	{
		pid.L_Power_i = 0;
		if (pid.ActualSpeed < 200)
		{
			pid.H_Power_i++;
			if (pid.H_Power_i > 4)
			{
				pid.H_Power_i = 0;
				if (R_Temp_PowerMAX < R_PowerMAXMAX && pid.B_Temp_state != 2)
				{
					R_Temp_PowerMAX += 250;
				}
			}
		}
		else
		{
			pid.H_Power_i = 0;
		}
	}

	if (pid.err < 2 && pid.err > -5)
	{
		pid.voltage = 0;
	}
	else
		pid.voltage = pid.Kp * (pid.err - pid.err_last) + pid.Ki * pid.err + pid.Kd * (pid.err - 2 * pid.err_last + pid.err_last2);
	pid.err_last2 = pid.err_last;
	pid.err_last = pid.err;
	if (pid.voltage > 500)
	{
		pid.voltage = 500;
	}
	else if (pid.voltage < -1000)
	{
		pid.voltage = -1000;
	}

	if (pid.err < (-6) || pid.ActualSpeed > 320)
	{
		if (pid.voltage > 0)
		{
			pid.voltage = 0;
		}
		R_Out_W_Set -= 100;
	}

	R_Out_W_Set += (s16)pid.voltage;
	if ((s16)R_Out_W_Set < 0)
	{
		R_Out_W_Set = 0;
	}
	else if (R_Out_W_Set > R_Temp_PowerMAX)
	{
		R_Out_W_Set = R_Temp_PowerMAX;
	}
	if (pid.B_Temp_state == 1)
	{
		if (R_Out_W_Set < 100)
		{
			if (pid.B_state_i++ > 20)
			{
				pid.B_Temp_state = 2;
			}
		}
	}
}
