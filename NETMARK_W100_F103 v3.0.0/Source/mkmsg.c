#include "main.h"
#include "stm32f10x_adc.h"	
#include "adc.h"

/***********************************************************
 * �������gmsg18
 * ����  ��������Ϣ18
 * ����  : ��
 * ���  : ��
 ***********************************************************/
void msg18(void)
{	
// 	jingdu=GPS.longitude; //���¾�γ����Ϣ
// 	weidu=GPS.latitude;
	//u8 i;
	
	Read_GPS_Info();
	
	if(direction >= 100)
	{
		direction = direction/100*100 + battery*10; //COG�ĸ�λ�������ָʾ
	}
	else
	{
		direction = battery*10;
	}
		
	//SendBetteryLow();//������ָʾ
	
	msg[0] =0x48 ;	         //6bit��ϢID:18;  2bitת��ָʾ����0
	msg[1] = MMSI >>22;	     //30bit�û�ID��MMSI���
	msg[2] = MMSI >>14;
	msg[3] = MMSI >> 6;
	msg[4] = MMSI << 2;  //��2bit����Ϊ0
	msg[5] = sog >> 8;  //��6bit����Ϊ0����2bit���溽��Ϊ0
	msg[6] = sog;			 //���溽��Ϊ0
	msg[7] = (jingdu >> 21)| 0x80; //���λΪλ��׼ȷ��1������7λ��ʼΪ���ȵ����λ
	msg[8] = (jingdu) >> 13;   
	msg[9] = (jingdu) >> 5;
	msg[10] = ((jingdu << 3) & 0xF8 )+ ((weidu>>24 )& 0x07); //�������ݵ�5λ��γ����Ϣ��3bit.
	msg[11] = (weidu) >> 16; 
	msg[12] = (weidu) >> 8;
	msg[13] = (weidu) >> 0;
	msg[14] = (u8)(direction >> 4); //0xE1;	   //COG:3600 ��8bit
	msg[15] = (u8)(direction << 4)|0x0F; //0x40 + (u8)(direction >> 5);		   //COG��4λ + ʵ�ʺ����4λ ʵ�ʺ�������Ϊȫ0
	msg[16] = 0xFF; //(u8)(direction << 3) + 0x07;		   //ʵ�ʺ���ĵ�5λ+ʱ�����3bit.ʵ�ʺ����ʱ�����������
	msg[17] = 0x84;	   //ʱ�����3bit:100;����2bit:00;B��װ�ñ�־1bit:1;B����ʾ����־1bit:0;B��DSC��־: 0  
	msg[18] = 0x08+((slot_increacement>>12)&0x01); // B������־�� 0��ͨ��״̬��3bit:000  ITDMA
	msg[19]=slot_increacement>>4;
	msg[20]=(slot_increacement<<4)+0x04;

	len=21;
}
/***********************************************************
 * �������gmsg24A
 * ����  ��������Ϣ24A
 * ����  : ��
 * ���  : ��
 ***********************************************************/
void msg24A(void)
{
	u8 i;
	msg[0] =0x60 ;	         //6bit��ϢID:24;  2bitת��ָʾ����00
	msg[1] = MMSI >>22;	     //30bit�û�ID��MMSI���
	msg[2] = MMSI >>14 ;
	msg[3] = MMSI >> 6;
	msg[4] = MMSI << 2;  //��2bitΪ���ֱ�ţ�00
	for(i=0;i<15;i++)   
		 msg[5+i] = boatnum[i+1];	 //MMSI��ֻ����,ASCII��
	len=20;
}
/***********************************************************
 * �������gmsg24B
 * ����  ��������Ϣ24B
 * ����  : ��
 * ���  : ��
 ***********************************************************/
void msg24B(void)
{
	msg[0] =0x60 ;	         //6bit��ϢID:24;  2bitת��ָʾ����00
	msg[1] = MMSI >>22;	     //30bit�û�ID��MMSI���
	msg[2] = MMSI >>14 ;
	msg[3] = MMSI >> 6;
	msg[4] = (MMSI << 2)|0x01;  //��2bitΪ���ֱ�ţ�01
	msg[5] = 0x00;	 //�������ͺͻ�������	
	msg[6] = Vender_ID[0]; //��Ӧ��ID 1:8
	msg[7] = Vender_ID[1]; // 9:16
	msg[8] = Vender_ID[2];  //  17:24
	msg[9] = Vender_ID[3];   //25:32
	msg[10] = Vender_ID[4]; 		//33:40
	msg[11] = Vender_ID[5] | Call_sign[0];    //��Ӧ��ID 41:42;���� 1:6
	msg[12] = Call_sign[1];       //���� 7:14
	msg[13] = Call_sign[2];       //���� 15:22
	msg[14] = Call_sign[3];								//���� 23:30
	msg[15] = Call_sign[4];       //���� 31:38
	msg[16] = Call_sign[5] | boatsize_a >> 5;   //���� 39:42 // �����ߴ�  A: 1-4
	msg[17] =	boatsize_a << 3 | boatsize_b >> 6; //A: 5-9 // B: 1-3  
	msg[18] = boatsize_b << 2 | boatsize_c >> 4;  // B: 4-9 // C:1-2
	msg[19] = boatsize_c << 4| boatsize_d >> 2;   // C: 3-6 // D: 1- 4
	msg[20] = boatsize_d << 6;										// D: 5,6        
	len=21;
}
