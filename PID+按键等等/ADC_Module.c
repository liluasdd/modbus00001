#include "main.h"
#include "main_2.h"
#include "APP_Delay.h"

//总电池电压计算
u16	ReadBATz_Data(void)
{
	u16	d1;	
	d1=ADC_TDx_Data(ADC_CHANNEL_BATH,12);
    u16 R=ADC_TDx_Data_1C(ADC_CONV_CHANNEL_17);
    if(R>1595)
    {
       R=ADC_TDx_Data(ADC_CONV_CHANNEL_17,12);
	   // 计算电池电压  ADC_REF是内部参考电压1.2V
	   // 电压 = 电压参考 * 2 / (R1 + R2) * 电压
       R_BAT_L_Real=(u16)((u32)d1*((u32)ADC_REF*333u/4096u)*2u/R); 
    }
    else
    {
        R_BAT_L_Real=JS_BATZ(d1);    
    }
	R_BAT_MAX_Real=R_BAT_L_Real;
	return R_BAT_L_Real;
}

u16	ReadBATz_Data1(void)
{
	u16	d1;    
	d1=ADC_TDx_Data_1C(ADC_CHANNEL_BATH);   
    u16 R=ADC_TDx_Data_1C(ADC_CONV_CHANNEL_17);
    if(R>1595)
    {
       d1=(u16)((u32)d1*((u32)ADC_REF*333u/4096u)*2u/R); 
    }
    else
    {
        d1=JS_BATZ(d1);
    }    
	return d1;
}

//PCB温度计算
s16 ReadTemp_Data(void)
{
    int16_t temp ;
	
	return temp; 
}

//输出电压计算
u16 Read_outV_Data(void)
{
	return JS_outV(ADC_TDx_Data(ADC_CHANNEL_VOUT,12));
}

u16 Read_outV_Data1(void)
{
	return JS_outV(ADC_TDx_Data_1C(ADC_CHANNEL_VOUT));
}

u16 Read_outV_Data4C(void)
{
	return JS_outV(ADC_TDx_Data(ADC_CHANNEL_VOUT,4));
}
//输出电流计算
u16 Read_outI_Data(void)
{
	return JS_outI(ADC_TDx_Data(ADC_CHANNEL_IOUT,12));
}

//输出电流计算
u16 Read_outI_Data_1C(void)
{
	return JS_outI(ADC_TDx_Data_1C(ADC_CHANNEL_IOUT));
}

//输出电流计算
u16 Read_outI_Data_4C(void)
{
	return JS_outI(ADC_TDx_Data(ADC_CHANNEL_IOUT,4));
}

//负载电阻计算
u32	R_ReadLoad_outV;
u32	R_ReadLoad_outI;
u32	R_ReadLoad_outV_Q;
u32	R_ReadLoad_outI_Q;
u16 ReadLoad_Data(void)
{
	u32	a;
	u32	b;
	u32	c;
	u32 data1;
	R_ReadLoad_outV=ADC_TDx_Data(ADC_CHANNEL_VOUT,12);
	R_ReadLoad_outI=ADC_TDx_Data(ADC_CHANNEL_IOUT,12);
	if(R_ReadLoad_outI==0)
	{
		data1=20000;
	}
	else
	{
		data1=(R_ReadLoad_outV*200/(R_ReadLoad_outI));
	}
   

	R_ReadLoad_outI=(JS_outI(R_ReadLoad_outI));
	a=JS_outV(R_ReadLoad_outV);


    if(data1>5000)
		data1=20000;
	data1=data1*104/100;	
	// 	if(data1>500)
	// 	{
	// 		data1-=15;
	// 	}
		
	// if(data1>4500)
	// {
	// 	data1=data1*85/100;
	// }
	// else if(data1>4000)
	// {
	// 	data1=data1*87/100;
	// }
	// else if(data1>3000)
	// {
	// 	data1=data1*90/100;
	// }
	// else if(data1>2500)
	// {
	// 	data1=data1*93/100;
	// }
	// else if(data1>1700)
	// {
	// 	data1=data1*95/100;
	// }
	
    R_ReadLoad_outV_Q=a;
    R_ReadLoad_outI_Q=R_ReadLoad_outI;
	b=a-R_ReadLoad_outI*10/2000;
	c=a*10/12; //90%
	if(c>b)
		R_ReadLoad_outV=c;
	else
		R_ReadLoad_outV=b;
	if((s32)R_ReadLoad_outV<0)
		R_ReadLoad_outV=0;
//	
    
	return (u16)(data1);
}

u16 Read_PCB_ADC(void)
{
  return ADC_TDx_Data(ADC_CHANNEL_PCBTEMP,12);
}


// u16 Read_CHARGE_TEMP(void)
// {
//   return ADC_TDx_Data(ADC_CHANNEL_PCBTEMP0,12);
// }


// u16 Read_BAT_TEMP(void)
// {
//   return ADC_TDx_Data(ADC_CHANNEL_PCBTEMP1,12);
// }
//USB端电压计算
u16 Read_USB_OVP_Data1(void)
{
	return JS_USBOVP(ADC_TDx_Data_1C(ADC_CHANNEL_OVP));
}

//USB端电压计算
    

u16 Read_USB_OVP_Data(void)
{
//    u16 R_VCC;
    u16 R_USB;
    u16 R=ADC_TDx_Data(ADC_CONV_CHANNEL_17,12);
    R_USB=ADC_TDx_Data(ADC_CHANNEL_OVP,12);
    if(R>1595)//VCC＜3.3V
    {
        return ((u16)((u32)R_USB*((u32)ADC_REF*333u/4095u)*11u/R));
    }
    else
    {
        return JS_USBOVP(R_USB);
    }
	
}
//充电电流计算
u16 Read_ChargIV_Data(void)
{
  return 0;//return JS_ChargI(ADC_TDx_Data(ADC_ChargIGPIO));
}

//VCC电压计算
u16 Read_VCC_Data(void)
{

	return	330; 
}

//光敏二极管ADC
u16 Read_LUX_Data(void)
{
	//return	ADC_TDx_Data(ADC_LUXGPIO);
	return 0;
}



