#ifndef __Lowpower_H
#define __Lowpower_H	 
#include <stm32f10x.h>
#include "stm32f10x_pwr.h"
#include "stm32f10x_rcc.h"


#define SCB_SysCtrl              ((uint32_t)0xE000ED10)

void  EnterSleep(void );

void PWR_EnterSLEEPMode(u32 SysCtrl_Set, u8 PWR_SLEEPEntry);






#endif

