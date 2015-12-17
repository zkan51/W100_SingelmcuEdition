#ifndef __CHARGE_H
#define __CHARGE_H	
#include "stm32f10x.h"

void BackupRegisterInit(void);
void Enter_PowersaveMode(void);
u8 GetBatteryPower(void);
void SendBetteryLow(void);

#endif 
