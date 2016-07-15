#include "main.h"
#include <stdarg.h>
#include "stm32f10x_dma.h"
#include "usart3.h"

//static u8 com3_start;
//static u8 com3_stop;
//static u8 com3_num;

#define UART_RX1_LEN     100 
static u8 com1_rxbuf[UART_RX1_LEN];  //����1���ջ���
u8 tx1buf[31]; 	   //����1���ͻ���
extern u8 swchflag;

 /***********************************************************
 * ������: USART1_NVIC_Configuration
 * ����  �����ô���1�ж�
 * ����  : ��
 * ���  : ��
 ***********************************************************/
static void USART1_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    												
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

static void UART1_DMA_Configuration(void)
{
	  DMA_InitTypeDef DMA_InitStructure;
	  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  //����DMAʱ��  

			DMA_DeInit(DMA1_Channel5); 	//USART1 DMA1ͨ��5����   
			DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);//�����ַ  
			DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)com1_rxbuf;     //�ڴ��ַ  
			DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;            //dma���䷽������赽�ڴ� 
			DMA_InitStructure.DMA_BufferSize = UART_RX1_LEN;               //����DMA�ڴ���ʱ�������ĳ���  
			DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//����DMA�������ַ���� 
			DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;      //����DMA���ڴ����ģʽ  
			DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//���������ֳ� 8λ
			DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;    //�ڴ������ֳ�8λ   
			DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                //����DMA�Ĵ���ģʽ   
			DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;      //����DMA�����ȼ��� ���
			DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                 //����DMA��2��memory�еı����������  
			DMA_Init(DMA1_Channel5,&DMA_InitStructure);  
	
			DMA_Cmd(DMA1_Channel5,ENABLE);
}

 /***********************************************************
 * ������: USART1_Config
 * ����  �����ô���1��9600������
 * ����  : ��
 * ���  : ��
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

	USART_ITConfig(USART1,USART_IT_TC,DISABLE);    //��ֹ 
	USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);  //��ֹ 
	USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);   //����

	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);	//����DMA��ʽ����  
	
  USART_Cmd(USART1, ENABLE);
}

/************************************************************
 * ��������fputc
 * ����  ���ض���c�⺯��printf��USART1
 * ����  ����
 * ���  ����
 * ����  ����printf����
 ************************************************************/
int fputc(int ch, FILE *f)
{
/* ��Printf���ݷ������� */
  USART_SendData(USART1, (unsigned char) ch);
  while (!(USART1->SR & USART_FLAG_TXE));
 
  return (ch);
}
/*************************************************************
 * ��������itoa
 * ����  ������������ת�����ַ���
 * ����  ��-radix =10 ��ʾ10���ƣ��������Ϊ0
 *         -value Ҫת����������
 *         -buf ת������ַ���
 *         -radix = 10
 * ���  ����
 * ����  ����
 * ����  ����USART1_printf()����
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
 * ��������USART1_printf
 * ����  ����ʽ�������������C���е�printf��������û���õ�C��
 * ����  ��-USARTx ����ͨ��������ֻ�õ��˴���1����USART1
 *		     -Data   Ҫ���͵����ڵ����ݵ�ָ��
 *			   -...    ��������
 * ���  ����
 * ����  ���� 
 * ����  ���ⲿ����
 *         ����Ӧ��USART1_printf( USART1, "\r\n this is a demo \r\n" );
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

	while ( *Data != 0)     // �ж��Ƿ񵽴��ַ���������
	{				                          
		if ( *Data == 0x5c )  //'\'
		{									  
			switch ( *++Data )
			{
				case 'r':							          //�س���
					USART_SendData(USARTx, 0x0d);
					Data ++;
					break;

				case 'n':							          //���з�
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
				case 's':										  //�ַ���
					s = va_arg(ap, const char *);
          for ( ; *s; s++) 
					{
						USART_SendData(USARTx,*s);
						while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
          }
					Data++;
          break;

        case 'd':										//ʮ����
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
		Length = USART1->DR; //��USART_IT_IDLE��־  
		
		Usart1GetCommand();
		
		DMA1_Channel5->CNDTR = UART_RX1_LEN;//��װ��,���ý��յ�ַƫַ��0��ʼ
		DMA_Cmd(DMA1_Channel5, ENABLE);//������,�ؿ�DMA   
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

/***********************************************************
 * ������  tx1bufInfo
 * ����  ����䷵�ظ���λ�������� tx1buf
 * ����  : ��
 * ���  : ��
 ***********************************************************/
void tx1bufInfo(char txbuf1,char txbuf2)
{
	 char i;
	 tx1buf[0]='$';
	 tx1buf[1]=txbuf1;
	 tx1buf[2]=txbuf2;
	 for(i=3;i<18;i++)
				tx1buf[i]=0x00;
}

void Usart1GetCommand(void)  //����1����
{
	u8 i;

	if(com1_rxbuf[0] == 0x24)
	{
		switch(com1_rxbuf[1])	
		{
			case 0x01://����λ��д����
								{
										if(isCharging == on)
										{
												if(encryption==on)  //�������
												{
													//���Ƚ���
													tx1bufInfo(0x01,0x02);
													com1sendback();
												}
												else //δ�������
												{
														for(i=0;i<16;i++)
															 boatnum[i] = com1_rxbuf[i+2];
													
														WriteflashBoatnum();
														ReadflashBoatnum();
														
														tx1bufInfo(0x01,0x01);
														com1sendback();
												}
										}
								}
								break;
								
			case 0x02://����λ��дMMSI��
								{
									if(isCharging == on)
									{
										if(encryption==on)   //�������
										{
														//���Ƚ���
														tx1bufInfo(0x02,0x02);
														com1sendback();
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
												
													tx1bufInfo(0x02,0x01);
													com1sendback();
											}
									}
								}
								break;
			
			case 0x04://����λ�Ƕ�ȡ����
								{
										if(isCharging == on)
										{
												tx1buf[0] = '$'; 
											 tx1buf[1] = 0x04;

												for(i=0;i<16;i++)   
											   tx1buf[i+2]=boatnum[i];
											
												com1sendback();
										}
								}
								break;
				
			case 0x05://����λ�Ƕ�ȡMMSI��
								{
										if(isCharging == on)
										{
												tx1buf[0] = '$';
											 tx1buf[1] = 0x05;
											
												for(i=0;i<4;i++)
													 tx1buf[i+2] = MMSI >> (24 - i*8);
												for(i=6;i<18;i++)
													 tx1buf[i]=0x00;

												com1sendback();		
										}
								}
								break;
			
			case 0x0A://����λ�Ǽ���
								{
									if(isCharging == on)
									{
										if(encryption==off)          //���豸δ���ܣ�����
										{
												for(i=0;i<6;i++)
												CodeNum[i] = com1_rxbuf[i+2];
											
												WriteflashCode();
												ReadflashCode();
											
												tx1bufInfo(0x0A,0x01);
												encryption=on;      //������λ�Ǽ���״̬
												com1sendback();
										}
										else                     //���豸�Ѽ��ܣ������ظ�����
										{
												tx1bufInfo(0x0A,0x02);
												com1sendback();
										}
									}
								}
								break;
				
			case 0x0B://����λ�ǽ���
								{
									if(isCharging == on)
									{
										if(encryption==on)  //�ж��Ƿ���ܹ�
										{
												for(i=0;i<6;i++)  //�ж�������ܳ��Ƿ���ȷ
												{
														if(CodeNum[i] == com1_rxbuf[i+2])
															code_cnt++;
												}

												if(code_cnt==6)  //������ȷ
												{
														for(i=0;i<6;i++)
																CodeNum[i] = 0xFF;
														WriteflashCode();
														ReadflashCode();
														
														tx1bufInfo(0x0B,0x01);
														encryption=off;
														com1sendback();
												}
												else //�������
												{
														tx1bufInfo(0x0B,0x02);
														com1sendback();
												}
												code_cnt = 0;
										}
										else//�豸δ���ܣ�ѯ���Ƿ���Ҫ����
										{
												tx1bufInfo(0x0B,0x03);
												com1sendback();	
										}
									}
								}
								break;
			
			case 0x0C:   //�ָ��������ã��������
								{
										if(isCharging == on)
										{
												for(i=0;i<6;i++)
													 CodeNum[i] = 0xFF;     //��ʼ������
												WriteflashCode();
												ReadflashCode();
												
												encryption=off;          //������λ��δ����״̬��
												
												MMSI=00000000;           //��ʼ��MMSI
												WriteflashMMSI();
												ReadflashMMSI();
											
												for(i=0;i<16;i++)
													 boatnum[i] = 0;     //��ʼ������
												WriteflashBoatnum();
												ReadflashBoatnum();
												
											 intervalA=2;             //��ʼ��������1����
												WriteflashMPeriod();

												flag_channel = 0;        //��ʼ������ͨ��
												Write_TX_Channel();
												Read_TX_Channel();
												
												jingdu=0;weidu=0;        //��ʼ����γ��
												Write_GPS_Info();
												Read_GPS_Info();					
												
												Time = 0xFFFFFFFF; // ��������Ϣ
												for(i=0; i<15; i++)
												AgencyName[i] = 0xFF;
												WriteFalsh_AgencyInfo();		
												
												tx1bufInfo(0x0C,0x01);
												com1sendback();
											}
								}
								break;
			
			case 0x12: //���������� /������������̶�ֵ
								{
									if(isCharging == on)
									{
											intervalA = com1_rxbuf[6]; //�������̶�ֵ
											intervalA = intervalA << 8;
											intervalA = intervalA + com1_rxbuf[7];
										
											WriteflashMPeriod();
											ReadflashMPeriod();
						
											tx1bufInfo(0x12,0x01);
											com1sendback();
									}
								}
								break;			
			
			case 0x16: //ID����
								{
										if(isCharging == on)
										{
												ChipUniqueID_Flash[0]=*(__IO u32 *)(0X1FFFF7F0);
												ChipUniqueID_Flash[1]=*(__IO u32 *)(0X1FFFF7EC);
												ChipUniqueID_Flash[2]=*(__IO u32 *)(0X1FFFF7E8);
												
												WriteflashMUID();
												ReadflashMUID();
												
												activation_flag=0x0101;  //�����־	
												WriteflashMActivation();
											
												tx1buf[0] = '$'; 
											 tx1buf[1] = 0x16;
												for(i=2;i<18;i++)   
											    tx1buf[i]=0x00;
												com1sendback();
										}
								}
								break;
			
			case 0x17: //������λ��
								{
									if(isCharging == on)
									{
										Is_Coded();
										tx1buf[0] = '$';
										tx1buf[1] = 0x17;
										tx1buf[2] = 0x01;
									
										for(i=3;i<18;i++)   
												tx1buf[i]=0x00;
										
										//ʱ��δע��,�жϾ������ǲ��ǵ�һ��д�����
									 if((u8)Time == 0xff) 
											tx1buf[3] = 1;
										else 
											tx1buf[3] = 0;
										
										for(i=4;i<18;i++)   
												tx1buf[i]=0x00;									
										com1sendback();
									}
								}
								break;

			case 0x18: //�Ͽ���λ��
								{
									if(isCharging == on)
									{
											tx1buf[0] = '$';
											tx1buf[1] = 0x18;
											for(i=2;i<18;i++)   
														tx1buf[i]=0x00;
										
											com1sendback();
									}
								}
								break;

			case 0x2A: //����������ѡ��
								{
									if(isCharging == on)
									{
											if(com1_rxbuf[2] == 0x01) 
													cogsel = 1;
											else
													cogsel = 0;
											
											Write_Flash_Cogsel();
											tx1bufInfo(0x2A,0x01);
											com1sendback();
									}
								}
								break;
			
			case 0x2B: //���ߴ�����
								{
										if(isCharging == on)
										{
												boatlong = (com1_rxbuf[2] << 8) + com1_rxbuf[3];
												boatwidth = com1_rxbuf[4];
												
												WriteflashBoatInfo();
												tx1bufInfo(0x2B,0x01);
												com1sendback();
										}
								}
								break;
								
		case 0x30: //��������Ϣע��
						 {
								if(isCharging == on)
								{
									if(com1_rxbuf[2]==0)
									{
										Time = 0;
										Time += (com1_rxbuf[3]<<24);
										Time += (com1_rxbuf[4]<<16);
										Time += (com1_rxbuf[5]<<8);
										Time += com1_rxbuf[6];
										
										tx1bufInfo(0x30,0x01);
										com1sendback();
									}
									else
									{
										for(i=3;i<18;i++)
											AgencyName[i-3] = com1_rxbuf[i];
										
											tx1bufInfo(0x30,0x01);
											com1sendback();
										
											WriteFalsh_AgencyInfo();
									}
								}
							}
							break;
		
		case 0x31://��������Ϣ��ȡ
							{
								if(isCharging == on)
								{
									ReadFalsh_AgencyInfo();
									
									if(com1_rxbuf[2] == 1)//��ȡʱ��
									{
										tx1buf[0] = '$';
										tx1buf[1] = 0x31;
										tx1buf[2] = 0x01;
										tx1buf[3] = Time>>24;
										tx1buf[4] = Time>>16;
										tx1buf[5] = Time>>8;
										tx1buf[6] = Time;
										
										for(i=7;i<18;i++)
											tx1buf[i] = 0;
										com1sendback();
									}
									else //��ȡ����
									{
										tx1buf[0] = '$';
										tx1buf[1] = 0x31;
										tx1buf[2] = 0x02;
										for(i=0;i<16;i++)
											tx1buf[i+3] = AgencyName[i];
										com1sendback();
									}
								}
							}
							break;			
			default:
								break;		
		}	
	}
}





