#ifndef __TIMER_H
#define	__TIMER_H

#include "stm32f10x.h"

typedef enum
{ 
	AIS_CHANNEL_1=0,
	AIS_CHANNEL_2=1
}AIS_CHANNEL;

typedef enum
{ 
	m24_off=0,
	m24_on=1
}M24_SEND_FLAG;
										
void TIM3_NVIC_Configuration(void);
void TIM3_OFF(void);

void TIM3_Configuration(void);
void TIM3_Init(void);

void TIM4_Configuration(void);
void TIM4_OFF(void);
void TIM4_ON(void);

void TIM6_Configuration(void);
void TIM6_OFF(void);

void TIM2_Configuration(void);
#endif /* __TIMER_H */
