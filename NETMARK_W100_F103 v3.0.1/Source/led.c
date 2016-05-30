#include "main.h"

void LedFlash(void)  //闪一次
{
	  if(battery >= BATTERYLEVEL) //绿灯
			{
					LED_ON();
					Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);
					Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);
					Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);
					Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);
					Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);
					Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);
					Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);
					Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);
					Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);
					Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);
					
					if(charging_flag == off)	
								LED_OFF();	
			}
			else  //红灯
			{
					LED_RED_ON();
					Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);
					Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);
					Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);
					Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);
					Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);
					Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);
					Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);
					Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);
					Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);
					Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);
					
					if(charging_flag == off)	
								LED_RED_OFF();	
			}
}

void LedShutdown(void) //关机快闪
{
	u16 i;
	
	for(i=0;i<10;i++)
	{
		LED_ON();
		Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);
		Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);
		Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);

		if(charging_flag != on)
		{
			LED_OFF();
			Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);
			Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);
			Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);
		}
	}
}
