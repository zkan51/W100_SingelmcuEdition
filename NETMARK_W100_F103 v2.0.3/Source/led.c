#include "main.h"

void LedFlash(void)  //��һ��
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
	
	if(charging_flag == off)	LED_OFF();	
}

void LedShutdown(void) //�ػ�����
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
