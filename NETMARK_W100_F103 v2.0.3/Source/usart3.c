#include "main.h"
#include "stm32f10x_dma.h"

//static u8 com3_start;
//static u8 com3_stop;
//static u8 com3_num;
#define UART_RX3_LEN     18
static u8 com3_rxbuf[31];  //����3���ջ���
u8 com3_txbuf[31]; 	   //����3���ͻ���
extern u8 swchflag;


void USART3_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  													
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;	  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

static void UART3_DMA_Configuration(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  //����DMAʱ��  

	DMA_DeInit(DMA1_Channel3); 	//USART3 DMA1ͨ��3����   
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART3->DR);//�����ַ  
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)com3_rxbuf;     //�ڴ��ַ  
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;            //dma���䷽������赽�ڴ� 
	DMA_InitStructure.DMA_BufferSize = UART_RX3_LEN;               //����DMA�ڴ���ʱ�������ĳ���  
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//����DMA�������ַ���� 
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;      //����DMA���ڴ����ģʽ  
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//���������ֳ� 8λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;    //�ڴ������ֳ�8λ   
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                //����DMA�Ĵ���ģʽ   
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;      //����DMA�����ȼ��� ���
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                 //����DMA��2��memory�еı����������  
	DMA_Init(DMA1_Channel3,&DMA_InitStructure);  

	DMA_Cmd(DMA1_Channel3,ENABLE);     //USART3 ʹ��ͨ��3 
}

void USART3_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	USART3_NVIC_Configuration();
	UART3_DMA_Configuration();
	
	/* config USART3 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	/* USART3 GPIO config */
   /* Configure USART2 Tx (PB.10) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	    
  /* Configure USART2 Rx (PB.11) as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	  
	/* USART3 mode config */
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);
	
	USART_ITConfig(USART3,USART_IT_TC,DISABLE);    //��ֹ 
	USART_ITConfig(USART3,USART_IT_RXNE,DISABLE);  //��ֹ 
	USART_ITConfig(USART3,USART_IT_IDLE,ENABLE);   //����

	USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);	//����DMA��ʽ����  
	
	USART_Cmd(USART3, ENABLE);
}

void sendtocom3(void)   
{
	u8 i;
	
	for(i=0;i<18;i++)	
	{
		USART_SendData(USART3, com3_txbuf[i]);
		while (!(USART3->SR & USART_FLAG_TXE)) ;
	}
}

void test(u8 data)   
{
	USART_SendData(USART1, data);
	while (!(USART1->SR & USART_FLAG_TXE)) ;
}

void USART3_IRQHandler(void)                                 
{     
    uint32_t Length = 0;
    if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)  
    {  
  		DMA_Cmd(DMA1_Channel3,DISABLE); 
			Length = USART3->SR;  
			Length = USART3->DR; //��USART_IT_IDLE��־  
			Length = UART_RX3_LEN - DMA_GetCurrDataCounter(DMA1_Channel3);	

//test(0xaa);
			
			Usart3GetCommand();
			
	    DMA1_Channel3->CNDTR = UART_RX3_LEN;//��װ��,���ý��յ�ַƫַ��0��ʼ
      DMA_Cmd(DMA1_Channel3, ENABLE);//������,�ؿ�DMA   
    }   
    __nop();   
}

void Usart3GetCommand(void)  //����3����
{
	u8 i;

	if(com3_rxbuf[0] == 0x24)
	{
		switch(com3_rxbuf[1])	
		{
			case 0x01://����λ��д����
								{
										tx1buf[0] = '$'; 
									 tx1buf[1] = 0x01;
									 tx1buf[2] = 0x01; //Ӧ�� д�봬���ɹ�
										for(i=3;i<18;i++)
											 tx1buf[i]=0x00;
									
										com1sendback();
								}
								break;
			
//			case 0x03://ע��Կ��
//								{
//									 for(i=0;i<18;i++)
//										  tx1buf[i] = com3_rxbuf[i];
//									
//									 com1sendback();
//								}
//								break;
//			
//			case 0x06://��ȡԿ��
//								{
//									 for(i=0;i<18;i++)
//										  tx1buf[i] = com3_rxbuf[i];
//									
//									 com1sendback();
//								}
//								break;
//			
//			case 0x0F:                //Կ�׿�
//								{
//										swchflag = 0;
//										
//										ReadBackup();
//										interval_num = intervalA;
//										interval_s = intervalA;
//										WriteBackup();
//										Read_Flash_GPS_Info();
//										
//										task_flag2=on;
//										gps_led_on = 1;
//								}
//								break;

//			case 0x10:             //Կ�׹�
//								{
//									ReadBackup();
//									interval_num = intervalA;
//									interval_s = intervalA;
//					//				time_o = 0xffff;
//									WriteBackup();
//									Write_Flash_GPS_Info();  //����flash
//									
//									Enter_PowersaveMode();
//									
//									LedShutdown();
//									
//									PWR_WakeUpPinCmd(ENABLE);
//									PWR_EnterSTANDBYMode();
//								}
//								break;
			
			case 0x0C:   //�ָ��������ã��������
								{
										tx1buf[0] = '$';
								 	tx1buf[1] = 0x0c;
									 tx1buf[2] = 0x01; //Ӧ�� д�봬���ɹ�
									
										for(i=3;i<18;i++)
											  tx1buf[i]=0x00;
									
										com1sendback();
								}
								break;
			
			case 0x12: //���������� /������������̶�ֵ
								{
											tx1buf[0] = '$'; 
									  tx1buf[1] = 0x12;
									  tx1buf[2] = 0x01; //Ӧ�� д�봬���ɹ�
									
											for(i=3;i<18;i++)
												  tx1buf[i]=0x00;
									
											com1sendback();
								}
								break;
			
			case 0x17: //������λ��
								{
										task_flag2=off;
										
										TIM3_OFF();
										Enter_PowersaveMode();
										tx1buf[0] = '$'; 
									 tx1buf[1] = 0x17; 
									 tx1buf[2] = 0x01;
									
										for(i=3;i<18;i++) 
									    tx1buf[i]=0x00;
									
										com1sendback();
								
										charging_flag = on;
								}
								break;

			case 0x18: //�Ͽ���λ��
								{
									 tx1buf[0] = '$'; 
									 tx1buf[1] = 0x18;
									 for(i=2;i<18;i++)  
    									tx1buf[i]=0x00;
									
									 com1sendback();
								}
								break;
			
			case 0x1A: //����/�˳����
								{
									if(com3_rxbuf[2] == 0)  //�˳����
									{
											charging_flag = off;
											
											LED_OFF();

											RTC_Init();
											PWR_WakeUpPinCmd(ENABLE);
											PWR_EnterSTANDBYMode();
									}
				     else  //������
									{
											charging_flag = on;
											TIM3_OFF();
											Enter_PowersaveMode();
											TIM4_OFF();
											LED_ON();
									}
								}
								break;

			default:
								break;		
		}
	}
}





