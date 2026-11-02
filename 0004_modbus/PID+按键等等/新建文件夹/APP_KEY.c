#include "APP_KEY.h"

KEY_FLAG Flag_Key;
KEY_Variable R_Key;
u8 R_key_anxiaOpen; //
// u8 Flag_JustKey;
// u8 Flag_PowerAdj;
u8 key_value1 = 0; // ????????????
// u8 xdata R_CloclkTim=0;
u8 key_value = 0; // ???????????
void Get_KeyVol(void)
{
    static u8 R8_Key_Tim;
    R_Key.NewValue = 0;
    if (!KEY_1_STATUS) // 主按键,吸烟
    {
        R_Key.NewValue |= 0x04;
    }
    if (!KEY_2_STATUS) // 减
    {
        R_Key.NewValue |= 0x01;
    }
    if (!KEY_3_STATUS) // 加
    {
        R_Key.NewValue |= 0x02;
    }
    //    if(KEY_MIC_STATUS)
    //        {
    //            R_Key.NewValue |= 0x08;
    //        }
    //    if(!READ_KEY2_STATUS)
    //        {
    //           R_Key.NewValue  |= 0x02;
    //        }
    //    if(!READ_KEY1_STATUS)
    //        {
    //           R_Key.NewValue  |= 0x01;
    //        }

    if (R_Key.NewValue != key_value)
    {
        if (R8_Key_Tim < 255)
            R8_Key_Tim++;
        if (R8_Key_Tim > 7 || (R8_Key_Tim > 4 && R_Key.NewValue >= 0x04))
        {
            key_value = R_Key.NewValue;
            //            if(KEY_MIC_STATUS)
            //            {
            //                R_Key.Data = 0;
            ////                Flag_Key.LongPress=1;
            //                if(Flag_PowerAdj)
            //                {
            //                    Flag_PowerAdj=0;
            //                }
            //            }
        }
    }
    else
    {
        R8_Key_Tim = 0;
    }
}

// u8 key_value1_num = 0;
// extern u8 key_Sleep_num;
void JudgeKey(void)
{
    static u8 key_realxvalue = 0; // 按键松开时间计数

    //    Get_KeyVol();
    //    key_value = R_Key.NewValue;

    if (key_value == 0x00) // 所有按键都松开
    {
        if (R_Key.RealxCT != 0xFF)
            R_Key.RealxCT++; // 按键松开时间计数
        R_Key.LongCT = 0;
        if (key_realxvalue >= KEY_M)
        {
            if (R_Key.PressCT > 1)
            {
                if (R_Key.PressCT > 15) // 0.3S长按
                {
                    R_Key.Data = 0;
                }
                else
                {
                    if (R_Key.Data < 10)
                        R_Key.Data++;
                }
                if ((R_Key.PressCT < 15) && (!Flag_Key.LongPress)) // 0.3S短按
                {
                    //                        if(Flag_PowerAdj)
                    {
                        if (key_realxvalue == KEY_M)
                            key_value1 = KEY_M;
                    }
                }
            }

            if (R_Key.RealxCT == 15) //
            {
                if (!Flag_PowerAdj) // 0.3S短按
                {
                    if (R_Key.Data == 2)
                    {
                        key_value1 = F_PRESS2C;
                    }
                    //                       if(R_Key.Data ==3)
                    //                        {
                    //                            key_value1 = F_PRESS3C;
                    //                        }
                    //                        if(R_Key.Data ==4)
                    //                        {
                    //                            key_value1 = F_LOCKED;
                    //                        }
                }
            }
        }
        if ((R_Key.PressCT > 1) && ((R_Key.PressCT < 20) && (!Flag_Key.LongPress)))
        {
            key_value1 = key_realxvalue;
        }

        if ((R_Key.RealxCT > 15) || (R_Key.Data == 5))
        {
            R_Key.Data = 0;
        }
        Flag_Key.All_Invalid = 0;
        Flag_Key.LongPress = 0;
        Flag_Key.Press_Invalid = 0;
        Flag_Key.Con_Invalid = 0;
        Flag_Key.Smoking = 0;
        R_Key.PressCT = 0;
    }
    else if (Flag_Key.All_Invalid) // 所有按键都松开
    {
        R_Key.PressCT = 0;
        R_Key.Data = 0;
    }
    else
    {
        if (key_realxvalue != key_value) // 按键松开时间计数
        {
            R_Key.LongCT = 0;
            R_Key.PressCT = 0;
            Flag_Key.Press_Invalid = 0;
        }
        if (R_Key.RealxCT) // 按键松开时间计数
        {
            if (!Flag_SleepWake) // 0.3S短按
                R_Key.PressCT = 0;
            else
                R_Key.PressCT = 7;
            R_Key.RealxCT = 0;
            Flag_Key.New_Value = 1;
            R_Key.LongCT = 0;
        }
        if (R_Key.PressCT < 250)
            R_Key.PressCT++;

        if (((key_value >= KEY_M) || (Flag_Key.Smoking)) && (!Flag_PowerAdj))
        {
            //             if(R_Status==D_ProcessSetPuff)
            //                 {
            //                     if(key_value==KEY_M)
            //                     {
            //                         if(R_Key.PressCT==2)
            //                         {
            //                           key_value1 = F_KEYOK;
            //                         }
            //                     }
            //                 }
            //              else
            {
                key_value1 = KEY_M;
                if (R_Key.PressCT == 2)
                {
                    if (R_Key.Data == 4) // ??????
                    {
                        key_value1 = F_PRESS5C;
                    }
                }
                else if (R_Key.PressCT == 8)
                {
                    Flag_SleepWake = 0;
                    key_value1 = F_ECIGSTAR;
                }
            }
            if ((Flag_Key.Press_Invalid) && (key_value1 != F_PRESS5C))
            {
                key_value1 = KEY_NOP;
            }
        }
        //            if(Flag_SystemONOFF)
        //            {
        //                if(!Flag_Out.Smoking)
        //                {
        //                    if(!Flag_PowerAdj)
        //                    {
        //                         if(!READ_KEY3_STATUS)
        //                         {
        //                             if(R_Key.PressCT==150)
        //                             {
        //                                key_value1=F_CLEAR;
        //                             }
        //                         }
        //                    }
        //                }
        //            }
        else if (((key_value == KEY_M_SUB) || (key_value == KEY_M_ADD) || (key_value == KEY_M_ADD_SUB) || (key_value == KEY_ADD_SUB)) /*&&(!Flag_Key.Smoking)*/) // 长按按键
        {
            //                Flag_Key.Press_Invalid=1;
            Flag_Key.LongPress = 1;
            //                if(!Flag_Key.Con_Invalid)   //???????水??????Ч
            //                {
            //                   if(R_Key.PressCT==5)
            //                        key_value1 =F_COMBIN;
            ////                    if(Flag_SystemONOFF)
            ////                    {
            //////                        if(R_Key.PressCT==25)
            //////                        {
            //////                            if(key_value==KEY_ADD_SUB)
            //////                            {
            //////                               key_value1 = F_MENU;
            //////                            }
            //////                        }
            //////                        else if(key_value==KEY_M_ADD_SUB)
            //////                        {
            //////                          key_value1 = F_VERSION;
            //////                        }
            ////                    }
            ////                    else
            ////                   if(key_value==KEY_ADD_SUB)
            ////                    {
            ////                        if(!Flag_SystemONOFF)
            ////                        {
            ////                            if(R_Key.PressCT==100)
            ////                            {
            ////                                  key_value1 = F_VERSION;
            ////                            }
            ////                        }
            ////                        else
            ////                        {
            ////                            if(R_Key.PressCT==25)
            ////                            {
            ////                                key_value1 = F_LOCKED;
            ////                            }
            ////                        }

            ////                    }
            //                }
        }
        if (!Flag_Key.LongPress) // 长按按键
        {
            if (R_Key.LongCT < 250)
                R_Key.LongCT++;
            if ((R_Key.PressCT == 20) || (R_Key.PressCT == 30))
            {
                //                if (R_Key.LongCT > 100)
                //                    R_Key.PressCT = 24;
                //                else
                R_Key.PressCT = 22;
                if (key_value == KEY_ADD)
                    key_value1 = KEY_ADD;
                else if (key_value == KEY_SUB)
                    key_value1 = KEY_SUB;
                //                    if(Flag_PowerAdj)
                //                    {
                //                        if(R_Key.LongCT>100)
                //                             R_Key.PressCT=25;
                //                        else
                //                            R_Key.PressCT=22;
                //                        if(key_value==KEY_M)
                //                        {
                //                            key_value1 = KEY_M;
                //                             if(R_SetPower.ints<R_MAX_POWER)
                //                            R_DisPowerAdjDuty=2;
                //                        }
                //                    }
            }
        }
        key_realxvalue = key_value;
    }
    // if (key_Sleep_num == 0)
    {
        R_key_anxiaOpen = key_value1;
    }

    // if((!READ_KEY3_STATUS)&&(!Flag_SystemONOFF)&&(!Flag.VDDLow))
    //     {
    //        if(R_CloclkTim)
    //             R_CloclkTim--;
    //        if(R_CloclkTim==0)
    //         {
    //             R_CloclkTim=50;
    //            if(++R_5Cloclk==5)
    //             {
    //                InVerProcess();
    //             }
    //         }
    //     }
}

////  //xh=0: 电压调整时间  xh=1: 电压调整时间
u16 KEY_R_ADD_SUB(u8 *KEY, s16 R, u8 KADD, u8 KSUB, s16 max, s16 min, s8 bu, u8 xh, u8 tim)
{
    if (*KEY == KADD) // 加
    {
        if (R < max)
        {
            R += bu;
            tick.timeout = OutTimeMain;
        }
        else if (xh)
        {
            if ((!tim) || (tim >= 20))
            // if ((!tim) || (tim >= 30))
            {
                R = min;
            }
        }
    }
    else if (*KEY == KSUB) // 减
    {
        if (R > min)
        {
            R -= bu;
            tick.timeout = OutTimeMain;
        }
        else if (xh)
        {
            if ((!tim) || (tim >= 20))
            // if ((!tim) || (tim >= 30))
            {
                R = max;
            }
        }
    }
    if (R > max)
        R = max;
    else if (R < min)
        R = min;
    return R;
}

u8 Flag_SystemON = 0;
void JudgeSetKey(void)
{
    if (Flag_PowerAdj || Flag.VDDLow) // 电源调整或低电压
        return;
    if (R_key_anxiaOpen == F_PRESS5C) // 按键5C
    {
        if (Flag_SystemONOFF) // 系统开启
        {
            Flag_SystemON = 0;
            InPwerOffingProcess(); // 系统关闭
        }
        else
        {
            Flag_SystemON = 1;
            Flag_SystemONOFF = 1;     // 系统开启
            Flag_Key.All_Invalid = 1; // 所有按键无效
            InProcessPowerOn();       // 系统开启
        }
        Flag_Power_Off = 0;
    }

    if (Flag_SystemONOFF) // 系统开启
    {
        if (R_Status != D_ProcessPowerOn)
        {
            // if (!Flag_Key.Locked)// 锁定
            {
                if (R_key_anxiaOpen == F_PRESS2C) // 按键2C
                {
                    Flag_Preheating = 1;
                    //                        R_DisPowerAdjDuty=23;
                    //                        Flag_PowerAdj=1;
                    InMainProcess(); // 主处理
                    // InSetPowerProcess();// 设置功率
                }
                //                    if(R_key_anxiaOpen==F_CLEAR)
                //                    {
                //                        R_Puffnum=0;
                //                        if(	R_Status != D_ProcessMain)
                //                        {
                //                            InMainProcess();
                //                        }
                //                    }
            }

            if (R_key_anxiaOpen == F_LOCKED)
            {
                Flag_Key.Locked = !Flag_Key.Locked;
                //                    InLock_UnlockProcess();
            }
        }
    }
    //    else
    //        {
    //           if(R_key_anxiaOpen==F_VERSION)
    //             {
    //              InVerProcess();
    //             }
    //        }
}

void TaskKey(void)
{
    if (Flag_JustKey) // 20ms
    {
        Flag_JustKey = 0;
        feed_watchdog();
        JudgeKey();
        JudgeSetKey(); // 设置按键
    }
}
