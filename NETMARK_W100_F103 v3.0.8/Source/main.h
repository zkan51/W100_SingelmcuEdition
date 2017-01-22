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
// 	unsigned long dds_data32;  //�ߵ�ַ��Ÿ�λ���͵�ַ��ŵ�λ��
// 	u8 dds_data8[4];
// };

//////msg21/////
extern u8 dev_type;//�����豸���� 5bit
extern u8 dev_name[54];//�����豸���� 120bit+(0~84)bit
extern u8 dev_local[5]; //�ߴ�λ�òο��� 30bit
extern u8 dev_time; //ʱ�� 6bit
extern u8 dev_state; //AtoN״̬ 8bit
extern u8 dev_etype; //���Ӷ�λװ������ 4bit
////////////////

typedef enum
{ 
	off=0,
	on
}ais_status;

extern unsigned long MMSI;
extern u8 boatnum[16];
extern u8 CodeNum[6]; //����
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
extern u8 len; //������Ϣ���ֽ���
extern u16 slot_increacement; //���ʱ϶��������������������
extern ais_status sendTask;//����������ɱ�־ !!��ֵ������Ϊ1,������ʶ�Ƿ����ʱ϶������������� 


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

extern u16 interval_s;  //�������������仯ֵ
extern u16 interval_num; //����������
extern u16 gps_invalid; //gpsʧЧ�Ĵ�����־
//extern u16 gps_invalid_num; //ʧЧ����ֵ
//extern u16 time_o;//GPSǰ��ʱ��洢
extern u16 intervalA;  //����������� �̶�ֵ
extern u16 intervalB;  //����������� ��ʱֵ
extern u8 cogsel;

extern ais_status flag_m24;
//extern u16 msequence_poly;	//��λ��д�� ����ƵĽӿڡ�
extern u8 flag_channel;		 //�ŵ�ѡ���־λ,��ʼ��Ϊ�ŵ�1
extern u8 flag_charging; //����־λ
extern u8 code_cnt; //������ȷ����־��code_flag1=6ʱ������ȷ
extern ais_status encryption; //�Ƿ�д�������־��Ĭ��Ϊδд����

extern u16 activation_flag;//�����־
extern u32 ChipUniqueID_Flash[3];   //оƬ96bit UID

extern u8 rxflag; //���ͱ�־,��ֹ����ʱ�����ж�
extern u8 status;	//�����жϽ���/����״̬
extern u8 tx1buf[31]; 	   //����1���ͻ���

extern u8 openflag;

extern u8 battery;
extern u8 txBuf[64];

extern int tim2_cnt; //��ʱ��2����
extern u8 swchflag;
extern unsigned long Time;
extern u8 AgencyName[16];
#define BATTERYLEVEL 4

#endif /* __MAIN_H */
