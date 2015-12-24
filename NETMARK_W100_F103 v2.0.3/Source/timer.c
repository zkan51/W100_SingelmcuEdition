#include "main.h"

static u8 A;
static u16 B;
static u16 num_bit =0;
u8 TIM3_CAM1_flag=0; //防止反复进入
u8 TIM3_CAM3_flag=0; //防止反复进入
u8 TIM3_CAM5_flag=0; //防止反复进入

u16 tim3_cnt;
int tim2_cnt; //定时器2计数
u8 swchflag;
static u8 time4flag=0;

/***********************************************************
 * 函数名: TIM3_NVIC_Configuration
 * 描述  ：配置定时器T3中断
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void TIM3_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
/***********************************************************
 * 函数名: TIM3_Configuration
 * 描述  ：配置定时器T3
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void TIM3_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);
	TIM3_NVIC_Configuration();
	
    TIM_DeInit(TIM3);
    TIM_TimeBaseStructure.TIM_Period=99;		 								
    TIM_TimeBaseStructure.TIM_Prescaler= 9;			//48M/(9+1)*(99+1)=48KHz=9.6K*5的频率进入中断  
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 		
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
		TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	 TIM_ITConfig(TIM3, TIM_IT_Update , ENABLE);	
	 TIM_Cmd(TIM3, ENABLE);	/* 开启时钟 */ 
}
/***********************************************************
 * 函数名: TIM3_OFF
 * 描述  ：关闭定时器T3
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void TIM3_OFF(void)
{
	 TIM_ITConfig(TIM3, TIM_IT_Update , DISABLE);	
	 TIM_Cmd(TIM3, DISABLE);	
}
/***********************************************************
 * 函数名: TIM4_Configuration
 * 描述  ：配置定时器T4,外部时钟触发模式，dds方波作为触发
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void TIM4_Configuration(void)
{
	  /*GPIO_InitTypeDef GPIO_InitStructure;
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//配置TIM4外部触发IO
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_Init(GPIOE, &GPIO_InitStructure);
	
    TIM_DeInit(TIM4);
	
    TIM_TimeBaseStructure.TIM_Period=0xffff;		 								
    TIM_TimeBaseStructure.TIM_Prescaler= 0;				    
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 		
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
		TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

		TIM_ETRClockMode2Config(TIM4,TIM_ExtTRGPSC_OFF, TIM_ExtTRGPolarity_NonInverted, 0);
		
		TIM_SetCounter( TIM4, 0x00);
		TIM_Cmd(TIM4, ENABLE);	 */
		
		///////////////////////////////////
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	  NVIC_InitTypeDef         NVIC_InitStructure;
	
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
		
    TIM_TimeBaseStructure.TIM_Period=4999;		 								
    TIM_TimeBaseStructure.TIM_Prescaler= 7199;//10KHz计数，计数到5000为500ms				    
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
//		TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	  TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //使能指定的TIM4中断,允许更新中断

		NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn; //TIM4
    /*NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority=3;*/
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
		NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		
		//TIM_Cmd(TIM4, ENABLE);
}

/***********************************************************
 * 函数名: TIM4_ON
 * 描述  ：开启闭定时器T4
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void TIM4_ON(void)
{	
		TIM_Cmd(TIM4, ENABLE);
		//TIM_SetCounter( TIM4, 0x00);
}
/***********************************************************
 * 函数名: TIM4_OFF
 * 描述  ：关闭定时器T4
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void TIM4_OFF(void)
{
	 TIM_Cmd(TIM4, DISABLE);
}
//=============================================
void TIM2_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure; 
	 
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;	  
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void TIM2_Configuration(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
	TIM2_NVIC_Configuration();
	
  TIM_DeInit(TIM2);
  TIM_TimeBaseStructure.TIM_Period=1999;		 					/* 自动重装载寄存器周期的值(计数值) */
  //TIM_TimeBaseStructure.TIM_Prescaler= (36000 - 1);				    /* 时钟预分频数   例如：时钟频率=72MHZ/(时钟预分频+1) */ //0.5ms
	TIM_TimeBaseStructure.TIM_Prescaler= (24000 - 1);				      /* 时钟预分频数   例如：时钟频率=48MHZ/(时钟预分频+1) */ //0.5ms
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 			/* 采样分频 */
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 		/* 向上计数模式 */
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  TIM_ClearFlag(TIM2, TIM_FLAG_Update);							    /* 清除溢出中断标志 */
  TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
  TIM_Cmd(TIM2, ENABLE);											/* 开启时钟 */
}
//=============================================
/***********************************************************
 * 函数名: TIM3_IRQHandler
 * 描述  ：定时器3中断服务函数，按   频率进入中断，根据查找
           表写dds频率字
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void TIM3_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
  {
		  if(10 == tim3_cnt)	//打开PLL,1个时隙约20.83us
				{
						if(TIM3_CAM3_flag==0)
						{
							PLL_ON();
							TIM3_CAM3_flag=1;
						}
				}

			if(260 == tim3_cnt)  //等待250个时隙，VCC电压稳定及晶振起振
			{
					if(TIM3_CAM5_flag == 0) 
					{
							if(swchflag == 0)
							{
									SKY72310_Config();
									flag_channel = (1-flag_channel); //flag_channel=(AIS_CHANNEL)(1-flag_channel);
									Write_TX_Channel();
									
									msg_send1();//生成消息
							}
							else if(swchflag == 1)
							{
									SKY72310_Config();
									
									msg_send2();//生成消息
							}
							TIM3_CAM5_flag = 1;
					}
			}
			
			if(290 == tim3_cnt)  //配置PLL及锁定
			{
					if(TIM3_CAM1_flag == 0) 
					{
							PA_ON();
							TIM3_CAM1_flag = 1;
					}
			}
			
			if(485 == tim3_cnt)  //发送时隙到来时开启任务标志
			{
					if(task_flag1!=on)
					{					
							A=0;       //初始化
							B=0;
							num_bit=0;
							task_flag1=on;
					}
			}

			//
			//写PLL频率字
			//
			if(task_flag1==on)  
			{	
					SKY72310_Out(0x09,dds_word_real[msg18_24AB_add[B]][A]);
					while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
					SPI1_CS_HIGH();
				
					if(A==4)
					{
						A=0;
						B++;
					}
					else
					{
						A++;
					}
					num_bit++;
					
					// 发送结束判断
					if(num_bit>=var_m24)//设置一个变量var_m24=512*5-1
					{			
							task_flag1=off; //没有发送任务在执行
							task_flag2=off;//发送任务完成
							num_bit=0;  //初始化num_bit
							TIM3_CAM5_flag = 0;  //TIM3标记变量清零
							TIM3_CAM3_flag = 0;
							TIM3_CAM1_flag = 0;
						
							tim3_cnt=0;
       
						 
							if(swchflag == 0)//消息18 发送结束
							{
							}
							else if(swchflag == 1) //消息24AB发送结束
							{
								TIM_ITConfig(TIM3, TIM_IT_Update , DISABLE);
								TIM_Cmd(TIM3, DISABLE);
							}
							
							PLL_OFF();
							PA_OFF();
							swchflag ++;
							
							////////////////消息发送结束///////////////
							//判断是否要开GPS，GPS 3分钟开一次
							if(swchflag == 2)
							{
									if(interval_num == interval_s)
									{
											interval_num=0;
											BKP_WriteBackupRegister(BKP_DR1,interval_num);

											GPS_ON();
											TIM4_ON();
											TIM2_Configuration();
									}
									else if(interval_num > interval_s)
									{	
											interval_num = 0;
											BKP_WriteBackupRegister(BKP_DR1,interval_num);
											if(charging_flag == off)
											{
													RTC_Init();
													PWR_WakeUpPinCmd(ENABLE);
													PWR_EnterSTANDBYMode();
											}
									}
									else //<5
									{
											interval_num++;
											BKP_WriteBackupRegister(BKP_DR1,interval_num);
											if(charging_flag == off)
											{
													RTC_Init();
													PWR_WakeUpPinCmd(ENABLE);
													PWR_EnterSTANDBYMode();
											}
									}
							}
						
					}
			}
			
		 tim3_cnt++;
		 TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}	
}

//
//GPS失效时间判断
//
void TIM2_IRQHandler(void)  
{
	if( TIM_GetITStatus(TIM2 , TIM_IT_Update) != RESET ) 
	{
		
		tim2_cnt++;
		if(tim2_cnt == gps_invalid)     //2min
		{
				TIM_Cmd(TIM2, DISABLE);
				GPS_OFF();
				tim2_cnt = 0;
				LED_OFF();
				TIM4_OFF();

			 if(charging_flag == off)
			 {		
						RTC_Init();
						PWR_WakeUpPinCmd(ENABLE);
						PWR_EnterSTANDBYMode();
			 }
		}
				
		TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);
	}	
}

//
//开GPS时 绿灯闪
//
void TIM4_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	{
			if(time4flag == 0)
			{ 
					LED_ON();
					time4flag = 1;
			}
			else
			{    
					LED_OFF();
					time4flag = 0;
			}
			TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	}
}





