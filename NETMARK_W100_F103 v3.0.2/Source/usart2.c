#include "main.h"
#include <stdarg.h>
#include "stm32f10x_dma.h"

static u8 ch; //COM
static u8 num=0;
static u8 rev_start = 0;     //接收开始标志
static u8 gps_flag = 0;      //GPS处理标志
char rev_buf[80];       //接收缓存

#define UART_RX2_LEN     80
 /***********************************************************
 * 函数名: USART2_NVIC_Configuration
 * 描述  ：配置串口2中断
 * 输入  : 无
 * 输出  : 无
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
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  //启动DMA时钟 
	
	DMA_DeInit(DMA1_Channel6); 	//DMA1通道6配置   
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART2->DR);//外设地址  
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)rev_buf;     //内存地址  
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;            //dma传输方向从内存到外设 
	DMA_InitStructure.DMA_BufferSize = UART_RX2_LEN;               //设置DMA在传输时缓冲区的长度  
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//设置DMA的外设地址不变 
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;      //设置DMA的内存递增模式  
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据字长 8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;    //内存数据字长8位   
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                //设置DMA的传输模式   
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;      //设置DMA的优先级别 高
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                 //设置DMA的2个memory中的变量互相访问  
	DMA_Init(DMA1_Channel6,&DMA_InitStructure);  	
	
	DMA_Cmd(DMA1_Channel6,ENABLE);
}
/****************************************************
 * 函数名：USART2_Config
 * 描述  ：USART2 GPIO 配置,工作模式配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
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
	
// 	USART_ITConfig(USART2,USART_IT_TC,DISABLE);    //禁止 
// 	USART_ITConfig(USART2,USART_IT_RXNE,DISABLE);  //禁止 
// 	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);   //开启
 	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE); 
// 	
// 	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);	//采用DMA方式接收  
	
  USART_Cmd(USART2, ENABLE);
}

void USART2_IRQHandler(void)
{   
/*    uint32_t Length = 0;//,i=0;
    if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
    {  
			DMA_Cmd(DMA1_Channel6,DISABLE);
			Length = USART2->SR;
			Length = USART2->DR; //清USART_IT_IDLE标志
			
			GPS_RMC_Parse(rev_buf,&GPS);
			
	    DMA1_Channel6->CNDTR = UART_RX2_LEN;//重装填,并让接收地址偏址从0开始
      DMA_Cmd(DMA1_Channel6, ENABLE);//处理完,重开DMA
    }
    __nop();*/

/////////////////		
	if(USART2->SR & USART_FLAG_RXNE)//接收中断
	{
		USART2->SR &= ~USART_FLAG_RXNE;	//清零	
		ch = USART2->DR & 0x1ff;

// 					USART_SendData(USART1, ch);
//   			while (!(USART1->SR & USART_FLAG_TXE)) ;
		
		if(('$'==ch)&&(0==gps_flag))
		{
			rev_start = 1;
		}
		if (1==rev_start)  //标志位为1，开始接收
		{
			
 			rev_buf[num++] = ch;  //字符存到数组中

			if ('\n'==ch)     //如果接收到换行
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
