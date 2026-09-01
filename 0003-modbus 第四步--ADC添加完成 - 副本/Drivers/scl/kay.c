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


void Key_val_clear(void)
{
  if (g_key_tim < 255)
  {
    g_key_tim++;
  }

  if (g_key_tim == 50)
  {
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
      b_key_long_press = 0;    // 清除长按标志
      b_key_longpress_tim = 0; // 重置长按计时
      // g_b_screen_en = 1;       // 按键按下时，使能屏幕显示
      R8_Set_V_Delay = 30;     // 重置长按检测延迟阈值（初始 30 单位）
    }
    else // 按键状态为 1：按键持续按下中
    {
      b_key_longpress_tim++; // 累加长按计时器
      // 当长按时间超过当前延迟阈值时，触发长按逻辑
      if (b_key_longpress_tim > R8_Set_V_Delay)
      {
        // 动态缩短长按检测延迟（最小 10 单位，避免过短误判）
        if (R8_Set_V_Delay > 10)
          R8_Set_V_Delay -= 3;
        b_key_longpress_tim = 0; // 重置长按计时器
        b_key_long_press = 1;    // 置位长按标志（供上层逻辑判断）
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
