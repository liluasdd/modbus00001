#include "main.h"
#include "key.h"

u8 g_b_screen_en = 0;

u8 g_key_num = 0;
u8 g_key_buf = 0;
u8 g_key_tim = 0;
u8 g_b_key_status = 0;
u8 b_key_longpress_tim = 0;
u8 b_key_long_press = 0;
u8 g_dormancy_tim = 0;
u8 R8_Set_V_Delay = 0;

extern u8 led_num;

void Key_val_clear(void)
{
  if (g_key_tim < 255)
  {
    g_key_tim++;
  }

  if (g_key_tim == 30)
  {
    led_num = !led_num; // 切换led状态位
    g_key_num = g_key_buf;
  }
}

void key_scan(void)
{
  g_key_num = 0;
  if (!KEY1_STATE)
  {
    if (g_b_key_status == 0)
    {
      g_b_key_status = 1;
      g_dormancy_tim = 0;
      g_key_tim = 0;
      g_key_buf++;
      b_key_long_press = 0;    // 按键长按标志位
      b_key_longpress_tim = 0; // 按键长按时间计数器
      // g_b_screen_en = 1;       //  屏幕显示标志位
      R8_Set_V_Delay = 30; //  设置电压延时时间 30ms
    }
    else // 按键长按标志位为1时
    {
      b_key_longpress_tim++; // 按键长按时间计数器增加
      // 按键长按时间超过延时时间时
      if (b_key_longpress_tim > R8_Set_V_Delay)
      {
        // 电压延时时间超过 10ms 时，每次增加 3ms
        if (R8_Set_V_Delay > 10)
          R8_Set_V_Delay -= 3;
        b_key_longpress_tim = 0; // 按键长按时间计数器重置
        b_key_long_press = 1;    // 按键长按标志位设置为1，触发长按事件
      }
    }
  }
  else
  {
    if (g_b_key_status)
    {
      g_b_key_status = 0;
      g_key_tim = 0;
      b_key_long_press = 0;
      b_key_longpress_tim = 0;
    }
  }
  Key_val_clear();
}
