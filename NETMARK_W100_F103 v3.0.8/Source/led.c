#include "main.h"

void LedFlash(void)  //闪一次
{
	GetBatteryPower(); //获取电量
	if(battery >= BATTERYLEVEL) //绿灯
	{
		LED_ON();
		delay_ms(1000);
		LED_OFF();
	}
	else  //红灯
	{
		LED_RED_ON();
		delay_ms(1000);
		LED_RED_OFF();	
	}
}

//开机后红绿灯一起闪烁五秒
void led_Staringup(void)
{
 LED_ON();
	LED_RED_ON();
	delay_ms(1000);
	LED_OFF();
	LED_RED_OFF();
	delay_ms(1000);
}
