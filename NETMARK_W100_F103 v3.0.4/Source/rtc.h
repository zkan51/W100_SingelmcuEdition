#ifndef __RTC_H
#define	__RTC_H


#include "stm32f10x.h"

/* ������������������Ļ�,PC13�ͻ����Ƶ��ΪRTC Clock/64��ʱ�� */   
//#define RTCClockOutput_Enable  /* RTC Clock/64 is output on tamper pin(PC.13) */

static void RTC_NVIC_Configuration(void);
static void RTC_Configuration(void);
void RTC_Init(void);
void SoftReset(void);
uint16_t Get_RTC_Counter(void);

#endif /* __XXX_H */
