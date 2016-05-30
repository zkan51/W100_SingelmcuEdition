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

#define SI4463_ON()       GPIO_SetBits(GPIOA, GPIO_Pin_11) 			//SI4463开
#define SI4463_OFF()      GPIO_ResetBits(GPIOA, GPIO_Pin_11)  //SI4463关

#define GPS_ON()       GPIO_SetBits(GPIOB, GPIO_Pin_5) //GPS开
#define GPS_OFF()      GPIO_ResetBits(GPIOB, GPIO_Pin_5)  //GPS关

#define PA_ON()        GPIO_SetBits(GPIOA, GPIO_Pin_4) //功放开
#define PA_OFF()       GPIO_ResetBits(GPIOA, GPIO_Pin_4)  //功放关

#define LED_ON()       GPIO_SetBits(GPIOB, GPIO_Pin_15) //LED开
#define LED_OFF()      GPIO_ResetBits(GPIOB, GPIO_Pin_15)  //LED关

#define LED_RED_ON()       GPIO_SetBits(GPIOB, GPIO_Pin_7) //红LED开
#define LED_RED_OFF()      GPIO_ResetBits(GPIOB, GPIO_Pin_7)  //红LED关

//Configures the system clocks. 系统时钟 64M； PCLK1:32M ； PCLK2：64M
// void RCC_Configuration(void);

// Configures the different GPIO ports. CAN TRX config； SPI_IO
void GPIO_Configuration(void);

//Configures the NVIC and Vector Table base address.
//void NVIC_Configuration(void);

void SYS_CLK_UP(void);
void SYS_CLK_DOWN(void);



#endif

