#include "APP_Sleeping.h"
#include "main_2.h"
#include "main.h"
#include "WM_Disp.h"

u16	R_SleepTim=300;
u8 WakeUp_Clear_UI=0;
u8 Sleep_Before_Res=0;

volatile u8 B_WakeUp_Source=0;
u8 B_WakeUp_USB=0;

#define	d_SleepTim_max	200

void (*Disp_Sleep)(void)=Disp_Sleep_NOP;				//鏄剧ず杩涘叆浼戠湢鏃剁殑閽╁瓙鍑芥暟
void	Disp_Sleep_NOP(void)
{
	__NOP();
}


vu8	B_fast_Sleep=0;
u8	B_USB_inquiry=0;

void Wake_ADC_Atomizer_Status(void)
{
      u16 data=0;  // 存储ADC采样结果
        
      wakeup_adc_init();      // 初始化ADC用于唤醒检测
      data=ADC_TDx_Data_XC(ADC_CHANNEL_VOUT);     // 读取雾化器输出通道的ADC值
            // 判断条件1：ADC值大于500 且 平均阻值不是9999（9999代表未接入雾化器）
      // 说明已经接入雾化器，触发唤醒
      if(data>500&&(R_Res_average!=9999))
      {
        B_WakeUp_Source=1;
      }
      else if(data<30&&(R_Res_average==9999))      // 判断条件2：ADC值小于30 且 平均阻值是9999（未接入雾化器）      // 说明刚刚接入雾化器，触发唤醒
      {
        B_WakeUp_Source=1;
      }
//    __RCU_PERI_RST_ENABLE(ADC);
//    __RCU_PERI_RST_DISABLE(ADC);
//    __RCU_APB2_CLK_DISABLE(RCU_APB2_PERI_ADC);  
    gpio_mode_set(WakeUp_GPIO_Port,WakeUp_Pin,GPIO_MODE_IN_FLOAT);  // 检测完成后，将唤醒引脚设置为浮空输入，等待外部中断唤醒
   
}

// static u8 count=0;
void	Sleeping_Peripheral(void)
{	
	TFT_Sleep();	
    Exit_ADC_Func();
	Exit_DMA_Func();
    Exit_SPI_Func();
	Exit_Timer_Func();
// /////////////////////////配置模拟口////////////////////////////////////////////// 
	gpio_mode_set(GPIOA,GPIO_PIN_ALL,GPIO_MODE_OUT_PP(GPIO_SPEED_LOW));
	gpio_mode_set(GPIOB,GPIO_PIN_ALL,GPIO_MODE_OUT_PP(GPIO_SPEED_LOW));
	gpio_mode_set(GPIOC,GPIO_PIN_ALL,GPIO_MODE_OUT_PP(GPIO_SPEED_LOW));
	gpio_mode_set(GPIOF,GPIO_PIN_ALL,GPIO_MODE_OUT_PP(GPIO_SPEED_LOW));	
	GPIOA->DO&=~GPIO_PIN_ALL;
	GPIOB->DO&=~GPIO_PIN_ALL;
	GPIOC->DO&=~GPIO_PIN_ALL;
	GPIOF->DO&=~GPIO_PIN_ALL;
	
   //模拟口
    gpio_mode_set(SEN_BAT_GPIO_Port,SEN_BAT_Pin,GPIO_MODE_IN_FLOAT);
    gpio_mode_set(SEN_NTCIN_GPIO_Port,SEN_NTCIN_Pin,GPIO_MODE_IN_FLOAT);
//    gpio_mode_set(SEN_NTCIN_GPIO_Port1,SEN_NTCIN_Pin1,GPIO_MODE_IN_FLOAT);
//	gpio_mode_set(SEN_NTCIN_GPIO_Port2,SEN_NTCIN_Pin2,GPIO_MODE_IN_FLOAT);
	gpio_mode_set(SEN_VOUT_GPIO_Port,SEN_VOUT_Pin,GPIO_MODE_IN_FLOAT);
	gpio_mode_set(SEN_IOUT_GPIO_Port,SEN_IOUT_Pin,GPIO_MODE_IN_FLOAT);
//////////////配置输出高////////////////////////////
    gpio_mode_set(PWM_TFT_BL_GPIO_Port,PWM_TFT_BL_Pin,GPIO_MODE_OUT_PP(GPIO_SPEED_LOW));
     PWM_TFT_BL_GPIO_Port->DO|=PWM_TFT_BL_Pin;

   ////////////配置输出低////////////////////////////// /////////////////////////////////////////////////
     
	//driver_en
	gpio_mode_set(EN_MOSFET_GPIO_Port,EN_MOSFET_Pin,GPIO_MODE_OUT_PP(GPIO_SPEED_HIGH));
    MOSFET_DRIVE_DISABLE();
	
    //输出脚
     gpio_mode_set(PWM_BUCK_GPIO_Port,PWM_BUCK_Pin,GPIO_MODE_OUT_PP(GPIO_SPEED_LOW));
     gpio_mode_set(PWM_BOOST_GPIO_Port,PWM_BOOST_Pin,GPIO_MODE_OUT_PP(GPIO_SPEED_LOW));
	 PWM_BUCK_GPIO_Port->DO&=~PWM_BUCK_Pin;
	 PWM_BOOST_GPIO_Port->DO&=~PWM_BOOST_Pin;
    //5V-EN
     gpio_mode_set(EN_12V_GPIO_Port,EN_12V_Pin,GPIO_MODE_OUT_PP(GPIO_SPEED_LOW));
     BOOST_12V_DISABLE();

	//电池地
     gpio_mode_set(EN_BAT_GND_GPIO_Port,EN_BAT_GND_pin,GPIO_MODE_OUT_PP(GPIO_SPEED_LOW));
     EN_BAT_GND_DISABLE();

	 //充电使能脚
//     gpio_mode_set(EN_CHG_GPIO_Port,EN_CHG_Pin,GPIO_MODE_OUT_PP(GPIO_SPEED_LOW));
     if(!B_USB_Low)
        CHG_ENABLE();	
     else
     {
        CHG_DISABLE(); 
     }
    //SDA//SCL//DC//CS//RESET	
    gpio_mode_set(TFT_SDA_GPIO_PORT,TFT_SDA_PIN,GPIO_MODE_OUT_PP(GPIO_SPEED_LOW));
    TFT_SDA_L();
    gpio_mode_set(TFT_SCLK_GPIO_PORT,TFT_SCLK_PIN,GPIO_MODE_OUT_PP(GPIO_SPEED_LOW));	
    TFT_SCLK_L();
    gpio_mode_set(TFT_DC_GPIO_PORT,TFT_DC_PIN,GPIO_MODE_OUT_PP(GPIO_SPEED_LOW));
    TFT_DC_L();
    gpio_mode_set(TFT_NSS_GPIO_PORT,TFT_NSS_PIN,GPIO_MODE_OUT_PP(GPIO_SPEED_LOW));	
    TFT_NSS_L();
    gpio_mode_set(TFT_RESET_GPIO_PORT,TFT_RESET_PIN,GPIO_MODE_OUT_PP(GPIO_SPEED_LOW));
    TFT_RESET_L();	

	//FLASH
    gpio_mode_set(FLASH_MISO_GPIO_Port,FLASH_MISO_Pin,GPIO_MODE_OUT_PP(GPIO_SPEED_LOW));	
    FLASH_MISO_GPIO_Port->DO&=~FLASH_MISO_Pin;
	gpio_mode_set(FLASH_CS_GPIO_Port,FLASH_CS_Pin,GPIO_MODE_OUT_PP(GPIO_SPEED_LOW));	
    FLASH_CS_GPIO_Port->DO&=~FLASH_CS_Pin;
    ////////////////////////输入口//////////////////////////////////////
	//下载口
    gpio_mode_set(GPIOA,GPIO_PIN_13,GPIO_MODE_IN_PD);
    gpio_mode_set(GPIOA,GPIO_PIN_14,GPIO_MODE_IN_PD);		
     
	//充满脚
    //	gpio_mode_set(CHG_Check_GPIO_PORT,CHG_Check_PIN,GPIO_MODE_IN_PU); 
    ////////////////////////唤醒源配置//////////////////////////////////////
	//按键
    gpio_mode_set(KEY_1_GPIO_Port,KEY_1_Pin,GPIO_MODE_IN_FLOAT);
    gpio_mode_set(KEY_2_GPIO_Port,KEY_2_Pin,GPIO_MODE_IN_FLOAT);
	gpio_mode_set(KEY_3_GPIO_Port,KEY_3_Pin,GPIO_MODE_IN_FLOAT);
	// gpio_mode_set(WakeUp_GPIO_Port,WakeUp_Pin,GPIO_MODE_IN_PD);

    //	gpio_mode_set(KEY_4_GPIO_Port,KEY_4_Pin,GPIO_MODE_IN_FLOAT);
	//usb
    gpio_mode_set(SEN_USB_ADC_GPIO_Port,SEN_USB_ADC_Pin,GPIO_MODE_IN_FLOAT);


    __RCU_APB1_CLK_ENABLE(RCU_APB1_PERI_PMU);
    __RCU_APB2_CLK_ENABLE(RCU_APB2_PERI_SYSCFG);


     syscfg_exti_line_config(SYSCFG_EXTI_PORT_PA, SYSCFG_EXTI_PIN_1);
	 syscfg_exti_line_config(SYSCFG_EXTI_PORT_PA, SYSCFG_EXTI_PIN_6);
	 syscfg_exti_line_config(SYSCFG_EXTI_PORT_PB, SYSCFG_EXTI_PIN_2);
    //  syscfg_exti_line_config(SYSCFG_EXTI_PORT_PF, SYSCFG_EXTI_PIN_0);
    
    __EXTI_FLAG_CLEAR(EXTI_LINE_1);     
    __EXTI_FLAG_CLEAR(EXTI_LINE_2);
    __EXTI_FLAG_CLEAR(EXTI_LINE_6);
	// __EXTI_FLAG_CLEAR(EXTI_LINE_0);
	
    __EXTI_EDGE_ENABLE(EXTI_EDGE_RISING, EXTI_LINE_1);
	__EXTI_EDGE_ENABLE(EXTI_EDGE_FALLING, EXTI_LINE_1);
    
    __EXTI_EDGE_ENABLE(EXTI_EDGE_RISING, EXTI_LINE_2);
	__EXTI_EDGE_ENABLE(EXTI_EDGE_FALLING, EXTI_LINE_2);
	
	__EXTI_EDGE_ENABLE(EXTI_EDGE_RISING, EXTI_LINE_0);
	// __EXTI_EDGE_ENABLE(EXTI_EDGE_FALLING, EXTI_LINE_0);
	
	__EXTI_EDGE_ENABLE(EXTI_EDGE_RISING, EXTI_LINE_6);
	__EXTI_EDGE_ENABLE(EXTI_EDGE_FALLING, EXTI_LINE_6);
	
     __EXTI_INTR_ENABLE(EXTI_LINE_1);	
     __EXTI_INTR_ENABLE(EXTI_LINE_6);
	//  __EXTI_INTR_ENABLE(EXTI_LINE_0);
	 __EXTI_INTR_ENABLE(EXTI_LINE_2);
	 
    nvic_config_t nvic_config_struct;
    nvic_config_struct.enable_flag = ENABLE;
    nvic_config_struct.IRQn = IRQn_EXTI0_1;
    nvic_config_struct.priority = 0;
    nvic_init(&nvic_config_struct);
	
     nvic_config_struct.enable_flag = ENABLE;
     nvic_config_struct.IRQn = IRQn_EXTI2_3;
     nvic_config_struct.priority = 0;
     nvic_init(&nvic_config_struct);
	
	 nvic_config_struct.enable_flag = ENABLE;
    nvic_config_struct.IRQn = IRQn_EXTI4_15;
    nvic_config_struct.priority = 0;
    nvic_init(&nvic_config_struct);
	
    WakeUp_Clear_UI=0;
    B_WakeUp_Source=0;
	Sleep_Before_Res=0;
    B_WakeUp_USB=0;
	feed_Watchdog();
	 __RCU_AHB_CLK_DISABLE(RCU_AHB_PERI_GPIOA|RCU_AHB_PERI_GPIOB|RCU_AHB_PERI_GPIOC|RCU_AHB_PERI_GPIOF);	
	 __RCU_APB2_CLK_DISABLE(RCU_APB2_PERI_DBG);
//	count=0;
  
    while(!B_WakeUp_Source)
    {

//    //////////////////////////////////////////////////////////////
//   ///< 进入低功耗模式——深度休眠（使能唤醒后退出中断自动休眠特性）
        pmu_deep_sleep_mode_enter(PMU_LDO_LOW_POWER, PMU_DSM_ENTRY_WFI); 
		feed_Watchdog();        
		wakeup_adc_init();
        Wake_ADC_Atomizer_Status();        
//        bsp_adc_init();
		//////////////////////////////////
		// count++;
		// if(count>=2)
		// {
		// 	count=0;
		//     Update_Sevenday_Number();
		// }
		// //////////////////////////////
        Delay_Nus(60);
        u16 usb_v;
		s32 data=ADC_TDx_Data_XC(ADC_CHANNEL_OVP);

		if(data>=0&&!B_WakeUp_Source)
		{
            u16 R=ADC_TDx_Data_XC(ADC_CONV_CHANNEL_17);
            if(R>1595)
            {
                usb_v=((data*125*11)/R);
            }
            else
            {
                usb_v=JS_USBOVP(data);
            }
            if(usb_v>330)
            {
                if(!B_USB_RinBit)
                {
                    B_WakeUp_Source=1;
                }
                if(!B_USB_Low&&(usb_v<420||usb_v>d_USB_error_H))
                {
                    B_WakeUp_Source=1;
                }
                else if(B_USB_Low&&usb_v>450&&(usb_v<d_USB_error_H-20))
                {
                    B_WakeUp_Source=1;
                }
            }
            else
            {
                if(B_USB_RinBit)
                {
                    B_WakeUp_Source=1;
                }            
            }

	    }
	   __RCU_PERI_RST_ENABLE(ADC);
	   __RCU_PERI_RST_DISABLE(ADC);
		__RCU_APB2_CLK_DISABLE(RCU_APB2_PERI_ADC);
		 gpio_mode_set(SEN_USB_ADC_GPIO_Port,SEN_USB_ADC_Pin,GPIO_MODE_OUT_PP(GPIO_SPEED_LOW));
            SEN_USB_ADC_GPIO_Port->DO&=~SEN_USB_ADC_Pin;

    }

   __EXTI_INTR_DISABLE(EXTI_LINE_0);
   __EXTI_INTR_DISABLE(EXTI_LINE_1);
   __EXTI_INTR_DISABLE(EXTI_LINE_2);
   __EXTI_INTR_DISABLE(EXTI_LINE_6);
	
   feed_Watchdog();
   SystemClock_Config();                       //配置系统时钟
  // SysTick_Config(SystemCoreClock/1000);
   __RCU_AHB_CLK_ENABLE(RCU_AHB_PERI_GPIOA);
   __RCU_APB2_CLK_ENABLE(RCU_APB2_PERI_DBG);
   gpio_mode_set(GPIOA,GPIO_PIN_13, GPIO_MODE_MF_PP(GPIO_SPEED_HIGH));
   gpio_mode_set(GPIOA,GPIO_PIN_14, GPIO_MODE_MF_PP(GPIO_SPEED_HIGH));
   gpio_mf_config(GPIOA,GPIO_PIN_13, GPIO_MF_SEL0);
   gpio_mf_config(GPIOA,GPIO_PIN_14, GPIO_MF_SEL0);
}


u8 B_Sleep_YES_Q=0;
void	Sleeping_Module(void)
{

	if(R_Bright_screen_Time || (B_BAT_Charge&&(R_BattL_Proportion<100||!B_BAT_Congman))||B_BAT_OutCharge)
	{
		if(B_System_off!=d_System_Normal)
		{
			if(!First_Power_On)
               R_SleepTim=10;
			else
				R_SleepTim=5;
		}
		else
		{
			if(R_BATT_Actual<(d_Bat_OUT_Volt_min-20) && !R_XY_Tim_yanshi)
				B_fast_Sleep=1;
			else if(R_BATT_Actual>(d_Bat_OUT_Volt_min-5))
				B_fast_Sleep=0;
			if(B_fast_Sleep && !B_BAT_Charge)
			{
				if(R_Bright_screen_Time+10<R_Bright_screen_TimeMAX)
					R_Bright_screen_Time=0;
                
				if(B_USB_RinBit)
					R_SleepTim=d_SleepTim_max;
				else
					R_SleepTim=10;
			}
			else
			{
					R_SleepTim=10;
			}
		}
	}
//    
	// if(B_USB_RinBit||B_USB_Low)
	// 	return;
	if(!R_SleepTim)
	{
       if(R_Res_Initial>=d_Res_min && R_Res_Initial<d_Res_max && B_Sleep_YES!=1 && !B_BAT_error)
		{
			B_Sleep_YES=1;
			Enable_VoutADC_Use();
			R_Res_Sleep=Read_L_1C();
		}
        
        if(Disp_windows==Warning_interface||Disp_windows==One_Power_interface)
        {
            Disp_windows=Main_interface;
            Disp_windows_Set=Disp_windows;
        }
        Sleeping_Peripheral();
        R_SleepTim=20;								//休眠时间寄存器赋值
        R_Skinning_mode=Skinning_init;
        B_KEY_JC=1;
        B_Again_Read=1;
        R_Read_Rtim=10;
        B_Sleep_YES_Q=B_Sleep_YES;
        B_Sleep_YES=0;
		Sleep_Before_Res=1;
		Timer6_Init();
//		WDT_Config();
        feed_Watchdog();
        bsp_gpio_init();
        bsp_adc_init();	
        bsp_spi_init(); 
        bsp_dma_init();		
        BL_Timer3_Init();
        Timer14_Init();		
        PWM_Timer1_Init();
        IP6106_Init();
//		RTC_Config();
		// CX25890H_IIC_Init();
        WakeUp_Clear_UI=1;
		TFT_LCD_RESET();
        if(First_Power_On)
		{
			CHG_DISABLE();
			Delay_Nus(5);
			ReadBATz_Data();
			CHG_ENABLE();
					
			if(!R_XY_Tim_yanshi)
			{
				if(R_BATT_L_Actual>R_BAT_L_Real || R_BATT_L_Actual<R_BAT_L_Real-5)
				{
					if(R_BAT_L_Real>240 && R_BAT_L_Real<450)
					{
						B_BAT_error=0;
						B_Smoking_mark_Q=0;
					}
					if(R_BAT_L_Real>300) R_BAT_L_Real-=2;
					R_BATT_L_Actual=R_BAT_L_Real;
					B_Proportion_Init=0;
					Batt_Proportion();
				}	
			}
		}
    //    if(R_BattL_Proportion)
    //    {
    //        IP5355_IIC_Write(0x80, 0x22); // 开启手机充电
    //        IP5355_IIC_Write(0x40, 0x22);                     
    //    }
		TFT_ST7789S_Init();
        //Watchdog_ON(); 
        Enable_VoutADC_Use();
        if(B_Smoking_KEY_anxia)
        {
            B_KEY_JC = 0;				//极限秒吸 极限一换一 先不初始化		
        }   
        if(R_Skinning_mode==Key_UnLock)
        {
             R_Skinning_mode=Skinning_init;
        }
        WakeUp_Check_USB();
        R_SleepTim=20;								//休眠时间寄存器赋值
         B_Sleep_YES=B_Sleep_YES_Q;
		 First_Power_On=0;
	}

}

	

