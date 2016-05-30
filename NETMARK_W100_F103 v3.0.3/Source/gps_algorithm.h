#ifndef __ALGORITHM_H_
#define __ALGORITHM_H_

#include "stm32f10x.h"

u8 GetComma(u8 num,char* str);
double Get_Double_Number(char *s);
double Get_Double_Number_sog(char *s); 
double Get_Double_Number_direction(char *s);
double Get_Double_Number_time(char *s);
double Str_To_Double(char *buf);

//void RefreshGPS(void);

#endif /*__ALGORITHM_H_*/
