#ifndef _SPI_H_
#define _SPI_H_
#include "stm32f10x.h"
#include "stdio.h"
#include "delay.h"

#define SOFTSPI 1 	 //为1：表示软件SPI， 为0,表示硬件SPI
void SPI1IO_Init(void);			  	    													  
void SPI1_Init(void);			 //初始化SPI口
void SPI1_SetSpeed(u8 SpeedSet); //设置SPI速度   	 
u8 SPI_ExchangeByte(u8 TxData);  //SPI总线读写一个字节 	 
#endif

