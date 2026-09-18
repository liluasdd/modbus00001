#include	"main.h"
#include	"APP_Delay.h"
#include "APP_PWM_int.h"

/*输出PWM设置底层 BEGIN*******/

s16	R_OUT_PWM_data=0;				//PWM占空比0~150


void	OUT_PWM_Wdata(s16* data)
{
	s16	Buck_a,BOOST_b;
	if(*data>d_OUT_PWM_max)
	{
		*data=d_OUT_PWM_max;
	}
	else	if(*data<0)
	{
		*data=0;
	}
	if(*data>=d_OUT_PWM_Period)//升压
	{
		Buck_a = d_OUT_PWM_Period;
		BOOST_b = d_OUT_PWM_SUM-*data;
	}
	else
	{
		BOOST_b = d_OUT_PWM_Period;
		Buck_a = *data;
	} 
    if(Buck_a>d_OUT_PWM_Period)
		Buck_a=d_OUT_PWM_Period;
	
	Set_OUT_Buck_PWM_Data(Buck_a);
	Set_OUT_BOOST_PWM_Data(BOOST_b);
}
void	PWM_ON(void)
{
	MOSFET_DRIVE_ENABLE();
}

void	PWM_OFF(void)
{
	while(R_OUT_PWM_data)
	{
		R_OUT_PWM_data--;
		OUT_PWM_Wdata(&R_OUT_PWM_data);
		Delay_Nus(1);
	}
	
	Set_OUT_Buck_PWM_Data(0);//降压为0 //关闭输出
	Set_OUT_BOOST_PWM_Data(d_OUT_PWM_Period);
	R_OUT_PWM_data=0;
	MOSFET_DRIVE_DISABLE();
}

/*输出PWM设置底层 END*******/


/////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

//#ifdef d_BAT_Boost_Buck_MODE
///*充电PWM设置底层 BEGIN*******/

//s16	R_BAT_PWM_data=0;
//u8	B_BAT_PWM_EN=0;

//void	BAT_PWM_Wdata(s16* data)
//{
//	if(!B_BAT_PWM_EN)
//	{
//		*data=0;
//		Set_BAT_BOOST_PWM_Data(0);
//		Set_BAT_Buck_PWM_Data(0);
//		return;
//	}
//	if(*data>d_BAT_PWM_max)
//	{
//		*data=d_BAT_PWM_max;
//	}
//	else	if(*data<0)
//	{
//		*data=0;
//	}
//	if(*data>d_BAT_PWM_Period)
//	{
//		Set_BAT_Buck_PWM_Data(d_BAT_PWM_Period);
//		Set_BAT_BOOST_PWM_Data(*data-d_BAT_PWM_Period);
//	}
//	else
//	{
//		Set_BAT_BOOST_PWM_Data(0);
//		Set_BAT_Buck_PWM_Data(*data);
//	}	
//}

//void	BAT_PWM_EN(u8	B)
//{
//	if(B_BAT_PWM_EN!=B)
//	{
//		B_BAT_PWM_EN=B;
//		R_BAT_PWM_data=0;
//		Set_BAT_BOOST_PWM_Data(0);
//		Set_BAT_Buck_PWM_Data(0);
//	}
//}

/*充电PWM设置底层 END*******/
//#endif

/*背光PWM设置底层 BEGIN*******/
u16	R_BL_PWM_Data=0;
void	BL_PWM_Wdata(s16 data)
{
	R_BL_PWM_Data=data+d_BL_PWM_min;;
}
void	BL_PWM_OFF(void)
{
	R_BL_PWM_Data=0;
}
/*背光PWM设置底层 END*******/
