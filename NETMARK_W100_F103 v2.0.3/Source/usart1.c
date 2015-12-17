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
		
// 			Length = UART_RX1_LEN - DMA_GetCurrDataCounter(DMA1_Channel5);		
// 			for(i=0;i<Length;i++)
// 			{
// 				printf("%c",Uart_Rx1[i]);
// 			}
		
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


void Usart1GetCommand(void)  //����1����
{
	u8 i;

	if(com1_rxbuf[0] == 0x24)
	{
		switch(com1_rxbuf[1])	
		{
			case 0x01://����λ��д����
			{
				if(charging_flag == on)
				{
					if(code_flag2==on)  //�������
					{
						if(code_flag1==on)  //������ȷ
						{
								for(i=0;i<16;i++)
									boatnum[i] = com1_rxbuf[i+2];
								WriteflashBoatnum();
								ReadflashBoatnum();
							
								tx1buf[0] = '$'; tx1buf[1] = 0x01;tx1buf[2] = 0x01; //Ӧ��
								for(i=3;i<18;i++)
									tx1buf[i]=0x00;
								com1sendback();
							
								//code_flag1=off; //������������־
						}
						else  //�������
						{
							tx1buf[0] = '$'; tx1buf[1] = 0x01; tx1buf[2] = 0x02; //Ӧ�� ���Ƚ���
							for(i=3;i<18;i++)
								tx1buf[i]=0x00;
							com1sendback();
							
							code_flag1=off; //������������־
						}
					}
					else //δ�������
					{
						for(i=0;i<16;i++)
							boatnum[i] = com1_rxbuf[i+2];
						WriteflashBoatnum();
						ReadflashBoatnum();
						
						tx1buf[0] = '$'; tx1buf[1] = 0x01;tx1buf[2] = 0x01; //Ӧ��
						for(i=3;i<18;i++)
							tx1buf[i]=0x00;
						com1sendback();
					}
				}
			}
			break;
			case 0x02://����λ��дMMSI��
			{
				if(charging_flag == on)
				{
					if(code_flag2==on)   //�������
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
					
							tx1buf[0] = '$'; tx1buf[1] = 0x02;tx1buf[2] = 0x01; //��ȷд��mmsi
							for(i=3;i<18;i++)
								tx1buf[i]=0x00;
							com1sendback();
							//code_flag1=off;  //������������־
						}
						else
						{
							
							tx1buf[0] = '$'; tx1buf[1] = 0x02;tx1buf[2] = 0x02; //����������
							for(i=3;i<18;i++)
								tx1buf[i]=0x00;
							
							com1sendback();
							code_flag1=off; //������������־
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
						tx1buf[0] = '$'; tx1buf[1] = 0x02;tx1buf[2] = 0x01; //Ӧ��
						for(i=3;i<18;i++)
							tx1buf[i]=0x00;
						
						com1sendback();
					}
				}
			}
			break;
			
			case 0x04://����λ�Ƕ�ȡ����
			{
				if(charging_flag == on)
				{
					tx1buf[0] = '$'; tx1buf[1] = 0x04;

					for(i=0;i<16;i++)   tx1buf[i+2]=boatnum[i];
					com1sendback();
				}

			}
			break;
				
			case 0x05://����λ�Ƕ�ȡMMSI��
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
			
			case 0x0A://����λ�Ǽ���
			{
				if(charging_flag == on)
				{
					Is_Coded();
					if(code_flag2==off)          //���豸δ���ܣ�����
					{
						for(i=0;i<6;i++)
						CodeNum[i] = com1_rxbuf[i+2];
						WriteflashCode();
						ReadflashCode();
						
						tx1buf[0] = '$'; tx1buf[1]=0x0A; tx1buf[2]=0x01;  //���ܳɹ�
						for(i=3;i<18;i++)
							tx1buf[i]=0x00;
						
						code_flag2=on;      //������λ�Ǽ���״̬
						code_flag1=off;
						com1sendback();
					}
					else                     //���豸�Ѽ��ܣ������ظ�����
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
				
			case 0x0B://����λ�ǽ���
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
							tx1buf[0] = '$'; tx1buf[1]=0x0B; tx1buf[2]=0x01;   //���ܳɹ�������д��mmsi����
							for(i=3;i<18;i++)
								tx1buf[i]=0x00;

							code_flag1=on;  //����������ȷ��־
							
							for(i=0;i<6;i++)
								CodeNum[i] = 0xFF;
							WriteflashCode();
							ReadflashCode();
							
							code_flag2=off;      //������λ��δ����״̬��
							code_flag1=off;  //������������־		
							
							com1sendback();
						}
						else
						{
							tx1buf[0] = '$'; tx1buf[1]=0x0B; tx1buf[2]=0x02;    //�豸�Ѽ��ܡ������������
							for(i=3;i<18;i++)
								tx1buf[i]=0x00;
							
							code_flag1=off;  //������������־
							com1sendback();
						}
						
						code_cnt = 0;
					}
					else
					{
						tx1buf[0] = '$'; tx1buf[1]=0x0B; tx1buf[2]=0x03;    //�豸δ���ܣ�ѯ���Ƿ���Ҫ����
							for(i=3;i<18;i++)
								tx1buf[i]=0x00;
						
						code_flag1=off;   //Ĭ���������״̬
						com1sendback();	
					}
				}
			}
			break;
			
			case 0x0C:   //�ָ��������ã��������
			{
				if(charging_flag == on)
				{
					for(i=0;i<6;i++)
						CodeNum[i] = 0xFF;
					WriteflashCode();
					ReadflashCode();
					
					code_flag2=off;      //������λ��δ����״̬��
					code_flag1=off;  //������������־			
					
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
					
					tx1buf[0] = '$'; tx1buf[1] = 0x0c;tx1buf[2] = 0x01; //Ӧ��
					for(i=3;i<18;i++)
						tx1buf[i]=0x00;
					com1sendback();
				}
			}
			break;
			
			case 0x12: //���������� /������������̶�ֵ
			{
				if(charging_flag == on)
				{
					intervalA = com1_rxbuf[6]; //�������̶�ֵ
					intervalA = intervalA << 8;
					intervalA = intervalA + com1_rxbuf[7];
					
					gps_invalid = com1_rxbuf[8]; //gpsʧЧʱ��2min
					gps_invalid = gps_invalid << 8;
					gps_invalid = gps_invalid + com1_rxbuf[9];

		// 				intervalB = com1_rxbuf[10]; //GPS��ʱ��ʧЧ������ֵ
		// 				intervalB = intervalB << 8;
		// 				intervalB = intervalB + com1_rxbuf[11];
					
					interval_num = intervalA;
					interval_s = intervalA;
					WriteflashMPeriod();
					ReadflashMPeriod();

					tx1buf[0] = '$'; tx1buf[1] = 0x12;tx1buf[2] = 0x01; //Ӧ��
					for(i=3;i<18;i++)
						tx1buf[i]=0x00;
					com1sendback();
				}
			}
			break;			
			
			case 0x16: //ID����
			{
				if(charging_flag == on)
				{
					ChipUniqueID_Flash[0]=*(__IO u32 *)(0X1FFFF7F0);
					ChipUniqueID_Flash[1]=*(__IO u32 *)(0X1FFFF7EC);
					ChipUniqueID_Flash[2]=*(__IO u32 *)(0X1FFFF7E8);
					
					WriteflashMUID();
					ReadflashMUID();
					
					activation_flag=0x0101;  //�����־	
					WriteflashMActivation();
				
					tx1buf[0] = '$'; tx1buf[1] = 0x16;
					for(i=2;i<18;i++)   tx1buf[i]=0x00;
					com1sendback();
				}
			}
			break;
			
			case 0x17: //������λ��
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

			case 0x18: //�Ͽ���λ��
			{
				tx1buf[0] = '$'; tx1buf[1] = 0x18;
				for(i=2;i<18;i++)   tx1buf[i]=0x00;
				com1sendback();
			}
			break;

			case 0x2A: //����������ѡ��
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
			
			case 0x2B: //���ߴ�����
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





