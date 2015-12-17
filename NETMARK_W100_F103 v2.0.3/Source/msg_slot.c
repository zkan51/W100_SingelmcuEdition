#include "main.h"
#include "stm32f10x_adc.h"
extern u8 t_cnt;  //声明在exti.c

static u8 msg_tmp[512]={0};  
static u8 msg_data[512]={0};//存储高斯插值前NRZI编码的消息
static u8 TS[3]={0x55,0x55,0x55};//训练序列
static u8 FLAG=0x7e;//帧头帧尾
static u16 crc_reg=0x0000; //存储CRC校验结果
static u8 valid_cnt_m18; // 消息中断有效bit数
static u8 valid_cnt_m24A;
static u8 valid_cnt_m24B;
//static u16 valid_cnt_m21;
static u8 num_cnt = 0;   //记录发送消息18的次数。
static u16 num_slot = 0;   //时隙号 
static u8 charge_state_num;
//extern u16 t_convert;  //gps.c中声明

extern u8 TIM3_CAM1_flag; //防止反复进入
extern u8 TIM3_CAM3_flag; //防止反复进入
extern u8 TIM3_CAM5_flag; //防止反复进入

extern u16 tim3_cnt;
extern u8 swchflag;
extern int tim2_cnt; //定时器2计数

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
	valid_cnt_m18=bit_stuffing();
	nrzi();
	for(i=0;i<256;i++)
	{
		if(i==0)
		{
			msg18_add[i]=msg_data[i+1]+msg_data[i]*2;
		}
		else if( i > 0 && i <= valid_cnt_m18)
		{
			msg18_add[i]=msg_data[i-1]*4+msg_data[i]*2+msg_data[i+1];
		}
		else
		{
			msg18_add[i]=8;
		}
	}
}
/****************************************************************
 * 函数名：msg24A_generating 
 * 描述  : 生成消息24A的时隙，存储在msg24A_add[]中
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
	valid_cnt_m24A=bit_stuffing(); //记录有效消息bit数
	nrzi();
	for(i=0;i<256;i++)
	{
				if(i==0)
				{
					msg24A_add[i]=msg_data[i+1]+msg_data[i]*2;
				}
				else if( i>0 && i<=valid_cnt_m24A)
				{
					msg24A_add[i]=msg_data[i-1]*4+msg_data[i]*2+msg_data[i+1];
				}
				else
				{
					msg24A_add[i]=8;
				}
	}	
}
/****************************************************************
 * 函数名：msg24B_generating 
 * 描述  : 生成消息24B的时隙，存储在msg24B_add[]中
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
	valid_cnt_m24B=bit_stuffing(); //记录有效消息bit数
	nrzi();
	for(i=0;i<256;i++)
	{
				if(i==0)
				{
					msg24B_add[i]=msg_data[i+1]+msg_data[i]*2;
				}
				else if( i>0 && i<=valid_cnt_m24B)
				{
					msg24B_add[i]=msg_data[i-1]*4+msg_data[i]*2+msg_data[i+1];
				}
				else
				{
					msg24B_add[i]=8;
				}
	}	
}

void msg_send1(void)
{
		u16 i=0,j=0,k=0;
		if(flag_m24==on)
		{
			var_m24=256*5-1;        //消息24B只生成，没有进行发射
			//var_m24=256*3*5-1;    //发射24B消息
			msg18_generating();
 		  //msg24A_generating();
 			//msg24B_generating();	
			for(i=0;i<256;i++)
			{
				j=i%256;
				k=i/256;
				switch (k)
				{
					case 0:
							if(j<valid_cnt_m18)
							{
								msg18_24AB_add[i]=msg18_add[j];
							}
							else
							{
								msg18_24AB_add[i]=16;
							}
						break;
					case 1:
							if(j<valid_cnt_m24A)
							{
								msg18_24AB_add[i]=msg24A_add[j];
							}
							else
							{
								msg18_24AB_add[i]=16;
							}
						break;
					case 2:
							if(j<valid_cnt_m24B)
							{
								msg18_24AB_add[i]=msg24B_add[j];
							}
							else
							{
								msg18_24AB_add[i]=16;
							}
						break;
					default:
						break;
				}		
			}
		}
		else
		{
			var_m24=256*5-1;
			msg18_generating();
			//msg24A_generating();
			for(i=0;i<256;i++)
			{
				if(i<valid_cnt_m18) 
				{
					msg18_24AB_add[i]=msg18_add[i];
				}
				else
				{
					msg18_24AB_add[i]=16;
				}
			}
		}
}

void msg_send2()   
{
		u16 i=0,j=0,k=0;
		if(flag_m24==on)
		{
			var_m24=512*5-1;        //消息24B只生成，没有进行发射
			//var_m24=256*3*5-1;    //发射24B消息
			//msg18_generating();
 		  msg24A_generating();
 			msg24B_generating();	
			for(i=0;i<512;i++)
			{
				j=i%256;
				k=i/256;
				switch (k)
				{
// 					case 0:
// 							if(j<valid_cnt_m18)
// 							{
// 								msg18_24AB_add[i]=msg18_add[j];
// 							}
// 							else
// 							{
// 								msg18_24AB_add[i]=16;
// 							}
// 						break;
					case 0:
							if(j<valid_cnt_m24A)
							{
								msg18_24AB_add[i]=msg24A_add[j];
							}
							else
							{
								msg18_24AB_add[i]=16;
							}
						break;
					case 1:
							if(j<valid_cnt_m24B)
							{
								msg18_24AB_add[i]=msg24B_add[j];
							}
							else
							{
								msg18_24AB_add[i]=16;
							}
						break;
					default:
						break;
				}		
			}
		}
		else
		{
			var_m24=256*5-1;
			//msg18_generating();
			msg24A_generating();
			for(i=0;i<256;i++)
			{
				if(i<valid_cnt_m24A) 
				{
					msg18_24AB_add[i]=msg24A_add[i];
				}
				else
				{
					msg18_24AB_add[i]=16;
				}
			}
		}
}

/****************************************************************
 * 函数名：send_on
 * 描述  : AIS发射函数，完成消息生成，分配时隙，定时器开启，等待发射。
 * 输入  : 无 
 * 输出  : 无
 * 调用  ：
 *****************************************************************/
void send_on(void)
{
//	u8 i;
	num_cnt++;//记录执行函数的次数
	if(num_slot>=SEND_INTERVAL)//防止发射时隙溢出
	{
		num_slot=SEND_INTERVAL/2;
	}
	
	//if((Get_Adc_Average(ADC_Channel_9,3) < 1500)) //非充电状态
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)==0x00)
	{
			if(task_flag2==on)	//发射条件：上次任务完成
			{
							LedFlash();
							TIM3_OFF();	//消息生成前关闭定时器T3

						// 		if((num_cnt%CH_SW)==0)// 信道切换,没发射两次AIS消息切换信道
						// 		{
						//  			flag_channel = (1-flag_channel); //flag_channel=(AIS_CHANNEL)(1-flag_channel);
						// 			Write_TX_Channel();
						// 		}
							
							task_flag1=off;
							task_flag2=off;
							
							TIM3_CAM5_flag = 0;  //TIM3标记变量清零
							TIM3_CAM3_flag= 0;
							TIM3_CAM1_flag= 0;
							
							swchflag = 0;
							tim3_cnt=0;
							TIM3_Configuration();  //打开定时器T3
							
				}
	}
	else //充电状态
	{
			charging_flag = on;
			TIM_Cmd(TIM2, DISABLE);
		 TIM_Cmd(TIM4, DISABLE);
			GPS_OFF();
			tim2_cnt = 0;
			TIM3_OFF();
			LED_ON();
			LED_RED_ON();
	}
}









