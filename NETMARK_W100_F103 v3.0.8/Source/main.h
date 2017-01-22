#ifndef __MAIN_H
#define	__MAIN_H

#include "stm32f10x.h"
#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "math.h"

#include "exti.h"
#include "config.h"
#include "usart1.h"
#include "usart2.h"
#include "msg_slot.h"
#include "timer.h"
#include "gps.h"
#include "gps_algorithm.h"
#include "mkmsg.h"	
#include "cmdfunc.h"
#include "rtc.h"
#include "flashwr.h"
#include "power.h"
#include "adc.h"
#include "led.h"
#include "test.h"
#include "si4463.h"

#define SYSCLK_8M



void Delay(__IO u32 nCount);

// union dds_union
// {
// 	unsigned long dds_data32;  //高地址存放高位，低地址存放低位。
// 	u8 dds_data8[4];
// };

//////msg21/////
extern u8 dev_type;//助航设备类型 5bit
extern u8 dev_name[54];//助航设备名称 120bit+(0~84)bit
extern u8 dev_local[5]; //尺寸位置参考点 30bit
extern u8 dev_time; //时戳 6bit
extern u8 dev_state; //AtoN状态 8bit
extern u8 dev_etype; //电子定位装置类型 4bit
////////////////

typedef enum
{ 
	off=0,
	on
}ais_status;

extern unsigned long MMSI;
extern u8 boatnum[16];
extern u8 CodeNum[6]; //密码
extern u8 Vender_ID[6];
extern u8 Call_sign[6];
extern u16 boatlong; 
extern u8 boatwidth; 
extern u16 boatsize_a; 
extern u16 boatsize_b; 
extern u8 boatsize_c;	
extern u8 boatsize_d;	
extern unsigned long jingdu;
extern unsigned long weidu;
extern unsigned int sog;
extern unsigned int direction;
extern unsigned long gpstime;
extern u8 msg[45];
extern GPS_INFO   GPS;
extern u8 len; //输入消息的字节数
extern u16 slot_increacement; //随机时隙增量，由随机化程序产生
extern ais_status sendTask;//发送任务完成标志 !!初值必须置为1,用来标识是否进入时隙号随机化函数； 


//extern union dds_union dds_word_update[16][5];
//extern char  dds_word_real_num[8][5];
extern int  dds_word_real[9][5];
// extern u16 dds_w1[17][5];
// extern u16 dds_w2[17][5];
// extern u16 dds_w3[17][5];
// extern u16 dds_w4[17][5];

extern u8 msg18_24AB_add[768];
extern u8 msg18_add[256];
extern u8 msg24_A[256];
extern u8 msg24_B[256];
extern u8 msg21_add[256*2];

extern u16 interval_s;  //发射间隔计数，变化值
extern u16 interval_num; //发射间隔计数
extern u16 gps_invalid; //gps失效的次数标志
//extern u16 gps_invalid_num; //失效次数值
//extern u16 time_o;//GPS前次时间存储
extern u16 intervalA;  //间隔计数次数 固定值
extern u16 intervalB;  //间隔计数次数 超时值
extern u8 cogsel;

extern ais_status flag_m24;
//extern u16 msequence_poly;	//上位机写入 与控制的接口。
extern u8 flag_channel;		 //信道选择标志位,初始化为信道1
extern u8 flag_charging; //充电标志位
extern u8 code_cnt; //密码正确与否标志。code_flag1=6时密码正确
extern ais_status encryption; //是否写过密码标志，默认为未写过。

extern u16 activation_flag;//激活标志
extern u32 ChipUniqueID_Flash[3];   //芯片96bit UID

extern u8 rxflag; //发送标志,防止发送时进入中断
extern u8 status;	//用于判断接收/发送状态
extern u8 tx1buf[31]; 	   //串口1发送缓冲

extern u8 openflag;

extern u8 battery;
extern u8 txBuf[64];

extern int tim2_cnt; //定时器2计数
extern u8 swchflag;
extern unsigned long Time;
extern u8 AgencyName[16];
#define BATTERYLEVEL 4

#endif /* __MAIN_H */
