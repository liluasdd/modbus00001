#include "main.h"
#include "APP_Custom.h"
#include "main_2.h"
#include "crc.h"
u8		B_Custom_Hold=0;
u16		R_Custom_Flash_i;

Custom_union R_Custom_BUFF={
	.Custom_array.Custom_A5A5=0xA5A5A5A5,
	.Custom_array.CustomCCW_Data={120,110,100,90,80,70,60,50,40,30},
};

//const 	FLASH_Structure		R_FLASH_Custom  		__attribute__((at(FLASH_Custom_address)));


#define		d_FLASH_Date_maxbyte	14
#define		d_FLASH_Date_maxword	7

void Read_Custom_flash(void)
{
	
}


void Custom_WriteFLASH(void)
{
	
}



