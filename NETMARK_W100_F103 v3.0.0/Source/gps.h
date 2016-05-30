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
	long  latitude;  //经度
	long  longitude; //纬度
// 	int     latitude_Degree;	//度
// 	int		latitude_Cent;		//分
// 	int   	latitude_Second;    //秒
// 	int     longitude_Degree;	//度
// 	int		longitude_Cent;		//分
// 	int   	longitude_Second;   //秒
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
