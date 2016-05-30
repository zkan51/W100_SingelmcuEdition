#ifndef __USART1_H
#define	__USART1_H

#include "stm32f10x.h"
#include <stdio.h>

void USART1_Config(void);
void com1sendback(void);
void Usart1GetCommand(void);
void USART1_printf(USART_TypeDef* USARTx, uint8_t *Data,...);
void com1test(void);

#endif /* __USART1_H */
