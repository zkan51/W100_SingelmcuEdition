#ifndef __CHARGE_H
#define __CHARGE_H	
#include "stm32f10x.h"

void BackupRegisterInit(void);
void Sys_Standby(void);
u8 GetBatteryPower(void);
void chargingState(void);
void ExitCharging(void);
#endif 
