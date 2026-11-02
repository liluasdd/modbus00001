#include "APP_Task.h"
u8 R_DisDuty;
u16 R_Smoking_Tim;
// u8 Flag_SystemONOFF;
// u8 Flag_Power_Off;
// u8 Flag_OnPower;
// u8  R_MIC_Choose=0;
// u8 Flag_MICLock=0;
u8 R_BatDelayct = 0;
// u8 Flag_MIC_Work;
// u16 xdata R_Tick_Tim;
struct timetick tick;
u8 R_Low_Batt_Cnt = 0;
u8 Flag_OnePower = 0;

void InDisBatLowProcess(void) // 电池低电处理
{
    R_Low_Batt_Cnt = 5;
    // R_Bat.Percent = 0;
    R_Bat.Level = 0;
    // InMainProcess();
    if (Flag.LowBat == 0 || R_Bat.Analog < Bat33V)
    {
        Flag.LowBat = 1;
        R_Bat.Percent = 0;
        // R_DisBatLevelQQ = 0xff;
    }
    Flag.Bat_DelayCheck = 0;
    R_Status = D_ProcessBatLow;
    tick.timeout = OutTime2S; // 锟酵碉拷压锟斤拷示2.5S
    R_DisDuty = 0xFF;

    //    R_DisBatLevelQQ=0;
    //    Flag.Bat_DelayCheck=0;
    //	R_Status = D_ProcessBatLow;
    //	tick.timeout = 	OutTime2S;
    //    R_DisDuty = 0xFF;
}
// void InProcessPowerOnOne(void)
//{
//     R_Smoking_Tim=0;
//	R_Status = D_ProcessPowerOnOnece;
//	tick.timeout = OutTime2S;
//     R_DisDuty = 0xFF;
// }
void InProcessPowerOn(void) // 开机处理
{
    R_Smoking_Tim = 0;
    R_Status = D_ProcessPowerOn;
    tick.timeout = OutTime2S; // 显示logo2秒
    R_DisDuty = 0xFF;
}

void InShortProcess(void) // 短路处理
{
    Flag_Out.CheckRes = 1;
    //    R_Load = 0;
    R_Status = D_ProcessShort;
    tick.timeout = OutTime2S;
    R_DisDuty = 0xFF;
}
// void InLoadLowProcess(void)
//{
//     R_Smoking_Tim=0;
//     Flag_Out.CheckRes=1;
////    R_Load = 0;
//    R_Load=R_Load_0;
//	R_Status = D_ProcessLoadLow;
//	tick.timeout = OutTime2S;
//    R_DisDuty = 0xFF;
//}
void InOpenProcess(void)
{
    R_Status = D_ProcessOpen;
    tick.timeout = OutTime2S;
    R_DisDuty = 0xFF;
}
// void InLoadHighProcess(void)
//{
//	R_Status = D_ProcessLoadHigh;
//	tick.timeout = OutTime3S;
//     R_DisDuty = 0xFF;
// }
// void InLoadPromptProcess(void)
//{
//	R_Status = D_ProcessLoadPrompt;
//	tick.timeout = OutTimeLoadPropmt;
//     R_DisDuty = 0xFF;
// }
void InTooHotProcess(void)
{
    R_Status = D_ProcessTooHot;
    tick.timeout = OutTime2S;
    R_DisDuty = 0xFF;
}
void InCharingErroProcess(void)
{
    Flag_System_wakeup = 1;
    R_Status = D_ProcessAdPtErr;
    R_DisDuty = 0xFF;
    tick.timeout = OutTime30S;
}

void InLock_UnlockProcess(void) // 锁屏解锁处理
{
    //    Flag_Key.LongPress = 1;
    R_Smoking_Tim = 0;
    R_Status = D_ProcessKeyLock;
    tick.timeout = OutTime2S;
    R_DisDuty = 0xFF;
}

void InPowerOffProcess(void) // 关机处理
{
    //    R_Screen_Adjust=0;
    R_Smoking_Tim = 0;
    Flag_SystemONOFF = 0;
    Flag_Power_Off = 1;
    tick.timeout = OutTime3S;
    R_Status = D_ProcessPowerOff;
    R_DisDuty = 0xFF;
}

void InPwerOffingProcess(void) // 关机处理
{

    R_Smoking_Tim = 0;
    Flag_SystemONOFF = 0;
    R_Status = D_ProcessPowerOffing;
    tick.timeout = OutTime2S;
    // tick.timeout = OutTime1S;
    R_DisDuty = 0xFF;
}
// void InMaxPowerProcess(void)
//{
//     R_Status = D_ProcessMaxPower;
//     tick.timeout = 	OutTime3S;
//     R_DisDuty = 0xFF;
// }
// void InMenuProcess(void)
//{
//     R_BOutType=R_OutType;
//     R_Status = D_ProcessMenu;
//     tick.timeout = 	OutTimeMain;
//     R_DisDuty = 0xFF;
// }
// void InMenuProcess(void)
//{
//     R_Smoking_Tim=0;
//      R_MIC_Choose++;
//      if(R_MIC_Choose>2)
//      {
////        if(R_Mod_MAX==1)
//            R_MIC_Choose=A_MODE;
////        else
////        {
////            R_MIC_Choose=AB_MODE;
////        }
//     }
//
//    R_Status = D_ProcessMenu;
//    tick.timeout = OutTime3S;
//    R_DisDuty = 0xFF;
//}
void InSetPowerProcess(void) // 设置功率处理
{
    // R_Smoking_Tim = 0;
    // Flag_PowerAdj = 1; // 设置功率调节标志
    R_Status = D_ProcessSetPower;
    tick.timeout = OutTime2S;
    R_DisDuty = 0xFF;
}

// void InSetPuffProcess(void)
//{
//     R_Smoking_Tim=0;
//     R_Index_Puff=1;
//     R_Puffnum_Dis=R_Puffnum;
//     R_Status = D_ProcessSetPuff;
//     tick.timeout = 	OutTime3S;
//     R_DisDuty = 0xFF;
// }

// void InVerProcess(void)
//{
//     if(R_Status!=D_ProcessVer)
//     {
//         R_Status = D_ProcessVer;
//         tick.timeout = OutTime3S;
//         R_DisDuty = 0xFF;
//     }
// }

void LowbattTime(void)
{
    tick.timeout = OutTimeMain; // 返回到主页面状态的时间
}

// void ProcessMenuOperate(void)
//{
//      if(R_key_anxiaOpen ==F_KEYOK)
//      {
//       goto gobrck;
//      }
//	 if(!tick.timeout)
//		{
//         gobrck:
////		flag_write_eep = 1;
//        InMainProcess();
//		}
//    CheckCharing_Compare();
//}

extern u8 Flag_Bat_Full_OK;
void InMainProcess(void)
{
    // 预热或烟雾状态，按键无效
    if ((R_Status != D_ChargeProcess) && (!Flag_Out.Smoking))
        Flag_Key.Press_Invalid = 1;
    Flag_Key.LongPress = 1;   //
    R_Status = D_ProcessMain; // 主状态处理
    LowbattTime();            // 低电池处理
    //    Flag_Key.Con_Invalid=1;
    R_DisDuty = 0xFF;
}
void CharingTime(void)
{
    if (!Flag.Charger_Over) // 充电中
    {
        tick.timeout = OutTimeChargeing;
    }
    else
    {
        if (Flag_SystemONOFF) // 开机状态
            tick.timeout = OutTime1DM;
        if (Flag_Bat_Full_OK && Flag_SystemONOFF == 0)
            tick.timeout = OutTime3S;
        // tick.timeout = OutTime10DM;
    }
}
void InChargeProcess(void)
{
    Flag_Key.Press_Invalid = 1; // 充电状态下，按键无效
    //    R_Smoking_Tim=0;
    R_Status = D_ChargeProcess; // 充电状态处理
    CharingTime();              // 充电时间处理
    R_DisDuty = 0xFF;
}

void Datainit(void)
{
    R_SetPower.ints = 50;
    R_SetVol.ints = 180;
    Flag_Key.Locked = 0;
    //  R_MIC_Choose=B_MODE;
    //  Flag_MICLock=0;
    //  B_Smart_ON_OFF=0;
    R_Puffnum = 0;
    //  R_OutType=1;
    //  R_Index=1;
    //  B_Deful_Index=0;
    //  B_Smart_Index=0;
    //  R_MaxPower=400;
    //  R_MinPower=50;
}
u8 Flag_OnePower_one = 0;
void Power_Data_Init(void)
{
    InProcessPowerOn();
    Flag_OnePower_one = 0;
    // Flag_OnePower_one = 1;
    //    InMainProcess();
    //    tick.timeout = OutTime1S;
    Datainit();
    Flag_OnePower = 1; // 一键开启
    Flag_OnPower = 1;
    R_Screen_Adjust = 50;
    Flag.Adapt_Err = 0;
    R_Smoking_Tim = 0;
    Flag.Bat_DelayCheck = 0;
    R_Bat.Level = 0x00;   // 电池等级
    Flag_SystemONOFF = 1; // 系统开启状态
    Flag.Bat_Check = 1;
    //	Flag_Key.All_Invalid=1;     // 键盘无效状态
    Flag.Percent_Init = 0;
    Flag_Load_Initial = 0;
    R_Load_Initial = 0;
    R_DisBatLevelQQ = 0xff;
    R_CoilState = D_CoilUnIn; // 线圈状态
    //    R_MAX_POWER=300;
    //    R_MIN_POWER=50;
    R_Load_0 = 9999;
    R_Load = 9999;
    delay_ms(10);

    // Flag_OnePower_one = 0;
    // Flag_Power_Off = 0;
    // Flag_SystemONOFF = 1; // 系统开启
    // Flag_Key.All_Invalid = 1;
    // InProcessPowerOn(); // 系统开启
    CheckBat();
}

void ProcessPowerOff(void) // 关机处理
{
    if (Flag_Key.New_Value)
    {
        tick.timeout = OutPowerOff; // 重置关机时间
        //            if(Flag_Power_Off)
        //            {
        //               if(Flag.Adapt_In)
        //               {
        //                   if(!Flag.Adapt_Err)
        //                   {
        //                      InChargeProcess();
        //                   }
        //                   else
        //                   {
        //                    InCharingErroProcess();
        //                   }
        //               }
        //            }
    }
    CheckCharing_Compare(); // 检查充电状态

    if (!tick.timeout) // 系统开启时间大于等于关秒
    {
        //          if(!R_Screen_Adjust)
        {
            Flag_Power_Off = 1;

            //             if((Flag.Adapt_In)&&(!Flag.Adapt_Err))
            //                {
            //                InChargeProcess();
            //                }
            //              else if(Flag.Adapt_Err)
            //                {
            //                   InCharingErroProcess();
            //                }
            //              else
            {
                tick.timeout = OutPowerOff; // ????5????,???D_KEY5Ct??????sleep
                Compare_Sleep();
            }
        }
    }
}

void ProcessCharingError(void)
{
    if (Flag_Key.New_Value) // 键盘新值
    {
        if (Flag_SystemONOFF)
        {
            //           InMainProcess();
            if (R8_Key_Smoke >= KEY_M)
            {
                JudgeEcig(); // 判断ECIGSTAR
            }
            else
            {
                BackUpM();
            }
            tick.timeout = OutTime2S; // 重置睡眠时间
        }
    }

    CheckCharing_Compare(); // 检查充电状态

    if (Flag.Adapt_In == 0)
    {
        BackUpM(); // 备份M键状态
    }
    if (tick.timeout == 0) // 系统开启时间等于0
    {
        tick.timeout = OutTime30S; // 重置睡眠时间
        syssleep();                // 进入睡眠状态
    }
}

extern u8 Flag_SystemON;
u8 KEY_R_ADD_SUB_num = 0; //
u8 key_Sleep_num = 0;
void ProcessMain(void)
{
    //  u16 a;
    Flag_Out.Smoking = 0;
    if ((Flag_Key.New_Value) || (Flag.Buck) || Flag_PowerAdj) // 键盘新值或Buck或PowerAdj
    {
        LowbattTime(); // 重置低电池时间
        Flag.Buck = 0;
    }
    if (tick.timeout < OutTime2S) // 系统开启时间小于2秒
    {
        if (Flag_OnPower)
        {
            Flag.Percent_Init = 0;
            R_DisBatLevelQQ = 0xff; // 重置电池等级显示为最大
        }
    }
    // if (!B_Charge_Access) // 充电访问标志
    {
        if (R8_Key_Smoke >= KEY_M && Flag_SystemONOFF) // 键盘按下M键且系统开启
        {
            if (R_key_anxiaOpen == F_ECIGSTAR) // 键盘按下M键且ECIGSTAR
            {
                JudgeEcig(); // 判断ECIGSTAR
            }
        }
        if (Flag_Preheating && Flag_SystemONOFF) // 预热
        {
            JudgeEcig();           // 判断ECIGSTAR
            if (!Flag_Out.Smoking) // 未烟雾
            {
                Flag_Preheating = 0;
            }
        }
    }
    //    else
    //    {
    //        Flag_Preheating = 0;
    //    }
    //   if(Flag_PowerAdj)

    // if (KEY_R_ADD_SUB_num)
    // {
    //     KEY_R_ADD_SUB_num--;
    // }

    if (/*KEY_R_ADD_SUB_num == 0*/ Flag_SystemONOFF)
    {
        // KEY_R_ADD_SUB_num = 2;
        if ((R_key_anxiaOpen == KEY_ADD) || (R_key_anxiaOpen == KEY_SUB))
        {
            Flag_SystemON = 1;
            InSetPowerProcess();
        }
        // a = KEY_R_ADD_SUB(&R_key_anxiaOpen, R_SetVol.ints, KEY_ADD, KEY_SUB, MaxVoult, MinVoult, 10, 1, R_Key.LongCT); // 电压调整

        // if (a != R_SetVol.ints)
        // {
        //     // R_SetVol.ints = a;
        //     tick.timeout = OutTime2S;
        //     InSetPowerProcess();
        //     R_DisPowerAdjDuty = 2;                                      // 电压调整时间
        //     if (R_SetVol.ints == MaxVoult || R_SetVol.ints == MinVoult) // 电压调整到最大或最小
        //     {
        //         Flag_Key.LongPress = 1;
        //     }

        // if (Flag_PowerAdj == 1)
        // {
        //     R_SetVol.ints = a;
        // }
        // }
    }
    if (tick.timeout == 1 || Flag_SystemONOFF == 0)
    {
        Flag_SystemON = 0;
    }
    if (Flag_SystemON == 0)
    {
        CheckCharing_Compare(); // 检查充电状态
    }

    if (!tick.timeout)
    {
        //        if(!R_Screen_Adjust)
        //        {
        LowbattTime(); // 重置低电池时间
        //          if(Flag.Adapt_In)
        //            Flag_Full_Buck=1;
        //          else
        //            Flag_Full_Buck=0;
        //        }
        Compare_Sleep(); // 比较睡眠时间 调试不进休眠
    }
}

void ProcessSetPower(void)
{
    u16 a;
    // if (Flag_PowerAdj)
    {
        a = KEY_R_ADD_SUB(&R_key_anxiaOpen, R_SetVol.ints, KEY_ADD, KEY_SUB, MaxVoult, MinVoult, 10, 1, R_Key.LongCT); // 电压调整
        if (a != R_SetVol.ints)
        {
            tick.timeout = OutTime2S;
            R_SetVol.ints = a;
            if (R_SetVol.ints == MaxVoult || R_SetVol.ints == MinVoult) // 电压调整到最大或最小
            {
                Flag_Key.LongPress = 1;
            }
        }
        R_key_anxiaOpen = 0; // 同一按键事件只处理一次
    }

    // if (R8_Key_Smoke >= KEY_MODE)
    if (R8_Key_Smoke >= KEY_M /* && !B_Charge_Access*/)
    {
        JudgeEcig();
    }

    if (tick.timeout == 0)
    {
        InMainProcess();
    }
}

// void ProcessSetPower(void)
// {
//     u16 a;
//     if (Flag_PowerAdj)
//     {
//         // a = KEY_R_ADD_SUB(&R_key_anxiaOpen, R_SetPower.ints, KEY_M, KEY_SUB, R_MAX_POWER, MinPower, 10, 1, R_Key.LongCT);
//         // if (a != R_SetPower.ints) // 功率调整
//         // {
//         //     R_SetPower.ints = a;
//         //     tick.timeout = OutTime2S;
//         //     if (R_SetPower.ints == R_MAX_POWER)
//         //     {
//         //         Flag_Key.LongPress = 1;
//         //     }
//         // }
//         a = KEY_R_ADD_SUB(&R_key_anxiaOpen, R_SetVol.ints, KEY_ADD, KEY_SUB, MaxVoult, MinVoult, 10, 1, R_Key.LongCT); // 电压调整
//         if (a != R_SetVol.ints)
//         {
//             tick.timeout = OutTime2S;
//             R_SetVol.ints = a;
//             R_DisPowerAdjDuty = 2;                                      // 电压调整时间
//             if (R_SetVol.ints == MaxVoult || R_SetVol.ints == MinVoult) // 电压调整到最大或最小
//             {
//                 Flag_Key.LongPress = 1;
//             }
//         }
//     }
//     // if (R8_Key_Smoke >= KEY_MODE)
//     if (R8_Key_Smoke >= KEY_M /* && !B_Charge_Access*/)
//     {
//         Flag_PowerAdj = 0; // 清除功率调节标志
//         JudgeEcig();
//     }
//     if (tick.timeout == 0)
//     {
//         Flag_PowerAdj = 0; // 清除功率调节标志
//         InMainProcess();   //// 返回主界面处理
//     }
// }

// void ProcessSetPuff(void)
//{
//     if(R_key_anxiaOpen==F_KEYOK)
//     {
//         tick.timeout=OutTime3S;
////    R_key_anxiaOpen=KEY_ADD;
//    }
//    if(R8_Key_Smoke>=KEY_MODE)
//        {
//          if(R_Index_Puff==0)
//          {
//            R_Puffnum=0;
//          }
//           JudgeEcig();
//        }
////    a=KEY_R_ADD_SUB(&R_key_anxiaOpen,R_Index_Puff,KEY_ADD,KEY_SUB,1,0,1,1,R_Key.LongCT);
////    if(a!=R_Index_Puff)
////    {
////        tick.timeout = OutTime5S;
////        R_Index_Puff=a;
////    }
////    if(R_key_anxiaOpen==F_KEYOK)
////    {
////       if(!R_Index_Puff)
////            R_Puffnum=0;
////        InMainProcess();
////            R_Index_Puff=0;
////    }
//    if(!tick.timeout)
//    {
//      InMainProcess();
//    }
//}

// void ProcessCheckRes(void)
//{
//     if(R8_Key_Smoke>=KEY_M)
//     {
//         JudgeEcig();
//     }
//     if(!tick.timeout)
//     {
//         InMainProcess();
//     }
// }
