#include "main_2.h"
u16	R_Statistics_number_Set=0;	
u32	R_Statistics_number=0;			
u32	R_Vape_Time_number=0;			

//uint16_t	R_Statistics_Daynumber=0;			
uint32_t    R_Statistics_SevendayNum[5];        
u32	R_OneDay_Seconde_Cnt=0;    //一天的计数器

void	Statistics_numberANDlengthTim(u8	R_tim_s)
{
	static	u8	B;
	static	u8	tim;
	u8	R;
	if(R_tim_s<5)
	{
		B=0;
		tim=0;
	}
	else
	{	
		R=R_tim_s-tim;
		if(R)
		{
			R_Vape_Time_number+=R;
			if(R_Vape_Time_number>d_Statistics_numberTime_MAX)
				R_Vape_Time_number=d_Statistics_numberTime_MAX;
			tim=R_tim_s;
		}
		if(!B)
		{
			B=1;
			if(R_Statistics_number<d_Statistics_number_MAX)
            {
				R_Statistics_number++;

            }
            if(R_Statistics_SevendayNum[0]<d_Statistics_number_EveryDay_MAX)
			{
				R_Statistics_SevendayNum[0]++;
			}
//			if(R_Statistics_Daynumber<d_Statistics_number_EveryDay_MAX)
//			{
//				R_Statistics_Daynumber++;
//			}
		}
	}
}

//uint8_t Update_Flag=0;
//void Statistics_Sevenday_Number(uint8_t hours)
//{
//	static uint8_t hours_q = 0;
//	
//	if(hours != hours_q)
//	{
//		hours_q = hours;
//        if(Update_Flag)
//        {
//            Update_Flag=0;
//            return;
//        }
//		if(!hours)  //0?
//		{
//			for(s8 i=3; i>=0; --i) 
//			{
//				R_Statistics_SevendayNum[i+1] = R_Statistics_SevendayNum[i];
//			}
//			//R_Statistics_Daynumber = 0;
//             R_Statistics_SevendayNum[0] = 0;
//		}
//	}
//	
//	//R_Statistics_SevendayNum[0] = R_Statistics_Daynumber;
//}

//???????????
void	Clear_numberANDlengthTim(void)
{
	R_Vape_Time_number=0;
	R_Statistics_number=0;
	R_OneDay_Seconde_Cnt=0;
   // R_Statistics_Daynumber = 0;
	for(uint8_t i=0; i<5; i++)
	{
		R_Statistics_SevendayNum[i] = 0;
	}
}


//void ExitSleep_Number_Updata(u8 day)
//{
//	uint8_t j;
//	if(day==0)
//	{
//		return;
//	}
//	else if(day>5) 
//	{
//		for(j=0; j<5; j++) 
//		{
//			R_Statistics_SevendayNum[j] = 0;
//		}
//	}
//	else 
//	{
//        for(j=0; j<day; j++)
//		{
//            for(s8 i=3; i>=0; --i) 
//            {
//                R_Statistics_SevendayNum[i+1] = R_Statistics_SevendayNum[i];
//            }
//            R_Statistics_SevendayNum[0] = 0;
//        }
//        Update_Flag=1;
//  }
// // R_Statistics_Daynumber = 0;
//}

void Update_Sevenday_Number(void)
{
   R_OneDay_Seconde_Cnt++;
   if(R_OneDay_Seconde_Cnt>=86400)   //24*60*60
   {
        R_OneDay_Seconde_Cnt=0;
        for(s8 i=3; i>=0; --i) 
        {
            R_Statistics_SevendayNum[i+1] = R_Statistics_SevendayNum[i];
        }
        R_Statistics_SevendayNum[0] = 0;
  }       
}

uint16_t Get_Xday_Number(u8 day) //???????
{ 
	if(day<1 || day>5) return 0;
	return R_Statistics_SevendayNum[day-1];
}
