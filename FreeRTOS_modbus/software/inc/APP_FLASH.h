#ifndef		_APP_FLASH_
#define		_APP_FLASH_
#include	"main.h"


typedef union
{
	volatile	u8	R_Flash8[1024];
	volatile	u16	R_Flash16[512];
	volatile	u32	R_Flash32[256];
    volatile	u64	R_Flash64[128];
}FLASH_Power_typedef;



//Flash	数据区
#define		d_Flash_Sector									 1024			               //一页FLASH大小
#define 	FLASH_Power_down_save_ADDR   	                 ((uint32_t)0x800F000)   	/* 掉电保存区域 1024*/
  	

/*定义数据属性---------------------*/		

extern 	u8 R_Flash_WB;

extern void Data_retention(void);
extern void Rflash(void);
#endif



