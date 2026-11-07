#ifndef __KEY_H__
#define __KEY_H__

#include "main.h"

extern u8 B_KEY_JC; // 按键检测标志位
extern u8 R_KEY_timMax;
extern vu8 R_KEY_timi;
extern u8 Key_LB_value; // 濠娿倖灏濋崥搴ｆ畱缂佹挻鐏?

extern u8 R_key_anxia;
extern u8 R_key_Open;
extern u8 R_key_anxiaOpen; // 短按松开
extern u8 R_key_anzhuOpen; // 长按松开
extern u8 R_key_keep;
extern u8 R_key_super_keep;
extern u8 R_key_1C;
extern u8 R_key_2C; // 按键2次
extern u8 R_key_3C; // 按键3次
extern u8 R_key_4C; // 按键4次
extern u8 R_key_5C;
extern u8 R_key_7C;
extern u8 R_key_XC;
extern u8 R_key_XC_Data;
extern u8 R_key_anzhu;
extern u8 key_value; // 当前按键值寄存器
extern u8 key_value1; // 当前按键值寄存器
extern u8 B_Key_Clean_xC;
extern u8 B_Key_release;
extern u8 B_Key_Different;
extern u8 B_anxiaOpen_EN;
extern u8 B_anxiaOpen_EN0;

extern u8 B_USB_RinBit;
extern u8 B_USB_Low;
extern u8 B_USB_In_Flag; // usb插入标志
extern u8 B_InterfaceGOTO;
extern u16 USB_OVP;
extern u16 Bat_V_diff_data;

typedef struct 
{
    u8 Smoking              :1;     //烟雾
    u8 LongPress            :1;     //长按标志
    u8 All_Invalid          :1;     //全无效
    u8 Con_Invalid          :1;     //连续无效
    u8 Press_Invalid        :1;     //短按无效
    u8 New_Value            :1;     //新值
    u8 WakeUp               :1;     //唤醒标志
    u8 Locked               :1;     //锁定标志  
}KEY_FLAG;

typedef struct 
{
    u8 NewValue;     //新值
    u8 Data;         //数据
    u8 RealxCT;      //真实时间
    u8 LongCT;       //长按时间
    u8 PressCT;      //短按时间     
            
}KEY_Variable;
typedef enum  KEY_bit
{
	KEY_SUB=0x01,
    KEY_ADD=0x02,
	KEY_ADD_SUB=0x03,
    KEY_M=0x04,
	KEY_M_SUB=0x05,
	KEY_M_ADD=0x06,
    KEY_M_ADD_SUB=0x07,
	KEY_MODE=0x08,
	KEY_NOP=0xFF,
}KEY_bit;

typedef enum  KEY_Function
{
	F_COMBIN=0x20,
    F_ECIGSTAR=0x21,
    F_PRESS5C=0x22,
    F_PRESS3C=0x23,
    F_DIRSET=0x24,
    F_VERSION=0x25,
    F_CHANGESET=0x26,
    F_LOCKED=0x27,
    F_POWERADJ=0x09,    
    F_MENU=0x28,  
    F_KEYOK=0x29,
    F_CLEAR=0x30,   
    F_PRESS2C=0x31, 
    
}KEY_Function;

extern void key_scan(void);
extern void KEY_USB_GPIO_int(void);
extern void Key_scan(void);
extern void KEY_ALL_Task(void);
extern void KEY_TFT_RESET(void);
extern u16 KEY_R_ADD_SUB(u8 *key_value, s16 R, u8 KADD, u8 KSUB, s16 max, s16 min, s8 bu, u8 xh, u8 tim);
extern void KEY_Filter_processing(void);
extern void WakeUp_Check_USB(void);
extern void TaskKey(void);

extern u8 g_key_num;
extern u8 g_b_screen_en;
extern u8 g_dormancy_tim;
extern u8 R8_Set_V_Delay;
extern u8 b_key_long_press;

#endif
