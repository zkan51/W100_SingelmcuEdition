#ifndef __CONFIG_H
#define __CONFIG_H
#include <stdlib.h>
#include <stdio.h>
#include "stm32f10x.h"//#include "stm32f10x_lib.h"
#include "stm32f10x_flash.h"
//#include "misc.h"

typedef unsigned long  UINT32;
typedef unsigned short UINT16;
typedef unsigned char  UINT8;

#define SI4463_ON()       GPIO_SetBits(GPIOA, GPIO_Pin_11) 			//SI4463��
#define SI4463_OFF()      GPIO_ResetBits(GPIOA, GPIO_Pin_11)  //SI4463��

#define GPS_ON()       GPIO_SetBits(GPIOB, GPIO_Pin_5) //GPS��
#define GPS_OFF()      GPIO_ResetBits(GPIOB, GPIO_Pin_5)  //GPS��

#define PA_ON()        GPIO_SetBits(GPIOA, GPIO_Pin_4) //���ſ�
#define PA_OFF()       GPIO_ResetBits(GPIOA, GPIO_Pin_4)  //���Ź�

#define LED_ON()       GPIO_SetBits(GPIOB, GPIO_Pin_15) //LED��
#define LED_OFF()      GPIO_ResetBits(GPIOB, GPIO_Pin_15)  //LED��

#define LED_RED_ON()       GPIO_SetBits(GPIOB, GPIO_Pin_7) //��LED��
#define LED_RED_OFF()      GPIO_ResetBits(GPIOB, GPIO_Pin_7)  //��LED��

//Configures the system clocks. ϵͳʱ�� 64M�� PCLK1:32M �� PCLK2��64M
// void RCC_Configuration(void);

// Configures the different GPIO ports. CAN TRX config�� SPI_IO
void GPIO_Configuration(void);

//Configures the NVIC and Vector Table base address.
//void NVIC_Configuration(void);

void SYS_CLK_UP(void);
void SYS_CLK_DOWN(void);



#endif

