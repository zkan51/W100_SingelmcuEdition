#include "main.h"

void LedFlash(void)  //��һ��
{
	GetBatteryPower(); //��ȡ����
	if(battery >= BATTERYLEVEL) //�̵�
	{
		LED_ON();
		delay_ms(1000);
		LED_OFF();
	}
	else  //���
	{
		LED_RED_ON();
		delay_ms(1000);
		LED_RED_OFF();	
	}
}

