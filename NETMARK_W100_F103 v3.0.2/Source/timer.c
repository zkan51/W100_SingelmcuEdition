#include "main.h"

u8 TIM3_CAM1_flag=0; //��ֹ��������
u8 TIM3_CAM3_flag=0; //��ֹ��������
u8 TIM3_CAM5_flag=0; //��ֹ��������

u16 tim3_cnt;
int tim2_cnt; //��ʱ��2����
u8 swchflag;
static u8 time4flag=0;
static u16 txLen = 0;
static u8 cmd[8];
static u8 condition  = 0x30; // ������ɺ󣬷���ready״̬
static u8 size_of_FIFO = 64; // оƬ�з���FIFO������Ϊ64 bytes
static u8 tx_threshold = 40; // FIFO�пճ��Ŀռ�ﵽ40 bytesʱ������TFAE�ж�

/***********************************************************
 * ������: TIM3_NVIC_Configuration
 * ����  �����ö�ʱ��T3�ж�
 * ����  : ��
 * ���  : ��
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
 * ������: TIM3_Configuration
 * ����  �����ö�ʱ��T3
 * ����  : ��
 * ���  : ��
 ***********************************************************/
void TIM3_Configuration(void)
{
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);
		TIM3_NVIC_Configuration();

		TIM_DeInit(TIM3);
		TIM_TimeBaseStructure.TIM_Period=99;		 								
		TIM_TimeBaseStructure.TIM_Prescaler= 9;			//48M/(9+1)*(99+1)=9.6K*5��Ƶ�ʽ����ж�  
		TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 		
		TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
		TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
		TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	 TIM_ITConfig(TIM3, TIM_IT_Update , ENABLE);	
	 TIM_Cmd(TIM3, ENABLE);	/* ����ʱ�� */ 
}
/***********************************************************
 * ������: TIM3_OFF
 * ����  ���رն�ʱ��T3
 * ����  : ��
 * ���  : ��
 ***********************************************************/
void TIM3_OFF(void)
{
	 TIM_ITConfig(TIM3, TIM_IT_Update , DISABLE);	
	 TIM_Cmd(TIM3, DISABLE);	
}
/***********************************************************
 * ������: TIM4_Configuration
 * ����  �����ö�ʱ��T4,�ⲿʱ�Ӵ���ģʽ��dds������Ϊ����
 * ����  : ��
 * ���  : ��
 ***********************************************************/
void TIM4_Configuration(void)
{
	  /*GPIO_InitTypeDef GPIO_InitStructure;
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//����TIM4�ⲿ����IO
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
    TIM_TimeBaseStructure.TIM_Prescaler= 7199;//10KHz������������5000Ϊ500ms				    
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
//		TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	  TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM4�ж�,��������ж�

		NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn; //TIM4
    /*NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority=3;*/
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
		NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		
		TIM_Cmd(TIM4, ENABLE);
}

/***********************************************************
 * ������: TIM4_ON
 * ����  �������ն�ʱ��T4
 * ����  : ��
 * ���  : ��
 ***********************************************************/
void TIM4_ON(void)
{	
		TIM_Cmd(TIM4, ENABLE);
		//TIM_SetCounter( TIM4, 0x00);
}
/***********************************************************
 * ������: TIM4_OFF
 * ����  ���رն�ʱ��T4
 * ����  : ��
 * ���  : ��
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
  TIM_TimeBaseStructure.TIM_Period=1999;		 					/* �Զ���װ�ؼĴ������ڵ�ֵ(����ֵ) */
  //TIM_TimeBaseStructure.TIM_Prescaler= (36000 - 1);				    /* ʱ��Ԥ��Ƶ��   ���磺ʱ��Ƶ��=72MHZ/(ʱ��Ԥ��Ƶ+1) */ //0.5ms
	TIM_TimeBaseStructure.TIM_Prescaler= (24000 - 1);				      /* ʱ��Ԥ��Ƶ��   ���磺ʱ��Ƶ��=48MHZ/(ʱ��Ԥ��Ƶ+1) */ //0.5ms
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 			/* ������Ƶ */
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 		/* ���ϼ���ģʽ */
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  TIM_ClearFlag(TIM2, TIM_FLAG_Update);							    /* �������жϱ�־ */
  TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
  TIM_Cmd(TIM2, ENABLE);											/* ����ʱ�� */
}
//=============================================
/***********************************************************
 * ������: TIM3_IRQHandler
 * ����  ����ʱ��3�жϷ���������   Ƶ�ʽ����жϣ����ݲ���
           ��дddsƵ����
 * ����  : ��
 * ���  : ��
 ***********************************************************/
void TIM3_IRQHandler(void)
{	
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
  {
		  if(10 == tim3_cnt)	// ��si4463,1��ʱ϶Լ20.83us
				{
						if(TIM3_CAM3_flag==0)
						{
							SI4463_ON();
							TIM3_CAM3_flag=1;
						}
				}

			if(1000 == tim3_cnt)  //�ȴ�1000��ʱ϶��оƬ�ȶ�
			{
					if(TIM3_CAM5_flag == 0) 
					{
							if(swchflag == 0)
							{
									flag_channel = (1-flag_channel); //flag_channel=(AIS_CHANNEL)(1-flag_channel);
									Write_TX_Channel();
								 SI446X_CONFIG_INIT(); 
									txLen = 32;
									msg_send1();//������Ϣ18
							}
							else if(swchflag == 1)
							{
						  SI446X_CONFIG_INIT(); 
							 txLen = 64;								
								msg_send2();//������Ϣ24AB
							}
							TIM3_CAM5_flag = 1;
					}
			}
			
			if(1030 == tim3_cnt)  //��PA
			{
					if(TIM3_CAM1_flag == 0) 
					{
							PA_ON();
							TIM3_CAM1_flag = 1;
					}
			}
			
			if(1230 == tim3_cnt)  //����ʱ϶����ʱ���������־
			{
					if(task_flag1!=on)
					{					
							task_flag1=on;
					}
			}

			//
			if(task_flag1==on)  
			{	
					//����AIS��Ϣ
					SI446X_SEND_PACKET(txBuf, txLen, flag_channel, condition, size_of_FIFO, tx_threshold);
				
					task_flag1=off; //û�з���������ִ��
					task_flag2=off;//�����������
					TIM3_CAM5_flag = 0;  //TIM3��Ǳ�������
					TIM3_CAM3_flag = 0;  //SI4463������־
					TIM3_CAM1_flag = 0;  //PA������־
				
					tim3_cnt=0;
					
					
					if(swchflag == 0)//��Ϣ18 ���ͽ���
					{
					}
					else if(swchflag == 1) //��Ϣ24AB���ͽ���
					{
						TIM_ITConfig(TIM3, TIM_IT_Update , DISABLE);
						TIM_Cmd(TIM3, DISABLE);
					}
					
					SI4463_OFF();
					PA_OFF();
					swchflag ++;
					
					////////////////��Ϣ���ͽ���///////////////
					//�ж��Ƿ�Ҫ��GPS��GPS 3���ӿ�һ��
					if(swchflag == 2)
					{
								if (interval_num == 0) // ��GPS
								{
											GPS_ON();
											//TIM4_ON();
											TIM4_Configuration();
											tim2_cnt = 0;
											TIM2_Configuration();
											interval_num++;
											if (intervalA == 0)
														interval_num = 0;
											BKP_WriteBackupRegister(BKP_DR1,interval_num);
								}
								else  //GPS ���ϴο���δ��3����
								{
											if (intervalA == 5) // ���ͼ��30s
											{
														interval_num++;
														if (interval_num == 4)
																	interval_num = 0;
														BKP_WriteBackupRegister(BKP_DR1,interval_num);
														RTC_Init();
														PWR_WakeUpPinCmd(ENABLE);
														PWR_EnterSTANDBYMode();
											} 
											else if (intervalA == 2)  // ���ͼ��Ϊ1����
											{
														interval_num++;
														if (interval_num == 2)
																	interval_num = 0;
														BKP_WriteBackupRegister(BKP_DR1,interval_num);
														RTC_Init();
														PWR_WakeUpPinCmd(ENABLE);
														PWR_EnterSTANDBYMode();
											}
											else 
											{
											}
								}
					}
			}
			
		 tim3_cnt++;
		 TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}	
}

//
//GPSʧЧʱ���ж�
//
void TIM2_IRQHandler(void)  
{
	if( TIM_GetITStatus(TIM2 , TIM_IT_Update) != RESET ) 
	{
		tim2_cnt++;
		// ��GPSʱ�䳬��2����
		if(tim2_cnt == gps_invalid)     //2min
		{
				TIM_Cmd(TIM2, DISABLE);
				GPS_OFF();
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
//��GPSʱ �̵���
//
void TIM4_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	{
		  if(battery >= BATTERYLEVEL)
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
				}
				else 
				{
					 if(time4flag == 0)
						{
								LED_RED_ON();
						  time4flag = 1;
						}
						else 
						{
								LED_RED_OFF();
							 time4flag = 0;
						}
				}
			TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	}
}





