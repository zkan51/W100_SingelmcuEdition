#include "main.h"
#include <stdarg.h>
#include "stm32f10x_dma.h"
#include "usart3.h"

//static u8 com3_start;
//static u8 com3_stop;
//static u8 com3_num;

#define UART_RX1_LEN     100 
static u8 com1_rxbuf[UART_RX1_LEN];  //串口1接收缓存
u8 tx1buf[31]; 	   //串口1发送缓冲
extern u8 swchflag;

 /***********************************************************
 * 函数名: USART1_NVIC_Configuration
 * 描述  ：配置串口1中断
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
static void USART1_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  													
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

static void UART1_DMA_Configuration(void)
{
	  DMA_InitTypeDef DMA_InitStructure;
	  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  //启动DMA时钟  

    DMA_DeInit(DMA1_Channel5); 	//USART1 DMA1通道5配置   
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);//外设地址  
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)com1_rxbuf;     //内存地址  
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;            //dma传输方向从外设到内存 
    DMA_InitStructure.DMA_BufferSize = UART_RX1_LEN;               //设置DMA在传输时缓冲区的长度  
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//设置DMA的外设地址不变 
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;      //设置DMA的内存递增模式  
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据字长 8位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;    //内存数据字长8位   
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                //设置DMA的传输模式   
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;      //设置DMA的优先级别 最高
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                 //设置DMA的2个memory中的变量互相访问  
    DMA_Init(DMA1_Channel5,&DMA_InitStructure);  
  
    DMA_Cmd(DMA1_Channel5,ENABLE);
}

 /***********************************************************
 * 函数名: USART1_Config
 * 描述  ：配置串口1，9600波特率
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void USART1_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	UART1_DMA_Configuration();
	USART1_NVIC_Configuration();
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

  /* Configure USART1 Tx (PA.09) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);    
  /* Configure USART1 Rx (PA.10) as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	  
	/* USART1 mode config */
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	USART_ITConfig(USART1,USART_IT_TC,DISABLE);    //禁止 
	USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);  //禁止 
	USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);   //开启

	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);	//采用DMA方式接收  
	
  USART_Cmd(USART1, ENABLE);
}

/************************************************************
 * 函数名：fputc
 * 描述  ：重定向c库函数printf到USART1
 * 输入  ：无
 * 输出  ：无
 * 调用  ：由printf调用
 ************************************************************/
int fputc(int ch, FILE *f)
{
/* 将Printf内容发往串口 */
  USART_SendData(USART1, (unsigned char) ch);
  while (!(USART1->SR & USART_FLAG_TXE));
 
  return (ch);
}
/*************************************************************
 * 函数名：itoa
 * 描述  ：将整形数据转换成字符串
 * 输入  ：-radix =10 表示10进制，其他结果为0
 *         -value 要转换的整形数
 *         -buf 转换后的字符串
 *         -radix = 10
 * 输出  ：无
 * 返回  ：无
 * 调用  ：被USART1_printf()调用
 ************************************************************/
static char *itoa(int value, char *string, int radix)
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;

    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }

    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';

        /* Make the value positive. */
        value *= -1;
    }

    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }

    /* Null terminate the string. */
    *ptr = 0;

    return string;

} /* NCL_Itoa */

/*********************************************************************
 * 函数名：USART1_printf
 * 描述  ：格式化输出，类似于C库中的printf，但这里没有用到C库
 * 输入  ：-USARTx 串口通道，这里只用到了串口1，即USART1
 *		     -Data   要发送到串口的内容的指针
 *			   -...    其他参数
 * 输出  ：无
 * 返回  ：无 
 * 调用  ：外部调用
 *         典型应用USART1_printf( USART1, "\r\n this is a demo \r\n" );
 *            		 USART1_printf( USART1, "\r\n %d \r\n", i );
 *            		 USART1_printf( USART1, "\r\n %s \r\n", j );
 ***********************************************************************/
void USART1_printf(USART_TypeDef* USARTx, uint8_t *Data,...)
{
	const char *s;
  int d;   
  char buf[16];

  va_list ap;
  va_start(ap, Data);

	while ( *Data != 0)     // 判断是否到达字符串结束符
	{				                          
		if ( *Data == 0x5c )  //'\'
		{									  
			switch ( *++Data )
			{
				case 'r':							          //回车符
					USART_SendData(USARTx, 0x0d);
					Data ++;
					break;

				case 'n':							          //换行符
					USART_SendData(USARTx, 0x0a);	
					Data ++;
					break;
				
				default:
					Data ++;
				    break;
			}			 
		}
		else if ( *Data == '%')
		{									  //
			switch ( *++Data )
			{				
				case 's':										  //字符串
					s = va_arg(ap, const char *);
          for ( ; *s; s++) 
					{
						USART_SendData(USARTx,*s);
						while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
          }
					Data++;
          break;

        case 'd':										//十进制
          d = va_arg(ap, int);
          itoa(d, buf, 10);
          for (s = buf; *s; s++) 
					{
						USART_SendData(USARTx,*s);
						while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
          }
					Data++;
          break;
				 default:
						Data++;
				    break;
			}		 
		} /* end of else if */
		else USART_SendData(USARTx, *Data++);
		while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
	}
}

void USART1_IRQHandler(void)
{   
	uint32_t Length = 0;//,i=0;
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)  
	{  
		DMA_Cmd(DMA1_Channel5,DISABLE); 
		Length = USART1->SR;  
		Length = USART1->DR; //清USART_IT_IDLE标志  
		
		Usart1GetCommand();
		
// 			Length = UART_RX1_LEN - DMA_GetCurrDataCounter(DMA1_Channel5);		
// 			for(i=0;i<Length;i++)
// 			{
// 				printf("%c",Uart_Rx1[i]);
// 			}
		
		DMA1_Channel5->CNDTR = UART_RX1_LEN;//重装填,并让接收地址偏址从0开始
		DMA_Cmd(DMA1_Channel5, ENABLE);//处理完,重开DMA   
	}
	__nop();   	
	
}

void com1sendback(void)   
{
	u8 i;
	
	for(i=0;i<18;i++)	
	{
		USART_SendData(USART1, tx1buf[i]);
		while (!(USART1->SR & USART_FLAG_TXE)) ;
	}
}


void Usart1GetCommand(void)  //串口1接收
{
	u8 i;

	if(com1_rxbuf[0] == 0x24)
	{
		switch(com1_rxbuf[1])	
		{
			case 0x01://向网位仪写船名
			{
				if(charging_flag == on)
				{
					if(code_flag2==on)  //加密情况
					{
						if(code_flag1==on)  //密码正确
						{
								for(i=0;i<16;i++)
									boatnum[i] = com1_rxbuf[i+2];
								WriteflashBoatnum();
								ReadflashBoatnum();
							
								tx1buf[0] = '$'; tx1buf[1] = 0x01;tx1buf[2] = 0x01; //应答
								for(i=3;i<18;i++)
									tx1buf[i]=0x00;
								com1sendback();
							
								//code_flag1=off; //设置密码错误标志
						}
						else  //密码错误
						{
							tx1buf[0] = '$'; tx1buf[1] = 0x01; tx1buf[2] = 0x02; //应答 请先解密
							for(i=3;i<18;i++)
								tx1buf[i]=0x00;
							com1sendback();
							
							code_flag1=off; //设置密码错误标志
						}
					}
					else //未加密情况
					{
						for(i=0;i<16;i++)
							boatnum[i] = com1_rxbuf[i+2];
						WriteflashBoatnum();
						ReadflashBoatnum();
						
						tx1buf[0] = '$'; tx1buf[1] = 0x01;tx1buf[2] = 0x01; //应答
						for(i=3;i<18;i++)
							tx1buf[i]=0x00;
						com1sendback();
					}
				}
			}
			break;
			case 0x02://向网位仪写MMSI号
			{
				if(charging_flag == on)
				{
					if(code_flag2==on)   //加密情况
					{
						if(code_flag1==on)
						{
							MMSI=0;
							MMSI += (com1_rxbuf[2] << 24); //+ (rxbuf[2] << 16) + (rxbuf[3] << 8) + rxbuf[4];
							MMSI += (com1_rxbuf[3] << 16);
							MMSI += (com1_rxbuf[4] << 8);
							MMSI += com1_rxbuf[5];
							WriteflashMMSI();
							ReadflashMMSI();
					
							tx1buf[0] = '$'; tx1buf[1] = 0x02;tx1buf[2] = 0x01; //正确写入mmsi
							for(i=3;i<18;i++)
								tx1buf[i]=0x00;
							com1sendback();
							//code_flag1=off;  //设置密码错误标志
						}
						else
						{
							
							tx1buf[0] = '$'; tx1buf[1] = 0x02;tx1buf[2] = 0x02; //请输入密码
							for(i=3;i<18;i++)
								tx1buf[i]=0x00;
							
							com1sendback();
							code_flag1=off; //设置密码错误标志
						}
					}
					else
					{
						MMSI=0;
						MMSI += (com1_rxbuf[2] << 24); //+ (rxbuf[2] << 16) + (rxbuf[3] << 8) + rxbuf[4];
						MMSI += (com1_rxbuf[3] << 16);
						MMSI += (com1_rxbuf[4] << 8);
						MMSI += com1_rxbuf[5];

						WriteflashMMSI();
						ReadflashMMSI();
						tx1buf[0] = '$'; tx1buf[1] = 0x02;tx1buf[2] = 0x01; //应答
						for(i=3;i<18;i++)
							tx1buf[i]=0x00;
						
						com1sendback();
					}
				}
			}
			break;
			
			case 0x04://向网位仪读取船名
			{
				if(charging_flag == on)
				{
					tx1buf[0] = '$'; tx1buf[1] = 0x04;

					for(i=0;i<16;i++)   tx1buf[i+2]=boatnum[i];
					com1sendback();
				}

			}
			break;
				
			case 0x05://向网位仪读取MMSI号
			{
				if(charging_flag == on)
				{
					tx1buf[0] = '$';tx1buf[1] = 0x05;
					for(i=0;i<4;i++)
						tx1buf[i+2] = MMSI >> (24 - i*8);
					for(i=6;i<18;i++)
						tx1buf[i]=0x00;

					com1sendback();		
				}
			}
			break;
			
			case 0x0A://向网位仪加密
			{
				if(charging_flag == on)
				{
					Is_Coded();
					if(code_flag2==off)          //若设备未加密，加密
					{
						for(i=0;i<6;i++)
						CodeNum[i] = com1_rxbuf[i+2];
						WriteflashCode();
						ReadflashCode();
						
						tx1buf[0] = '$'; tx1buf[1]=0x0A; tx1buf[2]=0x01;  //加密成功
						for(i=3;i<18;i++)
							tx1buf[i]=0x00;
						
						code_flag2=on;      //设置网位仪加密状态
						code_flag1=off;
						com1sendback();
					}
					else                     //若设备已加密，请勿重复加密
					{
						tx1buf[0] = '$'; tx1buf[1]=0x0A; tx1buf[2]=0x02;
						for(i=3;i<18;i++)
							tx1buf[i]=0x00;
						
						//code_flag1=off;
						com1sendback();
					}
				}
			}
			break;
				
			case 0x0B://向网位仪解密
			{
				if(charging_flag == on)
				{
					if(code_flag2==on)
					{
						for(i=0;i<6;i++)
						{
							if(CodeNum[i] == com1_rxbuf[i+2])
							{
								code_cnt++;
							}
						}

						if(code_cnt==6)
						{
							tx1buf[0] = '$'; tx1buf[1]=0x0B; tx1buf[2]=0x01;   //解密成功，可以写入mmsi或船名
							for(i=3;i<18;i++)
								tx1buf[i]=0x00;

							code_flag1=on;  //设置密码正确标志
							
							for(i=0;i<6;i++)
								CodeNum[i] = 0xFF;
							WriteflashCode();
							ReadflashCode();
							
							code_flag2=off;      //设置网位仪未加密状态。
							code_flag1=off;  //设置密码错误标志		
							
							com1sendback();
						}
						else
						{
							tx1buf[0] = '$'; tx1buf[1]=0x0B; tx1buf[2]=0x02;    //设备已加密。输入密码错误
							for(i=3;i<18;i++)
								tx1buf[i]=0x00;
							
							code_flag1=off;  //设置密码错误标志
							com1sendback();
						}
						
						code_cnt = 0;
					}
					else
					{
						tx1buf[0] = '$'; tx1buf[1]=0x0B; tx1buf[2]=0x03;    //设备未加密，询问是否需要加密
							for(i=3;i<18;i++)
								tx1buf[i]=0x00;
						
						code_flag1=off;   //默认密码错误状态
						com1sendback();	
					}
				}
			}
			break;
			
			case 0x0C:   //恢复出厂设置，清除密码
			{
				if(charging_flag == on)
				{
					for(i=0;i<6;i++)
						CodeNum[i] = 0xFF;
					WriteflashCode();
					ReadflashCode();
					
					code_flag2=off;      //设置网位仪未加密状态。
					code_flag1=off;  //设置密码错误标志			
					
					MMSI=00000000; 
					WriteflashMMSI();
					ReadflashMMSI();
				
					for(i=10;i<16;i++)
						boatnum[i] = 0x00;
					WriteflashBoatnum();
					ReadflashBoatnum();
					
					ReadflashMPeriod();
					interval_num = intervalA;
					interval_s = intervalA;
	//				time_o = 0xffff;
					WriteflashMPeriod();

					flag_channel = 0;
					Write_TX_Channel();
					Read_TX_Channel();
					
					jingdu=0;weidu=0;
					Write_GPS_Info();
					Read_GPS_Info();					
					
					tx1buf[0] = '$'; tx1buf[1] = 0x0c;tx1buf[2] = 0x01; //应答
					for(i=3;i<18;i++)
						tx1buf[i]=0x00;
					com1sendback();
				}
			}
			break;
			
			case 0x12: //发射间隔设置 /间隔计数次数固定值
			{
				if(charging_flag == on)
				{
					intervalA = com1_rxbuf[6]; //发射间隔固定值
					intervalA = intervalA << 8;
					intervalA = intervalA + com1_rxbuf[7];
					
					gps_invalid = com1_rxbuf[8]; //gps失效时间2min
					gps_invalid = gps_invalid << 8;
					gps_invalid = gps_invalid + com1_rxbuf[9];

		// 				intervalB = com1_rxbuf[10]; //GPS长时间失效的启动值
		// 				intervalB = intervalB << 8;
		// 				intervalB = intervalB + com1_rxbuf[11];
					
					interval_num = intervalA;
					interval_s = intervalA;
					WriteflashMPeriod();
					ReadflashMPeriod();

					tx1buf[0] = '$'; tx1buf[1] = 0x12;tx1buf[2] = 0x01; //应答
					for(i=3;i<18;i++)
						tx1buf[i]=0x00;
					com1sendback();
				}
			}
			break;			
			
			case 0x16: //ID激活
			{
				if(charging_flag == on)
				{
					ChipUniqueID_Flash[0]=*(__IO u32 *)(0X1FFFF7F0);
					ChipUniqueID_Flash[1]=*(__IO u32 *)(0X1FFFF7EC);
					ChipUniqueID_Flash[2]=*(__IO u32 *)(0X1FFFF7E8);
					
					WriteflashMUID();
					ReadflashMUID();
					
					activation_flag=0x0101;  //激活标志	
					WriteflashMActivation();
				
					tx1buf[0] = '$'; tx1buf[1] = 0x16;
					for(i=2;i<18;i++)   tx1buf[i]=0x00;
					com1sendback();
				}
			}
			break;
			
			case 0x17: //连接网位仪
			{
				task_flag2=off;
				TIM3_OFF();
				Enter_PowersaveMode();
				
				tx1buf[0] = '$'; tx1buf[1] = 0x17;
				tx1buf[2] = 0x01;
				for(i=3;i<18;i++)   tx1buf[i]=0x00;
				com1sendback();
		
				charging_flag = on;
			}
			break;

			case 0x18: //断开网位仪
			{
				tx1buf[0] = '$'; tx1buf[1] = 0x18;
				for(i=2;i<18;i++)   tx1buf[i]=0x00;
				com1sendback();
			}
			break;

			case 0x2A: //流网或张网选择
			{
				if(charging_flag == on)
				{
					if(com1_rxbuf[2] == 0x01) 
						cogsel = 1;
					else
						cogsel = 0;
					
					Write_Flash_Cogsel();
					
					tx1buf[0] = '$'; tx1buf[1] = 0x2A; tx1buf[2] = 0x01;
					for(i=3;i<18;i++)   tx1buf[i]=0x00;
					com1sendback();
				}
			}
			break;
			
			case 0x2B: //船尺寸设置
			{
				if(charging_flag == on)
				{
					boatlong = (com1_rxbuf[2] << 8) + com1_rxbuf[3];
					boatwidth = com1_rxbuf[4];
					
					WriteflashBoatInfo();
					
// 					boatsize_a = boatlong/2;
// 					boatsize_b = boatlong/2;
// 					boatsize_c = boatwidth/2;
// 					boatsize_d = boatwidth/2;
					
					tx1buf[0] = '$'; tx1buf[1] = 0x2B; tx1buf[2] = 0x01;
					for(i=3;i<18;i++)   tx1buf[i]=0x00;
					com1sendback();
				}
			}
			break;
			
			default:
			break;		
		}	
	}
}





