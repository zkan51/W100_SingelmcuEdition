#ifndef __FLASHWR_H
#define	__FLASHWR_H

#include "stm32f10x.h"
#include <stdio.h>

#define FLASH_PAGE_SIZE    ((u16)0x400) //1024  1K 

void ReadFlashInit(void); 

void WriteflashBoatnum(void); 
void ReadflashBoatnum(void);

void WriteflashMMSI(void); 
void ReadflashMMSI(void); 

void WriteflashCode(void); 
void ReadflashCode(void); 

void ReadflashMPoly(void);
void WriteflashMPoly(void);

void ReadflashMPeriod(void); 
void WriteflashMPeriod(void);

void WriteflashMUID(void);
void ReadflashMUID(void);

void WriteflashMActivation(void);
void ReadflashMActivation(void);

void WriteflashBoatInfo(void);
void ReadflashBoatInfo(void);

void Write_Flash_GPS_Info(void);
void Read_Flash_GPS_Info(void);
void Write_GPS_Info(void);
void Read_GPS_Info(void);
void Write_TX_Channel(void);
void Read_TX_Channel(void);

void ReadBackup(void);  //掉电后读入变量值
void WriteBackup(void);

void Read_OpenOnce(void);

void Write_Flash_Cogsel(void);
void Read_Flash_Cogsel(void);

#endif /* __FLASHWR_H */
