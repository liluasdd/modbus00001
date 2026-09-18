#include	"APP_Delay.h"

volatile uint32_t uwTick;

#define HAL_MAX_DELAY              0xFFFFFFFFU




void Delay_Nus(uint32_t	t)
{ 
	uint8_t	i;
	while(t--)
	{
		i=5;
		while(i--);
	}
}







void Delay_Nms(unsigned int nTime)
{
  uint32_t tickstart = HAL_GetTick();
  uint32_t wait = nTime;
 
  /* Add a period to guarantee minimum wait */
  if (wait < HAL_MAX_DELAY)
  {
     wait++;
  }
  
  while((HAL_GetTick() - tickstart) < wait)
  {
  }
}
