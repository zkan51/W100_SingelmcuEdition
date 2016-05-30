#include "main.h"
#include "stm32f10x_adc.h"			

void BackupRegisterInit(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);  //for �͹���ģʽ �ؿ�
	PWR_BackupAccessCmd(ENABLE);
}
	
//�������ģʽ
void Sys_Standby(void)
{
	if(!isCharging)
	{
		RTC_Init();
		PWR_WakeUpPinCmd(ENABLE);
		PWR_EnterSTANDBYMode();
	}
}

/****************************************************************
 * ��������chargingState
 * ����  : ���ʱ�رո���ģ��
 * ����  : �� 
 * ���  : ��
 * ����  ����
 *****************************************************************/
void chargingState(void)
{
	sendTask = off;
	GPS_OFF();
	TIM2_OFF();
	TIM4_OFF();

	LED_ON();
	LED_RED_ON();
}

/****************************************************************
 * ��������ExitCharging
 * ����  : �˳����ʱ�رո���ģ��
 * ����  : �� 
 * ���  : ��
 * ����  ����
 *****************************************************************/
void ExitCharging(void)
{
	LED_OFF();
	LED_RED_OFF();
	interval_num=0;
	sendTask=on; 
}


//����ֵ�������ȼ�
u8 GetBatteryPower(void) 
{
	u16 batteryValue; //����ADֵ
	
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
