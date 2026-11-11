#include "main.h"
#include "key.h"

KEY_FLAG Flag_Key;
KEY_Variable R_Key;

extern u8 led_num;
static u8 R8_Key_Tim = 0;

u8 R_key_anxiaOpen; //
// u8 Flag_JustKey;
// u8 Flag_PowerAdj;
u8 key_value1 = 0; // 按键值1
// u8 xdata R_CloclkTim=0;
u8 key_value = 0; // 按键值

void KEY_Filter_processing(void) // 按键滤波处理，1ms一次
{
  u8 Key_LB_data = 0;
  if (!READ_KEY1_STATE)
    Key_LB_data |= 0x01;
  if (!READ_KEY2_STATE)
    Key_LB_data |= 0x02;
  // if (!READ_KEY3_STATE) // 减键被按下
  //   Key_LB_data |= 0x04;

  if (Key_LB_data != key_value)
  {
    if (R8_Key_Tim < 255)
      R8_Key_Tim++;
    if (R8_Key_Tim > 7 || (R8_Key_Tim > 4 && Key_LB_data >= 0x04))
    {
      key_value = Key_LB_data;
    }
  }
  else
  {
    R8_Key_Tim = 0;
  }
}

void JudgeKey(void) // 按键业务处理，20ms一次
{
  static u8 key_realxvalue = 0; // 按键松开时间计数
  if (key_value == 0x00)        // 所有按键都松开
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
          //  if(Flag_PowerAdj)
          {
            if (key_realxvalue == KEY_M)
              key_value1 = KEY_M;
          }
        }
      }

      if (R_Key.RealxCT == 15) //
      {
        // if (!Flag_PowerAdj) // 0.3S短按
        {
          if (R_Key.Data == 2)
          {
            key_value1 = F_PRESS2C;
          }
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
      // if (!Flag_SleepWake) // 0.3S短按
      //   R_Key.PressCT = 0;
      // else
        R_Key.PressCT = 7;
      R_Key.RealxCT = 0;
      Flag_Key.New_Value = 1;
      R_Key.LongCT = 0;
    }
    if (R_Key.PressCT < 250)
      R_Key.PressCT++;

    if (((key_value >= KEY_M) || (Flag_Key.Smoking))/* && (!Flag_PowerAdj)*/)
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
        // Flag_SleepWake = 0;
        key_value1 = F_ECIGSTAR;
      }

      if ((Flag_Key.Press_Invalid) && (key_value1 != F_PRESS5C))
      {
        key_value1 = KEY_NOP;
      }
    }
    else if (((key_value == KEY_M_SUB) || (key_value == KEY_M_ADD) || (key_value == KEY_M_ADD_SUB) || (key_value == KEY_ADD_SUB)) /*&&(!Flag_Key.Smoking)*/) // 长按按键
    {
      Flag_Key.LongPress = 1;
    }
    if (!Flag_Key.LongPress) // 长按按键
    {
      if (R_Key.LongCT < 250)
        R_Key.LongCT++;
      if ((R_Key.PressCT == 20) || (R_Key.PressCT == 30))
      {
        //  if (R_Key.LongCT > 100)
        //  R_Key.PressCT = 24;
        //  else
        R_Key.PressCT = 22;
        if (key_value == KEY_ADD)
          key_value1 = KEY_ADD;
        else if (key_value == KEY_SUB)
          key_value1 = KEY_SUB;
      }
    }
    key_realxvalue = key_value;
  }

  R_key_anxiaOpen = key_value1;
}

/*//xh=0: 电压调整时间  xh=1: 电压调整时间*/
u16 KEY_R_ADD_SUB(u8 *KEY, s16 R, u8 KADD, u8 KSUB, s16 max, s16 min, s8 bu, u8 xh, u8 tim)
{
  if (*KEY == KADD) // 加
  {
    if (R < max)
    {
      R += bu;
      // tick.timeout = OutTimeMain; //更新亮屏时间
    }
    else if (xh)
    {
      // if ((!tim) || (tim >= 20))
      if ((!tim) || (tim >= 30))
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
      // tick.timeout = OutTimeMain;//更新亮屏时间
    }
    else if (xh)
    {
      // if ((!tim) || (tim >= 20))
      if ((!tim) || (tim >= 30))
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

void JudgeSetKey(void)
{
  // if (Flag_PowerAdj || Flag.VDDLow) // 电源调整或低电压
  //   return;
  // if (R_key_anxiaOpen == F_PRESS5C) // 按键5C
  // {
  //   if (Flag_SystemONOFF) // 系统开启
  //   {
  //     InPwerOffingProcess(); // 系统关闭
  //   }
  //   else
  //   {
  //     Flag_SystemONOFF = 1;     // 系统开启
  //     Flag_Key.All_Invalid = 1; // 所有按键无效
  //     InProcessPowerOn();       // 系统开启
  //   }
  //   Flag_Power_Off = 0; // 电源关闭标志
  // }

  // if (Flag_SystemONOFF) // 系统开启
  // {
  //   if (R_key_anxiaOpen == F_PRESS2C) // 按键2C
  //   {
  //     Flag_Preheating = 1;
  //     //  R_DisPowerAdjDuty=23;
  //     //  Flag_PowerAdj=1;
  //     InMainProcess(); // 主处理

  //     // InSetPowerProcess();// 设置功率
  //   }

  //   if (R_key_anxiaOpen == F_LOCKED)
  //   {
  //     Flag_Key.Locked = !Flag_Key.Locked;
  //   }
  // }
}

void TaskKey(void)
{
  // if (g_b_task_jtim) // 20ms
  {
    // g_b_task_jtim = 0;
    // feed_watchdog();
    JudgeKey();
    JudgeSetKey(); // 设置按键
  }
}

#if 0 // 快速响应吸烟
void QuickEcig(void)
{

    R8_Key_Smoke = key_value;
    if (Flag_Key.Locked) // 锁定键
    {
        R8_Key_Smoke = R8_Key_Smoke & 0xfb; // 锁定键下，只响应5C键
    }
    if (Flag_Preheating && Flag_Out.Smoking) // 预热且烟雾
    {
        Flag_First_Egic = 1; // 预热且烟雾下，不响应其他按键
        return;
    }
    //    if(R_Status != D_ProcessMain)
    //    {
    //        Flag_First_Egic = 0;
    //        return;
    //    }
    if ((Flag_SystemONOFF) && ((R_Status == D_ProcessMain) /*||(R_Status == D_ProcessKeyLock)*/ || (R_Status == D_ChargeProcess))) // 系统开启且状态为主状态或充电状态
    {
        if (R8_Key_Smoke >= KEY_M)
        {
            if ((Flag_Key.All_Invalid) || (Flag_Key.Press_Invalid) || (Flag_First_Egic) || Flag_PowerAdj)
                return;
            Flag_First_Egic = 1;
            if ((R_Key.Data != 0x00))
                return;
            if (Flag.LowBat) // 低电池
            {
                return;
            }
            else
            {

                CHARGE_EN_OFF(); // 关闭充电
                if (R_Bat.Analog <= Bat32V)
                {
                    return;
                }
                if (R_Load > MaxLoadVol || R_Load < MinLoadVol || R_Load > 3050) //	5.0*20=100???????5.05????????????????
                {
                    return;
                }
                __delay_us(100); // 保证电平稳定
                // if (R_Load < MinLoadVol) // 0.0.25*20=5   ShortLoad
                // {
                //     return;
                // }
                Read1C_LoadRes();
                if ((R_Load_0 > MaxLoadVol) || (R_Load_0 < MinLoadVol))
                {
                    return;
                }
                // if (!B_Charge_Access) // 充电访问标志
                if (key_value_R_Puffnum_0 == 0)
                    Compara_Smoking();
            }
        }
        else
        {
            // key_value_R_Puffnum_0 = 0;
            Flag_First_Egic = 0;
        }
    }
    else
    {
        Flag_First_Egic = 0;
    }
}
#endif
