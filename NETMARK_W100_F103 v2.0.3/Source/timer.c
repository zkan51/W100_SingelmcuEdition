#include "main.h"

static u8 A;
static u16 B;
static u16 num_bit =0;
u8 TIM3_CAM1_flag=0; //��ֹ��������
u8 TIM3_CAM3_flag=0; //��ֹ��������
u8 TIM3_CAM5_flag=0; //��ֹ��������

u16 tim3_cnt;
int tim2_cnt; //��ʱ��2����
u8 swchflag;
static u8 time4flag=0;

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
    TIM_TimeBaseStructure.TIM_Prescaler= 9;			//48M/(9+1)*(99+1)=48KHz=9.6K*5��Ƶ�ʽ����ж�  
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
		
		//TIM_Cmd(TIM4, ENABLE);
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
		  if(10 == tim3_cnt)	//��PLL,1��ʱ϶Լ20.83us
				{
						if(TIM3_CAM3_flag==0)
						{
							PLL_ON();
							TIM3_CAM3_flag=1;
						}
				}

			if(260 == tim3_cnt)  //�ȴ�250��ʱ϶��VCC��ѹ�ȶ�����������
			{
					if(TIM3_CAM5_flag == 0) 
					{
							if(swchflag == 0)
							{
									SKY72310_Config();
									flag_channel = (1-flag_channel); //flag_channel=(AIS_CHANNEL)(1-flag_channel);
									Write_TX_Channel();
									
									msg_send1();//������Ϣ
							}
							else if(swchflag == 1)
							{
									SKY72310_Config();
									
									msg_send2();//������Ϣ
							}
							TIM3_CAM5_flag = 1;
					}
			}
			
			if(290 == tim3_cnt)  //����PLL������
			{
					if(TIM3_CAM1_flag == 0) 
					{
							PA_ON();
							TIM3_CAM1_flag = 1;
					}
			}
			
			if(485 == tim3_cnt)  //����ʱ϶����ʱ���������־
			{
					if(task_flag1!=on)
					{					
							A=0;       //��ʼ��
							B=0;
							num_bit=0;
							task_flag1=on;
					}
			}

			//
			//дPLLƵ����
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
					
					// ���ͽ����ж�
					if(num_bit>=var_m24)//����һ������var_m24=512*5-1
					{			
							task_flag1=off; //û�з���������ִ��
							task_flag2=off;//�����������
							num_bit=0;  //��ʼ��num_bit
							TIM3_CAM5_flag = 0;  //TIM3��Ǳ�������
							TIM3_CAM3_flag = 0;
							TIM3_CAM1_flag = 0;
						
							tim3_cnt=0;
       
						 
							if(swchflag == 0)//��Ϣ18 ���ͽ���
							{
							}
							else if(swchflag == 1) //��Ϣ24AB���ͽ���
							{
								TIM_ITConfig(TIM3, TIM_IT_Update , DISABLE);
								TIM_Cmd(TIM3, DISABLE);
							}
							
							PLL_OFF();
							PA_OFF();
							swchflag ++;
							
							////////////////��Ϣ���ͽ���///////////////
							//�ж��Ƿ�Ҫ��GPS��GPS 3���ӿ�һ��
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
//GPSʧЧʱ���ж�
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
//��GPSʱ �̵���
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





