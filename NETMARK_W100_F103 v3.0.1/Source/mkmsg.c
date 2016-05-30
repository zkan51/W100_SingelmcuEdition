#include "main.h"
#include "stm32f10x_adc.h"	
#include "adc.h"

/***********************************************************
 * 函数名msg18
 * 描述  ：生成消息18
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void msg18(void)
{	
// 	jingdu=GPS.longitude; //更新经纬度信息
// 	weidu=GPS.latitude;
	//u8 i;
	
	Read_GPS_Info();
	
	if(direction >= 100)
	{
		direction = direction/100*100 + battery*10; //COG的个位加入电量指示
	}
	else
	{
		direction = battery*10;
	}
		
	//SendBetteryLow();//电量低指示
	
	msg[0] =0x48 ;	         //6bit消息ID:18;  2bit转发指示符：0
	msg[1] = MMSI >>22;	     //30bit用户ID，MMSI编号
	msg[2] = MMSI >>14;
	msg[3] = MMSI >> 6;
	msg[4] = MMSI << 2;  //低2bit备用为0
	msg[5] = sog >> 8;  //高6bit备用为0，低2bit地面航速为0
	msg[6] = sog;			 //地面航速为0
	msg[7] = (jingdu >> 21)| 0x80; //最高位为位置准确度1，，低7位开始为经度的最高位
	msg[8] = (jingdu) >> 13;   
	msg[9] = (jingdu) >> 5;
	msg[10] = ((jingdu << 3) & 0xF8 )+ ((weidu>>24 )& 0x07); //经度数据低5位，纬度信息高3bit.
	msg[11] = (weidu) >> 16; 
	msg[12] = (weidu) >> 8;
	msg[13] = (weidu) >> 0;
	msg[14] = (u8)(direction >> 4); //0xE1;	   //COG:3600 高8bit
	msg[15] = (u8)(direction << 4)|0x0F; //0x40 + (u8)(direction >> 5);		   //COG高4位 + 实际航向高4位 实际航向设置为全0
	msg[16] = 0xFF; //(u8)(direction << 3) + 0x07;		   //实际航向的低5位+时间戳高3bit.实际航向和时间戳都不可用
	msg[17] = 0x84;	   //时间戳低3bit:100;备用2bit:00;B类装置标志1bit:1;B类显示器标志1bit:0;B类DSC标志: 0  
	msg[18] = 0x08+((slot_increacement>>12)&0x01); // B类带宽标志： 0；通信状态高3bit:000  ITDMA
	msg[19]=slot_increacement>>4;
	msg[20]=(slot_increacement<<4)+0x04;

	len=21;
}
/***********************************************************
 * 函数名msg24A
 * 描述  ：生成消息24A
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void msg24A(void)
{
	u8 i;
	msg[0] =0x60 ;	         //6bit消息ID:24;  2bit转发指示符：00
	msg[1] = MMSI >>22;	     //30bit用户ID，MMSI编号
	msg[2] = MMSI >>14 ;
	msg[3] = MMSI >> 6;
	msg[4] = MMSI << 2;  //低2bit为部分编号：00
	for(i=0;i<15;i++)   
		 msg[5+i] = boatnum[i+1];	 //MMSI船只名称,ASCII码
	len=20;
}
/***********************************************************
 * 函数名msg24B
 * 描述  ：生成消息24B
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void msg24B(void)
{
	msg[0] =0x60 ;	         //6bit消息ID:24;  2bit转发指示符：00
	msg[1] = MMSI >>22;	     //30bit用户ID，MMSI编号
	msg[2] = MMSI >>14 ;
	msg[3] = MMSI >> 6;
	msg[4] = (MMSI << 2)|0x01;  //低2bit为部分编号：01
	msg[5] = 0x00;	 //船舶类型和货物类型	
	msg[6] = Vender_ID[0]; //供应商ID 1:8
	msg[7] = Vender_ID[1]; // 9:16
	msg[8] = Vender_ID[2];  //  17:24
	msg[9] = Vender_ID[3];   //25:32
	msg[10] = Vender_ID[4]; 		//33:40
	msg[11] = Vender_ID[5] | Call_sign[0];    //供应商ID 41:42;呼号 1:6
	msg[12] = Call_sign[1];       //呼号 7:14
	msg[13] = Call_sign[2];       //呼号 15:22
	msg[14] = Call_sign[3];								//呼号 23:30
	msg[15] = Call_sign[4];       //呼号 31:38
	msg[16] = Call_sign[5] | boatsize_a >> 5;   //呼号 39:42 // 船舶尺寸  A: 1-4
	msg[17] =	boatsize_a << 3 | boatsize_b >> 6; //A: 5-9 // B: 1-3  
	msg[18] = boatsize_b << 2 | boatsize_c >> 4;  // B: 4-9 // C:1-2
	msg[19] = boatsize_c << 4| boatsize_d >> 2;   // C: 3-6 // D: 1- 4
	msg[20] = boatsize_d << 6;										// D: 5,6        
	len=21;
}
