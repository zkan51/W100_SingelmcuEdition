#include "main.h"
#include <stdarg.h>
#include "stm32f10x_dma.h"

static u8 ch; //COM
static u8 num=0;
static u8 rev_start = 0;     //���տ�ʼ��־
static u8 gps_flag = 0;      //GPS�����־
char rev_buf[80];       //���ջ���

#define UART_RX2_LEN     80
 /***********************************************************
 * ������: USART2_NVIC_Configuration
 * ����  �����ô���2�ж�
 * ����  : ��
 * ���  : ��
 ***********************************************************/
static void USART2_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  													
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;	  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

static void UART2_DMA_Configuration(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  //����DMAʱ�� 
	
	DMA_DeInit(DMA1_Channel6); 	//DMA1ͨ��6����   
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART2->DR);//�����ַ  
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)rev_buf;     //�ڴ��ַ  
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;            //dma���䷽����ڴ浽���� 
	DMA_InitStructure.DMA_BufferSize = UART_RX2_LEN;               //����DMA�ڴ���ʱ�������ĳ���  
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//����DMA�������ַ���� 
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;      //����DMA���ڴ����ģʽ  
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//���������ֳ� 8λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;    //�ڴ������ֳ�8λ   
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                //����DMA�Ĵ���ģʽ   
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;      //����DMA�����ȼ��� ��
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                 //����DMA��2��memory�еı����������  
	DMA_Init(DMA1_Channel6,&DMA_InitStructure);  	
	
	DMA_Cmd(DMA1_Channel6,ENABLE);
}
/****************************************************
 * ��������USART2_Config
 * ����  ��USART2 GPIO ����,����ģʽ����
 * ����  ����
 * ���  : ��
 * ����  ���ⲿ����
 ****************************************************/
void USART2_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

//	UART2_DMA_Configuration();
	USART2_NVIC_Configuration();
	
	/* config USART2 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	/* USART2 GPIO config */
   /* Configure USART2 Tx (PA.02) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	    
  /* Configure USART2 Rx (PA.03) as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	  
	/* USART2 mode config */
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);
	
// 	USART_ITConfig(USART2,USART_IT_TC,DISABLE);    //��ֹ 
// 	USART_ITConfig(USART2,USART_IT_RXNE,DISABLE);  //��ֹ 
// 	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);   //����
 	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE); 
// 	
// 	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);	//����DMA��ʽ����  
	
  USART_Cmd(USART2, ENABLE);
}

void USART2_IRQHandler(void)
{   
/*    uint32_t Length = 0;//,i=0;
    if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
    {  
			DMA_Cmd(DMA1_Channel6,DISABLE);
			Length = USART2->SR;
			Length = USART2->DR; //��USART_IT_IDLE��־
			
			GPS_RMC_Parse(rev_buf,&GPS);
			
	    DMA1_Channel6->CNDTR = UART_RX2_LEN;//��װ��,���ý��յ�ַƫַ��0��ʼ
      DMA_Cmd(DMA1_Channel6, ENABLE);//������,�ؿ�DMA
    }
    __nop();*/

/////////////////		
	if(USART2->SR & USART_FLAG_RXNE)//�����ж�
	{
		USART2->SR &= ~USART_FLAG_RXNE;	//����	
		ch = USART2->DR & 0x1ff;

// 					USART_SendData(USART1, ch);
//   			while (!(USART1->SR & USART_FLAG_TXE)) ;
		
		if(('$'==ch)&&(0==gps_flag))
		{
			rev_start = 1;
		}
		if (1==rev_start)  //��־λΪ1����ʼ����
		{
			
 			rev_buf[num++] = ch;  //�ַ��浽������

			if ('\n'==ch)     //������յ�����
			{
				
				rev_buf[num] = '\0';
				rev_start = 0;
				gps_flag  = 1;
				num = 0;			
				GPS_RMC_Parse(rev_buf,&GPS);
				gps_flag  = 0;
			}
		}		
	}
}
