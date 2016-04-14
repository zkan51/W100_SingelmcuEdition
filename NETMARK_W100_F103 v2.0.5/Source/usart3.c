#include "main.h"
#include "stm32f10x_dma.h"

//static u8 com3_start;
//static u8 com3_stop;
//static u8 com3_num;
#define UART_RX3_LEN     18
static u8 com3_rxbuf[31];  //串口3接收缓存
u8 com3_txbuf[31]; 	   //串口3发送缓冲
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
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  //启动DMA时钟  

	DMA_DeInit(DMA1_Channel3); 	//USART3 DMA1通道3配置   
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART3->DR);//外设地址  
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)com3_rxbuf;     //内存地址  
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;            //dma传输方向从外设到内存 
	DMA_InitStructure.DMA_BufferSize = UART_RX3_LEN;               //设置DMA在传输时缓冲区的长度  
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//设置DMA的外设地址不变 
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;      //设置DMA的内存递增模式  
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据字长 8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;    //内存数据字长8位   
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                //设置DMA的传输模式   
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;      //设置DMA的优先级别 最高
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                 //设置DMA的2个memory中的变量互相访问  
	DMA_Init(DMA1_Channel3,&DMA_InitStructure);  

	DMA_Cmd(DMA1_Channel3,ENABLE);     //USART3 使能通道3 
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
	
	USART_ITConfig(USART3,USART_IT_TC,DISABLE);    //禁止 
	USART_ITConfig(USART3,USART_IT_RXNE,DISABLE);  //禁止 
	USART_ITConfig(USART3,USART_IT_IDLE,ENABLE);   //开启

	USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);	//采用DMA方式接收  
	
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
			Length = USART3->DR; //清USART_IT_IDLE标志  
			Length = UART_RX3_LEN - DMA_GetCurrDataCounter(DMA1_Channel3);	

//test(0xaa);
			
			Usart3GetCommand();
			
	    DMA1_Channel3->CNDTR = UART_RX3_LEN;//重装填,并让接收地址偏址从0开始
      DMA_Cmd(DMA1_Channel3, ENABLE);//处理完,重开DMA   
    }   
    __nop();   
}

void Usart3GetCommand(void)  //串口3接收
{
	u8 i;

	if(com3_rxbuf[0] == 0x24)
	{
		switch(com3_rxbuf[1])	
		{
			case 0x01://向网位仪写船名
								{
										tx1buf[0] = '$'; 
									 tx1buf[1] = 0x01;
									 tx1buf[2] = 0x01; //应答 写入船名成功
										for(i=3;i<18;i++)
											 tx1buf[i]=0x00;
									
										com1sendback();
								}
								break;
			
//			case 0x03://注入钥匙
//								{
//									 for(i=0;i<18;i++)
//										  tx1buf[i] = com3_rxbuf[i];
//									
//									 com1sendback();
//								}
//								break;
//			
//			case 0x06://读取钥匙
//								{
//									 for(i=0;i<18;i++)
//										  tx1buf[i] = com3_rxbuf[i];
//									
//									 com1sendback();
//								}
//								break;
//			
//			case 0x0F:                //钥匙开
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

//			case 0x10:             //钥匙关
//								{
//									ReadBackup();
//									interval_num = intervalA;
//									interval_s = intervalA;
//					//				time_o = 0xffff;
//									WriteBackup();
//									Write_Flash_GPS_Info();  //存入flash
//									
//									Enter_PowersaveMode();
//									
//									LedShutdown();
//									
//									PWR_WakeUpPinCmd(ENABLE);
//									PWR_EnterSTANDBYMode();
//								}
//								break;
			
			case 0x0C:   //恢复出厂设置，清除密码
								{
										tx1buf[0] = '$';
								 	tx1buf[1] = 0x0c;
									 tx1buf[2] = 0x01; //应答 写入船名成功
									
										for(i=3;i<18;i++)
											  tx1buf[i]=0x00;
									
										com1sendback();
								}
								break;
			
			case 0x12: //发射间隔设置 /间隔计数次数固定值
								{
											tx1buf[0] = '$'; 
									  tx1buf[1] = 0x12;
									  tx1buf[2] = 0x01; //应答 写入船名成功
									
											for(i=3;i<18;i++)
												  tx1buf[i]=0x00;
									
											com1sendback();
								}
								break;
			
			case 0x17: //连接网位仪
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

			case 0x18: //断开网位仪
								{
									 tx1buf[0] = '$'; 
									 tx1buf[1] = 0x18;
									 for(i=2;i<18;i++)  
    									tx1buf[i]=0x00;
									
									 com1sendback();
								}
								break;
			
			case 0x1A: //进入/退出充电
								{
									if(com3_rxbuf[2] == 0)  //退出充电
									{
											charging_flag = off;
											
											LED_OFF();

											RTC_Init();
											PWR_WakeUpPinCmd(ENABLE);
											PWR_EnterSTANDBYMode();
									}
				     else  //进入充电
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





