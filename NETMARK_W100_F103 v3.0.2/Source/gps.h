#ifndef __GPS_H_
#define __GPS_H_

#include "stm32f10x.h"//#include "stm32f10x_type.h"

typedef struct{
// 	int year;  
// 	int month; 
// 	int  day;
// 	int hour;
// 	int minute;
	int second;
}DATE_TIME;

typedef struct{
	long  latitude;  //����
	long  longitude; //γ��
// 	int     latitude_Degree;	//��
// 	int		latitude_Cent;		//��
// 	int   	latitude_Second;    //��
// 	int     longitude_Degree;	//��
// 	int		longitude_Cent;		//��
// 	int   	longitude_Second;   //��
//	int     satellite;
	u8 	NS;
	u8 	EW;
	u16 second;
}GPS_INFO;

int GPS_RMC_Parse(char *line, GPS_INFO *GPS);
//int GPS_GGA_Parse(char *line, GPS_INFO *GPS);
void UTC2BTC(DATE_TIME *date_time);
//void Test_GPS(void);

#endif/*__GPS_H_*/
