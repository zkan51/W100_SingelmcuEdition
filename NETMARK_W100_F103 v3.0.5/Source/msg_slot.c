#include "main.h"
#include "stm32f10x_adc.h"

static u8 msg_tmp[512]={0};  
static u8 msg_data[512]={0};//存储高斯插值前NRZI编码的消息
static u8 TS[3]={0x55,0x55,0x55};//训练序列
static u8 FLAG=0x7e;//帧头帧尾
static u16 crc_reg=0x0000; //存储CRC校验结果

static u16 txLen = 0;
static u8 condition  = 0x30; // 发射完成后，返回ready状态
static u8 size_of_FIFO = 64; // 芯片中发射FIFO的容量为64 bytes
static u8 tx_threshold = 40; // FIFO中空出的空间达到40 bytes时，产生TFAE中断

u8 swchflag;
#define CH_SW   2 //定义信道切换宏
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
 * 函数名：msg18_generating 
 * 描述  : 生成消息18的时隙，存储在msg18_add[]中
 * 输入  : 无 
 * 输出  : 无
 * 调用  ：
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
 * 函数名：msg24A_generating 
 * 描述  : 生成消息24A的时隙，存储在msg24A[]中
 * 输入  : 无 
 * 输出  : 无
 * 调用  ：
 *****************************************************************/
void msg24A_generating(void)
{
	u16 i;
	msg24A();
	crc_reg=msg_crc(msg,len);
	bit_to_byte_msg(msg);
	bit_stuffing(); //记录有效消息bit数
	nrzi();
	for(i=0;i<256;i++)
	{
		msg24_A[i] = msg_data[i];
	}
}

/****************************************************************
 * 函数名：msg24B_generating 
 * 描述  : 生成消息24B的时隙，存储在msg24B[]中
 * 输入  : 无 
 * 输出  : 无
 * 调用  ：
 *****************************************************************/
void msg24B_generating(void)
{
	u16 i;
	msg24B();
	crc_reg=msg_crc(msg,len);
	bit_to_byte_msg(msg);
	bit_stuffing(); //记录有效消息bit数
	nrzi();
	for(i=0;i<256;i++)
	{
		msg24_B[i] = msg_data[i];
	}	
}

// 将比特流msg_data每8比特转换为一个字节，存储到txBuf数组中，用于发射
u8 bit2byte(u8 *a){ // a的长度为8
		u8 b=0;
		u8 i;
	 for(i=0;i<8;i++)
		{
				b |= a[i]<<(7-i);
		}
		return b;
}

// 消息18
void msg_send18(void)
{
	u16 i=0;
	msg18_generating();
	 
	for(i=0;i<32;i++)
	{
			txBuf[i] = bit2byte(msg_data+i*8);
	}
}

// 消息24A+消息24B
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
 * 函数名：send_on
 * 描述  : AIS发射函数，完成消息生成，发送。
 * 输入  : 无 
 * 输出  : 无
 * 调用  ：
 *****************************************************************/
void send_on(void)
{
	if(sendTask == on)
	{
		//亮灯1s
		LedFlash();
		SI4463_ON();
		delay_ms(20);
		SI446X_CONFIG_INIT();//配置4463芯片参数
		for(swchflag=0; swchflag<2; swchflag++)
		{
			if(swchflag == 0)//消息18
			{
				txLen = 32;   //发送AIS消息数组的长度
				msg_send18(); //生成消息18
				PA_ON();      //打开功放
				delay_ms(50); 
				
				SI446X_SEND_PACKET(txBuf, txLen, flag_channel, condition, size_of_FIFO, tx_threshold);		
			}
			else         	 //消息24A+24B
			{
				
				flag_channel = (1-flag_channel);  //选择发送通道，0：161.975 ，1：162.025
				Write_TX_Channel();
				txLen = 64;
				msg_send24();       //生成消息24A+24B
				delay_ms(50); 
				
				SI446X_SEND_PACKET(txBuf, txLen, flag_channel, condition, size_of_FIFO, tx_threshold);
			
				PA_OFF();
			}
		}
		
		SI4463_OFF();
		GPSIsOn();   //判断是否要开启GPS
		sendTask = off;
	}
}









