#include "APP_Sleeping.h"
static void APP_EnterStop(void);
u8 Flag_ResWake;

// void Scrren_Brightness(void)
//{
//   R_Screen_Adjust-=1;
//   SetBrighness(R_Screen_Adjust);
// }
// void System_ON_OFF_Time(void)
//{
//	if(Flag_SystemONOFF)
//	{
//	  tick.timeout = OutTimeMain;
//	}
//	else
//	{
//	 tick.timeout = OutPowerOff;
//	}
// }

extern u8 R_Load_num_tmp;
extern u8 key_Sleep_num;
u8 READ_USB_T_STATUS_num = 0;
u8 syssleep_num = 0;
u8 syssleep_num1 = 0;
u8 syssleep_num_tmp = 0;
void syssleep(void) // 进入睡眠状态
{
    // u8 syssleep_num_tmp1 = 0;
    u8 KEY_1_STATUS_kajian = 0;
    u8 usb_slp_error_tim = 0;
    Write_Command(0xAE);                         // 关闭OLED
    Clear_Screen();                              // 清除屏幕
    Flag_Key.LongPress = 1;                      // 长按按键
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};   // 初始化GPIO结构体
    HAL_SuspendTick();                           // 禁用SysTick中断
    DBGMCU->CR = 0;                              // 禁用调试模式(原样保留)
    __HAL_RCC_ADC1_FORCE_RESET();                // 强制复位ADC1(APB2)
    __HAL_RCC_ADC1_RELEASE_RESET();              // 释放ADC1复位位
    __HAL_TIM_DISABLE_IT(&htim3, TIM_IT_UPDATE); // 禁用TIM3更新中断(F103无TIM14)
    __HAL_TIM_DISABLE(&htim3);                   // 停止TIM3计数
    __HAL_RCC_TIM3_FORCE_RESET();                // 强制复位TIM3
    __HAL_RCC_TIM3_RELEASE_RESET();              // 释放TIM3复位位
    __HAL_RCC_TIM3_CLK_DISABLE();                // 禁用TIM3时钟
    __HAL_RCC_ADC1_CLK_DISABLE();                // 禁用ADC1时钟

    // LL_SYSTICK_DisableIT();                               // 禁用SysTick中断
    // DBGMCU->CR = 0;                                       // 禁用调试模式
    // LL_APB1_GRP2_ForceReset(LL_APB1_GRP2_PERIPH_ADC1);    // 强制复位ADC1
    // LL_APB1_GRP2_ReleaseReset(LL_APB1_GRP2_PERIPH_ADC1);  // 释放ADC1复位位
    // LL_TIM_DisableIT_UPDATE(TIM14);                       // 禁用TIM14中断
    // LL_APB1_GRP2_ForceReset(LL_APB1_GRP2_PERIPH_TIM14);   // 强制复位TIM14
    // LL_APB1_GRP2_ReleaseReset(LL_APB1_GRP2_PERIPH_TIM14); // 释放TIM14复位位
    // LL_APB1_GRP2_DisableClock(LL_APB1_GRP2_PERIPH_TIM14); // 禁用TIM14时钟
    // LL_APB1_GRP2_DisableClock(LL_APB1_GRP2_PERIPH_ADC1);  // 禁用ADC1时钟

    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    // GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;

    GPIO_InitStruct.Pin = SEN_RESL_PIN; // A3
    LL_GPIO_Init(SEN_RESL_GPIO, &GPIO_InitStruct);
    WAKEUP_GPIO->BSRR = SEN_RESL_PIN;
    __delay_us(100);
    GPIO_InitStruct.Pin = PWM_BUCK1_PIN; // A5
    LL_GPIO_Init(PWM_BUCK1_GPIO, &GPIO_InitStruct);
    OUT_PWM_BUCK1_OFF();

    GPIO_InitStruct.Pin = OUT_3V_EN_PIN; // A1
    LL_GPIO_Init(OUT_3V_EN_GPIO, &GPIO_InitStruct);
    OUT_3V_EN_OFF();

    GPIO_InitStruct.Pin = RT_RES_PIN; // A6
    LL_GPIO_Init(RT_RES_GPIO, &GPIO_InitStruct);
    RT_RES_OFF();

    GPIO_InitStruct.Pin = OLED_REST_PIN; // A0
    LL_GPIO_Init(OLED_REST_GPIO, &GPIO_InitStruct);
    OLED_REST_OFF();

    GPIO_InitStruct.Pin = OLED_SDA_PIN; // B1
    LL_GPIO_Init(OLED_SDA_GPIO, &GPIO_InitStruct);
    OLED_SDA_L();

    GPIO_InitStruct.Pin = OLED_SCK_PIN; // B0
    LL_GPIO_Init(OLED_SCK_GPIO, &GPIO_InitStruct);
    OLED_SCL_L();

    // 充电控制引脚
    GPIO_InitStruct.Pin = CHARGE_EN_PIN; // B3
    LL_GPIO_Init(CHARGE_EN_GPIO, &GPIO_InitStruct);
    if (Flag.Adapt_Err == 0)
        CHARGE_EN_GPIO->BRR = CHARGE_EN_PIN; // 低电平 // 开
    else
        CHARGE_EN_GPIO->BSRR = CHARGE_EN_PIN; // 高电平  // 关

    //     GPIO_InitStruct.Pin = CHARGE_EN_PIN;
    //	LL_GPIO_Init(CHARGE_EN_GPIO, &GPIO_InitStruct);
    //    CHARGE_EN_ON();

    //     GPIO_InitStruct.Pin = CHARGE_CHECK_PIN;
    //	LL_GPIO_Init(CHARGE_CHECK_GPIO, &GPIO_InitStruct);
    //    CHARGE_CHECK_GPIO->BRR =CHARGE_CHECK_PIN;

    //////======空闲引脚配置
    GPIO_InitStruct.Pin = LL_GPIO_PIN_0; // C0
    LL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    GPIOC->BRR = LL_GPIO_PIN_0; // 低电平

    GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG; // 模拟输入
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;

    /* disable debug_GPIO */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_6; // B6
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = LL_GPIO_PIN_2; // A2
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // USB_ADC引脚
    GPIO_InitStruct.Pin = SEN_USB_T_PIN; // A4
    LL_GPIO_Init(SEN_USB_T_GPIO, &GPIO_InitStruct);

    // // USB_ADC
    // GPIO_InitStruct.Pin = SEN_USB_T_PIN; // A4
    // LL_GPIO_Init(SEN_USB_T_GPIO, &GPIO_InitStruct);
    // 输出电阻检测 H_ADC 脚

    GPIO_InitStruct.Pin = SEN_RESH_PIN; // A7
    LL_GPIO_Init(SEN_RESH_GPIO, &GPIO_InitStruct);

    /* KEY */
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    // 按键引脚,未卡键
    KEY_1_STATUS_kajian = 0;
    GPIO_InitStruct.Pin = KEY_1_PIN; // B2
    LL_GPIO_Init(KEY_1_GPIO, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = KEY_2_PIN; // B7
    LL_GPIO_Init(KEY_2_GPIO, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = KEY_3_PIN; // C1
    LL_GPIO_Init(KEY_3_GPIO, &GPIO_InitStruct);

    // if (!Flag.Adapt_In) // 未连接
    {
        // 充电满检测引脚
        GPIO_InitStruct.Pin = CHARGE_CHECK_PIN;            // B5
        LL_GPIO_Init(CHARGE_CHECK_GPIO, &GPIO_InitStruct); //
        // if (Flag.Adapt_Err)                                // 高低压
        {
            // 充电状态引脚
            GPIO_InitStruct.Pin = CHARGE_FULL_PIN;            // B4
            LL_GPIO_Init(CHARGE_FULL_GPIO, &GPIO_InitStruct); //
        }
    }

    // if (Flag.Adapt_In) // 连接
    // {
    //     GPIO_InitStruct.Pin = SEN_USB_T_PIN; // A4
    //     LL_GPIO_Init(SEN_USB_T_GPIO, &GPIO_InitStruct);
    // }

    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    //    GPIO_InitStruct.Pin = KEY_MIC_PIN;
    //    LL_GPIO_Init(KEY_MIC_GPIO, &GPIO_InitStruct);

    // // 充电控制引脚
    // GPIO_InitStruct.Pin = CHARGE_EN_PIN; // B3
    // LL_GPIO_Init(CHARGE_EN_GPIO, &GPIO_InitStruct);

    // __delay_us(100);
    // if (Flag.Adapt_In) // 连接
    // {
    //     if (Flag.Charger_Over == 0 && Flag.Adapt_Err == 2) // 没充满+低压
    //     {
    //         // 充电满检测引脚
    //         GPIO_InitStruct.Pin = CHARGE_CHECK_PIN;            // B5
    //         LL_GPIO_Init(CHARGE_CHECK_GPIO, &GPIO_InitStruct); //
    //     }
    //     if (Flag.Adapt_Err) // 高低压
    //     {
    //         // 充电状态引脚
    //         GPIO_InitStruct.Pin = CHARGE_FULL_PIN;            // B4
    //         LL_GPIO_Init(CHARGE_FULL_GPIO, &GPIO_InitStruct); //
    //     }
    // }

    if (Flag.Charger_Over == 1) // 充电满
    {
        // 充电状态引脚
        GPIO_InitStruct.Pin = CHARGE_FULL_PIN;            // B4
        LL_GPIO_Init(CHARGE_FULL_GPIO, &GPIO_InitStruct); //
    }
    // // USB_ADC引脚
    // if (!Flag.Adapt_In) // 未连接
    {
        // GPIO_InitStruct.Pin = SEN_USB_T_PIN; // A4
        // LL_GPIO_Init(SEN_USB_T_GPIO, &GPIO_InitStruct);
    }

    GPIO_InitStruct.Pin = WAKEUP_PIN; // A3
    LL_GPIO_Init(WAKEUP_GPIO, &GPIO_InitStruct);
    // __delay_us(100);

    if (KEY_1_STATUS == 0) // 卡键休眠 处理
    {
        KEY_1_STATUS_kajian = 1;
        GPIO_InitStruct.Pin = KEY_1_PIN; // B2
        LL_GPIO_Init(KEY_1_GPIO, &GPIO_InitStruct);
    }
    if (KEY_2_STATUS == 0) // 卡键休眠 处理
    {
        GPIO_InitStruct.Pin = KEY_2_PIN; // B7
        LL_GPIO_Init(KEY_2_GPIO, &GPIO_InitStruct);
    }
    if (KEY_3_STATUS == 0) // 卡键休眠 处理
    {
        GPIO_InitStruct.Pin = KEY_3_PIN; // C1
        LL_GPIO_Init(KEY_3_GPIO, &GPIO_InitStruct);
    }

    __delay_us(200);

    LL_EXTI_InitTypeDef EXTI_InitStruct = {0};
    // EXTI_InitStruct.Line = LL_EXTI_LINE_1 | LL_EXTI_LINE_2 | LL_EXTI_LINE_6 | LL_EXTI_LINE_7; // PB2--CHG  PB0-MIC   PB5-KEY1  PB1--USB

    EXTI_InitStruct.Line = LL_EXTI_LINE_1 | LL_EXTI_LINE_2 | LL_EXTI_LINE_3 /* | LL_EXTI_LINE_4 | LL_EXTI_LINE_5 */ | LL_EXTI_LINE_7; // PB2--CHG  PB0-MIC   PB5-KEY1  PB1--USB

    EXTI_InitStruct.LineCommand = ENABLE;
    EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
    EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_RISING_FALLING;
    LL_EXTI_Init(&EXTI_InitStruct);

    LL_EXTI_SetEXTISource(LL_EXTI_CONFIG_PORTC, LL_EXTI_CONFIG_LINE1); //
    LL_EXTI_SetEXTISource(LL_EXTI_CONFIG_PORTB, LL_EXTI_CONFIG_LINE2);

    LL_EXTI_SetEXTISource(LL_EXTI_CONFIG_PORTA, LL_EXTI_CONFIG_LINE3); // 雾化器
    // LL_EXTI_SetEXTISource(LL_EXTI_CONFIG_PORTB, LL_EXTI_CONFIG_LINE4);
    // // LL_EXTI_SetEXTISource(LL_EXTI_CONFIG_PORTA, LL_EXTI_CONFIG_LINE4);

    // LL_EXTI_SetEXTISource(LL_EXTI_CONFIG_PORTB, LL_EXTI_CONFIG_LINE5);
    LL_EXTI_SetEXTISource(LL_EXTI_CONFIG_PORTB, LL_EXTI_CONFIG_LINE7);

    LL_EXTI_ClearFlag(LL_EXTI_LINE_1 | LL_EXTI_LINE_2 | LL_EXTI_LINE_3 /* | LL_EXTI_LINE_4 | LL_EXTI_LINE_5 */ | LL_EXTI_LINE_7);
    NVIC_EnableIRQ(EXTI0_1_IRQn);
    NVIC_EnableIRQ(EXTI2_3_IRQn);
    NVIC_EnableIRQ(EXTI4_15_IRQn);

    NVIC_SetPriority(EXTI0_1_IRQn, 0);
    NVIC_SetPriority(EXTI2_3_IRQn, 0);
    NVIC_SetPriority(EXTI4_15_IRQn, 0);

    Flag_System_wakeup = 0;
    Flag_ResWake = 0;

    // R_Puffnum = 0; //口数
    syssleep_num = 0;
    syssleep_num_tmp = 0;
    // syssleep_num1 = 6; // 接入
    // if (READ_USB_T_STATUS == 0)
    // {
    //     syssleep_num1 = 0; // 未接入
    // }
    Init_Lptim_WakeUp();
    // syssleep_num_tmp1 = 0;

System_Sleep:

    if (Flag_System_wakeup)
    {
        goto System_ON;
    }
    if (KEY_1_STATUS == 0 && KEY_1_STATUS_kajian == 0)
    {
        Flag_System_wakeup = 1;
        syssleep_num_tmp = 8;
        syssleep_num = 0;
        key_value = 0x04;
        goto System_ON;
        // R_Key.Data = 0;
    }

    feed_watchdog();
    Set_LT_Data();   // 设置LT数据
    APP_EnterStop(); // 进入休眠状态
    // if (syssleep_num_tmp1 < 200)
    //     syssleep_num_tmp1++;
#if 1
    // if (R_ADAdaptVol_num1)
    // if (syssleep_num_tmp == 4 || syssleep_num_tmp == 5) // 中断4/5
    {
        bsp_adc_init(0); // 初始化ADC
        GP_USB_T_Ang;
        R_ADAdaptVol = usb_adc_handle(adc_tdx_data_1c(ADC_CHANNEL_USB));
        GP_USB_T_Out;
        LL_APB1_GRP2_ForceReset(LL_APB1_GRP2_PERIPH_ADC1);
        LL_APB1_GRP2_ReleaseReset(LL_APB1_GRP2_PERIPH_ADC1);
        LL_APB1_GRP2_DisableClock(LL_APB1_GRP2_PERIPH_ADC1);

        if (Flag.Adapt_In) // 接入
        {
            if (R_ADAdaptVol < 200)
            {
                goto System_ON;
            }
            else
            {
                if (Flag.Adapt_Err)
                {
                    // if ((R_ADAdaptVol > 464 && R_ADAdaptVol < 575) || !CHARGE_CHECK_STATUS)
                    if ((R_ADAdaptVol > Adpt44V && R_ADAdaptVol < Adpt6V) /*|| !GP_USB_M*/)
                    {
                        //                        GP_USB_EN_ON();
                        Flag.Adapt_Err = 0;
                        goto System_ON;
                    }
                }
                // else
                // {
                //     if (R_ADAdaptVol < 444 || R_ADAdaptVol > 600)
                //     {
                //         g_b_irq_wake = 1;
                //     }
                // }
            }
        }
        else
        {
            //			g_b_irq_wake = 1;
            if ((R_ADAdaptVol > 200 && R_ADAdaptVol < Adpt6V) /*|| !GP_USB_M*/)
            {
                // if (!Flag.Charger_Over /*|| !GP_USB_M*/) // 充电完成
                goto System_ON;
                //  g_b_irq_wake = 1;
                // else if (Flag.Adapt_Err)
                // {
                //     Flag.Adapt_Err = 0;
                //     // g_b_irq_wake = 1;
                //     goto System_ON;
                // }
            }
            else
            {
                if (R_ADAdaptVol >= Adpt6V)
                {
                    CHARGE_EN_OFF();
                    if (!Flag.Adapt_Err)
                    {
                        usb_slp_error_tim++;
                        if (usb_slp_error_tim > 1)
                        {
                            Flag.Adapt_Err = 1;
                            usb_slp_error_tim = 0;
                            //                        g_b_ovp_voltage_err = 1;
                            // g_b_irq_wake = 1;
                            //											if(R_ADAdaptVol> 600)     g_b_irq_wake = 1;
                            goto System_ON;
                        }
                    }
                }
                else
                    usb_slp_error_tim = 0;
            }
        }
    }

#endif
    feed_watchdog();

    // if (syssleep_num)
    // if (0)
    // {
    //     __delay_us(200);
    //     if (syssleep_num == 3) // A3  中断3  雾化器唤醒
    //     {
    //         if (WAKEUP_RinBit == 0)
    //         {
    //             // syssleep_num_tmp = 3;
    //             // syssleep_num = 0;
    //             Flag_System_wakeup = 1;
    //         }
    //         else
    //             Flag_System_wakeup = 0;
    //     }
    //     else if (syssleep_num == 7) // B7  key2  中断7 减建
    //     {
    //         if (KEY_2_STATUS == 0)
    //         {
    //             // syssleep_num_tmp = 7;
    //             // syssleep_num = 0;
    //             Flag_System_wakeup = 1;
    //         }
    //         else
    //             Flag_System_wakeup = 0;
    //     }
    //     else if (syssleep_num == 8) // B8  key3  中断8 减建
    //     {
    //         if (KEY_3_STATUS == 0) // C1
    //         {
    //             // syssleep_num_tmp = 8;
    //             // syssleep_num = 0;
    //             Flag_System_wakeup = 1;
    //         }
    //         else
    //             Flag_System_wakeup = 0;
    //     }
    //     if (syssleep_num_tmp)
    //     {
    //         syssleep_num_tmp--;
    //         // syssleep_num = 0;
    //     }
    //     else if (Flag_System_wakeup)
    //     {
    //         syssleep_num_tmp = 3;
    //         syssleep_num = 0;
    //     }
    // }
    // else
    //     syssleep_num_tmp = 3;

    // R_Puffnum = syssleep_num_tmp1;

    if (KEY_1_STATUS == 0 && KEY_1_STATUS_kajian == 0)
    {
        Flag_System_wakeup = 1;
        syssleep_num_tmp = 8;
        syssleep_num = 0;
        key_value = 0x04;
        goto System_ON;
        // R_Key.Data = 0;
    }
    // if (Flag_System_wakeup)
    if (Flag_System_wakeup && syssleep_num == 0)
        goto System_ON;
    else
        goto System_Sleep;
System_ON:

    LL_EXTI_ClearFlag(LL_EXTI_LINE_1 | LL_EXTI_LINE_2 | LL_EXTI_LINE_3 | LL_EXTI_LINE_4 | LL_EXTI_LINE_5 | LL_EXTI_LINE_7);
    /////////////////////////////////
    LL_EXTI_DisableIT(LL_EXTI_CONFIG_LINE1);
    LL_EXTI_DisableIT(LL_EXTI_CONFIG_LINE2);
    LL_EXTI_DisableIT(LL_EXTI_CONFIG_LINE3);
    LL_EXTI_DisableIT(LL_EXTI_CONFIG_LINE4);
    LL_EXTI_DisableIT(LL_EXTI_CONFIG_LINE5);
    LL_EXTI_DisableIT(LL_EXTI_CONFIG_LINE7);
    NVIC_DisableIRQ(EXTI0_1_IRQn);
    NVIC_DisableIRQ(EXTI2_3_IRQn);
    NVIC_DisableIRQ(EXTI4_15_IRQn);
    NVIC_DisableIRQ(LPTIM1_IRQn);
    Set_Wakeup_LT_Data();  // 设置唤醒时间
    systemclock_config();  // 系统时钟配置
    LL_SYSTICK_EnableIT(); // 使能SysTick中断
    SET_SWD_AF0();         // 设置SWD引脚为复用功能0
    bsp_gpio_init();       // 初始化GPIO
    bsp_adc_init(1);       // 初始化ADC
    tim_timer14_init();    // 初始化定时器14
                           //    bsp_output_pwm_init();
    Flag_Lcd_Init = 0;
    Flag.Bat_Check = 1;
    R_Task = D_Task3;
    Get_BatVol(); // 获取电池电压
    QuickEcig();  // 快速ECIG
    Flag_Bat_Per = 1;
    Wakeup_Check_Batt(); // 唤醒检查电池
    R_DisDuty = 0xFF;
    if (!Flag.VDDLow)
    {
        if (!Flag_Out.Smoking)
        {
            Read1C_LoadRes();
            if ((R_Load > R_Load_0 + 100) || (R_Load + 100 < R_Load_0) || (R_Load_0 < ShortLoadVol))
            {
                R_CoilState = D_CoilUnIn;
                for (u8 i = 0; i < 100; i++)
                {
                    feed_watchdog();
                    Get_LoadRes_Out();
                    if (R_CoilState != D_CoilIn)
                    {
                        break;
                    }
                }
            }
        }

        // if (!Flag.Adapt_In && READ_USB_T_STATUS) // A4  中断4 USB_ADC引脚
        // {
        //     Flag_Adapt_FirstIn = 1;
        //     READ_USB_T_STATUS_num = 40;
        //     InChargeProcess();
        //     R_DisDuty = 0;
        // }

        // // 充电满检测引脚
        // if (Flag.Full_One == 1 && Flag_ResWake == 2)
        // {
        //     Flag_ResWake = 0;
        //     tick.timeout = OutTime3S;
        // }

        R_Atomizer_Cnt = 10;
        if (Flag_ResWake == 1) // 未插入适配器
        {
            Flag_ResWake = 0;
            Flag.Buck = 0;
            tick.timeout = 0;
            if (R_Load_num_tmp == 0) // 电阻未接入
            {
                R_Load_num_tmp = 1;
                tick.timeout = OutTime5S;
            }
            // else
            // {
            //     tick.timeout = 0;
            //     R_Load_num_tmp = 0;
            // }
        }
    }
    // if (Flag.VDDLow && (!Flag.Adapt_In && (R_ADAdaptVol > 200 && R_ADAdaptVol < Adpt6V)))
    // {
    //     InMainProcess();    // 主处理
    //     Flag_Power_Off = 0;
    //     Flag_SystemONOFF = 1;
    // }
    // else
    // if (KEY_2_STATUS == 0 || KEY_3_STATUS == 0)
    // {
    //     key_Sleep_num = 16;
    // }
    // if (Flag.VDDLow == 0 || Flag_SystemONOFF) // 非首次开机或低电量
    // {
    //     if (Flag.Adapt_In && R_ADAdaptVol < 200)                                  // USB拔出
    //         BackUpM();                                                            // 检查充电状态
    //     else if (!Flag.Adapt_In && (R_ADAdaptVol > 200 && R_ADAdaptVol < Adpt6V)) //
    //         InMainProcess();                                                      // 主处理
    // }
    // JudgeKey(); // 判断按键

    // else if (!Flag.Adapt_In && (R_ADAdaptVol > 200 && R_ADAdaptVol < Adpt6V))
    // CheckCharing_Compare(); // 检查充电状态
    R_Task = D_Task1;
    // Flag_SleepWake = 1;
}

static void APP_EnterStop(void)
{

    // /* Enable PWR clock */
    // LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

    // /* STOP mode with LPR ON */
    // LL_PWR_SetVoltageRegulatorMode(LL_PWR_VOLTAGE_REGULATOR_MODE_LPR);

    // /* Enter DeepSleep mode */
    // LL_LPM_EnableDeepSleep();

    // /* Request Wait For Event */
    // __SEV();
    // __WFE();
    // __WFE();

    // LL_LPM_EnableSleep();

    /* Enable PWR clock */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

    /* STOP mode with Deep low power regulator ON */
    LL_PWR_SetLprMode(LL_PWR_LPR_MODE_LPR);

    /* SRAM retention voltage aligned with digital LDO output */
    LL_PWR_SetStopModeSramVoltCtrl(LL_PWR_SRAM_RETENTION_VOLT_CTRL_LDO);

    /* Enter DeepSleep mode */
    LL_LPM_EnableDeepSleep();

    /* Request Wait For event */
    __SEV();
    __WFE();
    __WFE();

    LL_LPM_EnableSleep();
}
void Compare_Sleep(void)
{
    if (Flag.Adapt_In && !Flag.Full_One && R_ADAdaptVol_num1 == 0 && !Flag.Adapt_Err)
    {
        InChargeProcess(); // 充电满检测引脚
        return;
    }
    else
    {
        if (!Flag_SystemONOFF)
        {
            if (R_Status != D_ProcessPowerOff) // 系统开启状态
            {
                ProcessPowerOff();
                return;
            }
        }
        syssleep();
    }
}
void Wakeup_Check_Batt(void) // 电池电量检查
{
    if (Flag_OnPower)
    {
        Flag_OnPower = 0;
        Flag.Percent_Init = 0;
        R_DisBatLevelQQ = 0xff;
    }
    // if ((R_Bat.Analog <= Bat32V) || (!R_Bat.Percent)|| (Flag.LowBat && R_Bat.Percent < 10))
    if ((R_Bat.Analog <= Bat32V) || (!R_Bat.Percent) || (Flag.LowBat && R_Bat.Percent < 6))
    {
        R_Bat.Percent = 0;
        R_Bat.Level = 0;
        Flag.LowBat = 1;
    }
    else
    {
        Flag.LowBat = 0;
        //        Flag.Percent_Init=0;
        //        for(i=0;i<6;i++)
        //        {
        Batt_Proportion();
        Batt_Level();
        //        }
        R_DisBatLevel = R_Bat.Level;
        //        R_DisBatLevelQQ=R_DisBatLevel;
    }
}

#if configUSE_TICKLESS_IDLE、

// low_power.c
#include "low_power.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stm32f1xx_hal.h"
#include "modbus.h"         // 需要读 mb_state
#include "periph_manager.h" // 外设关/开

bool CanEnterStop(void)
{
    /* 快速判断，不做浮点、不做循环 */
    if (mb_state != MB_IDLE)
        return false;
    if (has_pending_tx)
        return false;
    if ((HAL_GetTick() - last_rx_tick) < 5)
        return false;
    return true;
}

/* FreeRTOS 空闲钩子 */
void vApplicationIdleHook(void)
{
    /* 注意：这个函数在 Idle 任务上下文中运行 */
    /* Idle 任务栈很小（默认 128 words），别在这里做重活 */

    if (CanEnterStop())
    {
        Enter_Stop_Mode();
        /* 唤醒后从这里继续，然后 Idle 任务继续循环 */
    }

    /* 不能睡时，直接返回，Idle 任务继续跑 */
}

void Periph_Stop_Shutdown(void)
{
    /* ===== 1. 先停 DMA 传输（所有在用的通道）===== */
    // UART RX/TX
    HAL_UART_DMAStop(&huart1);
    // ADC DMA
    HAL_ADC_Stop_DMA(&hadc1);
    // 如果有 TIM 触发 DMA（比如 TIM2->DAC 或 TIM3->DMA）
    // HAL_TIM_DMABurst_Stop(&htim2, ...);  // 按你实际用的加

    /* ===== 2. 停所有定时器 ===== */
    HAL_TIM_Base_Stop_IT(&htim2);
    HAL_TIM_Base_Stop_IT(&htim3);
    HAL_TIM_Base_Stop_IT(&htim4);
    // 如果你用了 PWM/OC/IC 模式，也要停：
    // HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
    // HAL_TIM_IC_Stop(&htim2, TIM_CHANNEL_1);

    /* ===== 3. 停 ADC ===== */
    HAL_ADC_Stop(&hadc1);
    // 如果 ADC 在扫描模式且用了 DMA，确保 DMA 也停了（上面已停）

    /* ===== 4. 关外设时钟（按总线批量关）===== */
    // APB1 上的 TIM2/3/4
    __HAL_RCC_TIM2_CLK_DISABLE();
    __HAL_RCC_TIM3_CLK_DISABLE();
    __HAL_RCC_TIM4_CLK_DISABLE();
    // APB2 上的 USART1 + ADC1
    __HAL_RCC_USART1_CLK_DISABLE();
    __HAL_RCC_ADC1_CLK_DISABLE();
    // AHB 上的 DMA1
    __HAL_RCC_DMA1_CLK_DISABLE();

    /* ===== 5. GPIO 收尾 ===== */
    // RX 切 EXTI（唤醒源）
    UART_RX_To_EXTI();
    // 未用脚模拟输入
    GPIO_UnusedToAnalog();
    // RS485 低功耗
    RS485_Sleep();
}

void Periph_Stop_Restore(void)
{
    /* ===== 1. 先恢复时钟（所有用到的）===== */
    // AHB
    __HAL_RCC_DMA1_CLK_ENABLE();
    // APB2
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_ADC1_CLK_ENABLE();
    // APB1
    __HAL_RCC_TIM2_CLK_ENABLE();
    __HAL_RCC_TIM3_CLK_ENABLE();
    __HAL_RCC_TIM4_CLK_ENABLE();

    /* ===== 2. 重初始化外设 ===== */
    // 串口（必须重配，Stop 后寄存器丢失部分状态）
    MX_USART1_Init();
    // ADC
    MX_ADC1_Init();

    // ////第五步 关
    // HAL_ADC_Stop_DMA(&hadc1);     // 先停 DMA 传输
    // HAL_ADC_Stop(&hadc1);         // 再停 ADC 转换
    // __HAL_RCC_ADC1_CLK_DISABLE(); // 最后关时钟
    //  开
    // __HAL_RCC_ADC1_CLK_ENABLE();
    // MX_ADC1_Init();
    // HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buf, len);
    // HAL_ADCEx_Calibration_Start(&hadc1);
    //     // 关（以 TIM3 为例，2/4 同理）
    // HAL_TIM_Base_Stop_IT(&htim3);
    // __HAL_TIM_DISABLE(&htim3);
    // __HAL_RCC_TIM3_CLK_DISABLE();
    // // 开
    // __HAL_RCC_TIM3_CLK_ENABLE();
    // MX_TIM3_Init();
    // HAL_TIM_Base_Start_IT(&htim3);
    // 关（逐个 Stream/Channel 停）
    // HAL_DMA_Abort(&hdma_usart1_rx);
    // HAL_DMA_Abort(&hdma_adc1);
    // // 然后关 DMA 时钟
    // __HAL_RCC_DMA1_CLK_DISABLE();
    // // 开
    // __HAL_RCC_DMA1_CLK_ENABLE();
    // // DMA 不用重新 Init，但 Stream/Channel 配置要重新设：
    // __HAL_LINKDMA(&huart1, hdmarx, hdma_usart1_rx);
    // __HAL_LINKDMA(&hadc1, DMA_Handle, hdma_adc1);
    // 六、一个容易漏的点：DMA 中断标志
    // Stop 唤醒后 DMA 中断标志可能残留，导致一恢复就进 DMA 中断。建议清一下：
    // __HAL_DMA_CLEAR_FLAG(&hdma_usart1_rx, DMA_FLAG_TCIFx);
    // __HAL_DMA_CLEAR_FLAG(&hdma_adc1, DMA_FLAG_TCIFx);
    //     // 七.快速检查
    //     进 Stop 前逐条过：
    // [ ] 所有 HAL_xxx_Stop_DMA() 调了
    // [ ] 所有 HAL_TIM_xxx_Stop() 调了
    // [ ] HAL_ADC_Stop() 调了
    // [ ] 所有外设时钟 _CLK_DISABLE() 调了
    // [ ] RX 切 EXTI
    // [ ] GPIO 未用脚模拟输入
    // [ ] HAL_SuspendTick()
    // 唤醒后逐条过：
    // [ ] SystemClock_ReConfig() 第一
    // [ ] 外设时钟 _CLK_ENABLE()
    // [ ] MX_xxx_Init() 重初始化
    // [ ] 重启工作模式（DMA 接收、TIM 启动、ADC 启动）
    // [ ] RX 切回 UART 复用
    // [ ] HAL_ResumeTick()


    HAL_ADC_Start(&hadc1); // 如果原来是一直采的
    // 定时器
    MX_TIM2_Init();
    MX_TIM3_Init();
    MX_TIM4_Init();

    /* ===== 3. 重启各自的工作模式 ===== */
    // UART DMA 接收
    HAL_UART_Receive_DMA(&huart1, mb_rx_buf, MB_BUF_LEN);
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);

    // ADC DMA 连续采样
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_buf, ADC_BUF_LEN);

    // TIM 重启
    HAL_TIM_Base_Start_IT(&htim3); // 比如 TIM3 做 Modbus 3.5T 守护
    // HAL_TIM_Base_Start_IT(&htim2);
    // HAL_TIM_Base_Start_IT(&htim4);
    // 如果有 PWM：
    // HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);

    /* ===== 4. RX 从 EXTI 切回 UART 复用 ===== */
    UART_RX_To_UART();

    /* ===== 5. RS485 恢复接收态 ===== */
    RS485_SetRxMode();
}

void Enter_Stop_Mode(void)
{
    /* 1. 睡前检查 */
    if (!CanEnterStop())
        return;

    /* 2. 关外设 */
    Periph_Stop_Shutdown();

    /* 3. 挂起 SysTick */
    HAL_SuspendTick();

    /* 4. 进 Stop */
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

    /* ===== 唤醒后 ===== */

    /* 5. 恢复时钟（SystemClock_ReConfig 里已经开了 HSE+PLL）*/
    SystemClock_ReConfig();

    /* 6. 恢复外设 */
    Periph_Stop_Restore();

    /* 7. 恢复 SysTick */
    HAL_ResumeTick();

    last_wake_tick = HAL_GetTick();
}

#endif

#if configUSE_TICK_HOOK
void vApplicationTickHook(void)
{
    // __WFI();
    /* 每个 Tick 调用一次，ISR 上下文，尽量少做处理 */
}
#endif // 新增

#if configUSE_IDLE_HOOK
void vApplicationIdleHook(void)
{
    // __WFI();
    /* 空闲任务调用，不能调用会阻塞的 API */
}
#endif // 新增
