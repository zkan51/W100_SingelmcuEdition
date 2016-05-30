#ifndef _SPI_H_
#define _SPI_H_
#include "stm32f10x.h"
#include "stdio.h"
#include "delay.h"

#define SOFTSPI 1 	 //Ϊ1����ʾ���SPI�� Ϊ0,��ʾӲ��SPI
void SPI1IO_Init(void);			  	    													  
void SPI1_Init(void);			 //��ʼ��SPI��
void SPI1_SetSpeed(u8 SpeedSet); //����SPI�ٶ�   	 
u8 SPI_ExchangeByte(u8 TxData);  //SPI���߶�дһ���ֽ� 	 
#endif

