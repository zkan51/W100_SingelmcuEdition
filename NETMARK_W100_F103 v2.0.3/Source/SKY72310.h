#ifndef _SKY72310_H_
#define _SKY72310_H_

#include "stm32f10x.h"
#include <stdio.h>

#define SPI1_CS_LOW()       GPIO_ResetBits(GPIOB, GPIO_Pin_0) 
#define SPI1_CS_HIGH()      GPIO_SetBits(GPIOB, GPIO_Pin_0) 

void SKY72310_Init(void);
void SKY72310_Config(void);
void SPI1_SendByte(u16 byte);
void SKY72310_Out(unsigned char Address,short data);

#endif

