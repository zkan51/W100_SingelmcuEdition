#ifndef __USART3_H
#define	__USART3_H

#include "stm32f10x.h"
#include <stdio.h>
#include "misc.h"

void USART3_NVIC_Configuration(void);
void USART3_Config(void);
void sendtocom3(void);
extern u8 com3_txbuf[31];
extern void Usart3GetCommand(void);
void test(u8 data);

#endif /* __USART3_H */
