#include "main.h"
#include "stm32f10x_adc.h"			

void BackupRegisterInit(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);  //for 低功耗模式 必开
	PWR_BackupAccessCmd(ENABLE);
}
	
void Enter_PowersaveMode(void)
{
  PA_OFF();
	PLL_OFF();
	GPS_OFF();
	//LED_OFF();
	
	task_flag2=off; //不发送任务
	
}

//返回值：电量等级
u8 GetBatteryPower(void) 
{
	u16 batteryValue; //电量AD值
	
	batteryValue = Get_Adc_Average(ADC_Channel_9,5);
	
	if(batteryValue > 3750)	battery = 9;  //>8v
	else if((batteryValue > 3650) && (batteryValue <= 3750 ))	battery = 8;  //7.8v-8v
	else if((batteryValue > 3554) && (batteryValue <= 3650))	battery = 7;  //7.6v-7.8v
	else if((batteryValue > 3465) && (batteryValue <= 3554))	battery = 6;  //7.4v-7.6v
	else if((batteryValue > 3380) && (batteryValue <= 3465))	battery = 5;  //7.2v-7.4v
	else if((batteryValue > 3290) && (batteryValue <= 3380))	battery = 4;  //7.0v-7.2v
	else if((batteryValue > 3205) && (batteryValue <= 3290))	battery = 3;  //6.8v-7.0v
	else if((batteryValue > 3130) && (batteryValue <= 3205))	battery = 2;  //~6.6v-6.8v
	else 	battery = 1;
	
	return battery;
}
