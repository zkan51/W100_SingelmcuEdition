#ifndef  __DELAY_H__
#define  __DELAY_H__
#include "stm32f10x.h"

#define SystemCoreClock_48M 48000000
#define SystemCoreClock_8M	8000000

#define SysTick_CTRL_ENABLE_Pos             0
#define SysTick_CTRL_ENABLE_Msk            (1ul << SysTick_CTRL_ENABLE_Pos)

void delay_init(void);
void  delay (u32 nCount);
void delay_us (u32 nus);
void delay_ms (u16 nms);
#endif
