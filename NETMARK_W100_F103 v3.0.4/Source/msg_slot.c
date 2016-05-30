#include "main.h"
#include "stm32f10x_adc.h"

static u8 msg_tmp[512]={0};  
static u8 msg_data[512]={0};//�洢��˹��ֵǰNRZI�������Ϣ
static u8 TS[3]={0x55,0x55,0x55};//ѵ������
static u8 FLAG=0x7e;//֡ͷ֡β
static u16 crc_reg=0x0000; //�洢CRCУ����

static u16 txLen = 0;
static u8 condition  = 0x30; // ������ɺ󣬷���ready״̬
static u8 size_of_FIFO = 64; // оƬ�з���FIFO������Ϊ64 bytes
static u8 tx_threshold = 40; // FIFO�пճ��Ŀռ�ﵽ40 bytesʱ������TFAE�ж�

u8 swchflag;
#define CH_SW   2 //�����ŵ��л���
/************************* *********************************
 * ???:msg_crc
 * ??  : ??????crc?????
 * ??  :????????ptr,????num
 * ??  : 16?CRC????
 **********************************************************/
u16 msg_crc(u8 *ptr,u8 num)
{
	u16 crc=0xffff;
	u8 i;
	u16 gx=0x1021;
	while(num--)
	{
		for(i=0x01;i!=0;i<<=1)
		{
			if((crc&0x8000)!=0)
			{
				crc<<=1;
				crc^=gx;
			}
			else
			{
				crc<<=1;
			}
			if(((*ptr)&i)!=0)
			{
				crc^=gx;
			}
		}
		ptr++;
	}
	return ~crc;
}
/**********************************************************
 * ???:bit_to_bytes_msg
 * ??  :???????bit??????crc??????????bit?????,??????msg_tmp[]?40???,
					 ??0~7????????,8~31???????,32~39???????,40~40+num1*8-1???,
					 40+num1*8~40+num1*8+15?crc???,40+num1*8+16~40+num1*8+16+7??????
 * ??  :??????????
 * ??  : ?
 * ??  :
 **********************************************************/
void bit_to_byte_msg(u8 *msg_ptr)
{	
	u8 i,num1;
	u16 crc=crc_reg;
	num1=len;	
	for(i=0;i<(num1)*8+55+24;i++)	
	{
		if(i<8)//????
			msg_tmp[i]=0;
		else if(i>=8&&i<32)//????
			msg_tmp[i]=TS[(i-8)/8]>>(7-(i-8)%8)&0x01;
		else if(i>=32&&i<40)//????
			msg_tmp[i]=FLAG>>(7-(i-32)%8)&0x01;
 		else if(i>=40&&i<40+num1*8)
			msg_tmp[i]=(msg_ptr[(i-40)/8]>>((i-40)%8))&0x01;  //??bit??,????
		else if(i>=40+num1*8&&i<40+num1*8+16)
			msg_tmp[i]=(u8)(crc>>(15-(i-40-num1*8)%16)&0x0001);
		else if(i>=40+num1*8+16&&i<40+num1*8+16+8)
			msg_tmp[i]=FLAG>>(7-(i-40-num1*8-16)%8)&0x01;
	}
}
/******************************************************
 * ???:bit_stuffing
 * ??  :?????????:??????????
 * ??  : ????20/21bytes
 * ??  : ?
 * ??  :
 ******************************************************/
u16 bit_stuffing(void)
{
	u16 i;
	u16 j,num;
	u16 cnt=len*8+16; 
	for(i=0;i<40;i++)
	{
		msg_data[i]=msg_tmp[i];
	}
	for(i=40,j=44;(i-40)<cnt;i++,j++)
	{
		while((i-40)<=3)
		{
			msg_data[i]=msg_tmp[i];
			i++;
		}
		msg_data[j]=msg_tmp[i];
		if((msg_data[j-4]==1)&&(msg_data[j-3]==1)&&(msg_data[j-2]==1)&&(msg_data[j-1]==1)&&(msg_data[j]==1))
		{
			j++;
			msg_data[j]=0;
		}			
	}
	while(i>=cnt+40&&i<cnt+40+8)
	{	
		msg_data[j]=msg_tmp[i];
		i++;
		j++;
	}
	num=j; //
	return num;
}
/******************************************************
 * ???:nrzi
 * ??  :?????????:??????????
 * ??  : ?
 * ??  : ?
 * ??  :
 ******************************************************/
void nrzi(void)
{
	u16 i;
	for(i=8;i<256;i++)
	{
		if(msg_data[i]==0)
		{
			msg_data[i]=1-msg_data[i-1];
		}
		else
		{
			msg_data[i]=msg_data[i-1];
		}
	}
}
/****************************************************************
 * ��������msg18_generating 
 * ����  : ������Ϣ18��ʱ϶���洢��msg18_add[]��
 * ����  : �� 
 * ���  : ��
 * ����  ��
 *****************************************************************/
void msg18_generating(void)
{
	u16 i;
	msg18();
	crc_reg=msg_crc(msg,len);
	bit_to_byte_msg(msg);
	bit_stuffing();
	nrzi();
}
/****************************************************************
 * ��������msg24A_generating 
 * ����  : ������Ϣ24A��ʱ϶���洢��msg24A[]��
 * ����  : �� 
 * ���  : ��
 * ����  ��
 *****************************************************************/
void msg24A_generating(void)
{
	u16 i;
	msg24A();
	crc_reg=msg_crc(msg,len);
	bit_to_byte_msg(msg);
	bit_stuffing(); //��¼��Ч��Ϣbit��
	nrzi();
	for(i=0;i<256;i++)
	{
		msg24_A[i] = msg_data[i];
	}
}

/****************************************************************
 * ��������msg24B_generating 
 * ����  : ������Ϣ24B��ʱ϶���洢��msg24B[]��
 * ����  : �� 
 * ���  : ��
 * ����  ��
 *****************************************************************/
void msg24B_generating(void)
{
	u16 i;
	msg24B();
	crc_reg=msg_crc(msg,len);
	bit_to_byte_msg(msg);
	bit_stuffing(); //��¼��Ч��Ϣbit��
	nrzi();
	for(i=0;i<256;i++)
	{
		msg24_B[i] = msg_data[i];
	}	
}

// ��������msg_dataÿ8����ת��Ϊһ���ֽڣ��洢��txBuf�����У����ڷ���
u8 bit2byte(u8 *a){ // a�ĳ���Ϊ8
		u8 b=0;
		u8 i;
	 for(i=0;i<8;i++)
		{
				b |= a[i]<<(7-i);
		}
		return b;
}

// ��Ϣ18
void msg_send18(void)
{
	u16 i=0;
	msg18_generating();
	 
	for(i=0;i<32;i++)
	{
			txBuf[i] = bit2byte(msg_data+i*8);
	}
}

// ��Ϣ24A+��Ϣ24B
void msg_send24()   
{
		u16 i=0;
		msg24A_generating();
		msg24B_generating();	
	
		for(i=0;i<32;i++)
		{
			txBuf[i] = bit2byte(msg24_A+i*8);
		}
		for(i=0;i<32;i++)
		{
			txBuf[i+32] = bit2byte(msg24_B+i*8);
		}		
}

/****************************************************************
 * ��������send_on
 * ����  : AIS���亯���������Ϣ���ɣ����͡�
 * ����  : �� 
 * ���  : ��
 * ����  ��
 *****************************************************************/
void send_on(void)
{
	if(sendTask == on)
	{
		//����1s
		LedFlash();
		for(swchflag=0; swchflag<2; swchflag++)
		{
			if(swchflag == 0)//��Ϣ18
			{
				SI4463_ON();
				delay_ms(20);
				SI446X_CONFIG_INIT();//����4463оƬ����
				
				txLen = 32;   //����AIS��Ϣ����ĳ���
				msg_send18(); //������Ϣ18
				PA_ON();      //�򿪹���
				delay_us(500); 
				
				SI446X_SEND_PACKET(txBuf, txLen, flag_channel, condition, size_of_FIFO, tx_threshold);
				
//USART_SendData(USART1,0x01);
//while(!(USART1->SR & USART_FLAG_TXE));
				
				PA_OFF();
				SI4463_OFF();
			}
			else         	 //��Ϣ24A+24B
			{
				
				SI4463_ON();
				delay_ms(20);
				SI446X_CONFIG_INIT();//����4463оƬ���				
				
				flag_channel = (1-flag_channel);  //ѡ����ͨ����0��161.975 ��1��162.025
				Write_TX_Channel();
				txLen = 64;
				msg_send24();       //������Ϣ24A+24B
				PA_ON();
				delay_us(500); 
				
//USART_SendData(USART1,txLen);
//while(!(USART1->SR & USART_FLAG_TXE));
				
				SI446X_SEND_PACKET(txBuf, txLen, flag_channel, condition, size_of_FIFO, tx_threshold);
				
//USART_SendData(USART1,0x03);
//while(!(USART1->SR & USART_FLAG_TXE));
				
				PA_OFF();
			}
		}
		
		SI4463_OFF();
		GPSIsOn();   //�ж��Ƿ�Ҫ����GPS
		sendTask = off;
	}
}









