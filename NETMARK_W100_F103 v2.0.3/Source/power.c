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
	
	if(batteryValue > 3722)	battery = 9;  //>8v
	else if((batteryValue > 3629) && (batteryValue <= 3722 ))	battery = 8;  //7.8v-8v
	else if((batteryValue > 3542) && (batteryValue <= 3629))	battery = 7;  //7.6v-7.8v
	else if((batteryValue > 3447) && (batteryValue <= 3542))	battery = 6;  //7.4v-7.6v
	else if((batteryValue > 3352) && (batteryValue <= 3447))	battery = 5;  //7.2v-7.4v
	else if((batteryValue > 3261) && (batteryValue <= 3352))	battery = 4;  //7.0v-7.2v
	else if((batteryValue > 3168) && (batteryValue <= 3261))	battery = 3;  //6.8v-7.0v
	else if((batteryValue > 3076) && (batteryValue <= 3168))	battery = 2;  //~6.6v-6.8v
	else 	battery = 1;
	
	return battery;
}

////////电量低指示//////////
void SendBetteryLow(void) 
{
	u8 i;
	
	if(battery >= BATTERYLEVEL) 
	{
		com3_txbuf[0] = '$'; com3_txbuf[1] = 0x19; com3_txbuf[2] = 0x01;
		for(i=3;i<18;i++)   com3_txbuf[i]=0x00;
		
		sendtocom3();
		
		Delay(30000);
		
		sendtocom3();
	}
	else
	{
		com3_txbuf[0] = '$'; com3_txbuf[1] = 0x19; com3_txbuf[2] = 0x00;
		for(i=3;i<18;i++)   com3_txbuf[i]=0x00;
		
		sendtocom3();
		
		Delay(30000);
		
		sendtocom3();
	}
	
}

