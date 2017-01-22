#include "main.h"

int tim2_cnt; //��ʱ��2����
ais_status led_state=off;
static u8 ledShiningCnt=0;
/***********************************************************
 * ������: TIM4_Configuration
 * ����  �����ö�ʱ��T4,�ⲿʱ�Ӵ���ģʽ��dds������Ϊ����
 * ����  : ��
 * ���  : ��
 ***********************************************************/
void TIM4_Configuration(void)
{
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	 NVIC_InitTypeDef         NVIC_InitStructure;
	
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
		
		TIM_TimeBaseStructure.TIM_Period=1999;		 				
#ifdef SYSCLK_8M	
		TIM_TimeBaseStructure.TIM_Prescaler= 4000-1;// 0.1*10000 = 1s	
#else
		TIM_TimeBaseStructure.TIM_Prescaler= 24000-1;// 0.1*10000 = 1s	
#endif
	
		TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
		TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
		TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
		TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM4�ж�,���������ж�
  TIM_ClearFlag(TIM4, TIM_FLAG_Update);
	
		NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn; //TIM4
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
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
	 
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;	  
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
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
#ifdef	SYSCLK_8M
		TIM_TimeBaseStructure.TIM_Prescaler= (4000 - 1);				      // ʱ��Ԥ��Ƶ��    //0.5*2000 = 1s
#else
		TIM_TimeBaseStructure.TIM_Prescaler= (24000 - 1);				      // ʱ��Ԥ��Ƶ��    //0.5*2000 = 1s
#endif
	
		TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 			/* ������Ƶ */
		TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 		/* ���ϼ���ģʽ */
		TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
		TIM_ClearFlag(TIM2, TIM_FLAG_Update);							    /* �������жϱ�־ */
//  TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
//  TIM_Cmd(TIM2, ENABLE);											/* ����ʱ�� */
}

/***********************************************************
 * ������: TIM2_OFF
 * ����  ���رն�ʱ��T2
 * ����  : ��
 * ���  : ��
 ***********************************************************/
void TIM2_OFF(void)
{
	 TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE );
	 TIM_Cmd(TIM2, DISABLE);
}
/***********************************************************
 * ������: TIM2_OFF
 * ����  ���رն�ʱ��T2
 * ����  : ��
 * ���  : ��
 ***********************************************************/
void TIM2_ON(void)
{
	 TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE );
	 TIM_Cmd(TIM2, ENABLE);
}

//=============================================


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
			Sys_Standby();
		}
				
		TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);
	}	
}

//
//��GPSʱ �̵���
//
void TIM4_IRQHandler(void)
{
	u8 ledShining;
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	{
		ledShining = Read_OpenOnce();
		if(ledShining && ledShiningCnt<6) //����ʱ���̵�һ��������
		{
			ledShiningCnt++;
			if(led_state==0)
			{
				LED_ON();
				LED_RED_ON();
				led_state = !led_state;
			}
			else 
			{
				LED_OFF();
				LED_RED_OFF();
				led_state = !led_state;
			}
			if(6==ledShiningCnt)
			{
				openflag = 1;
				BKP_WriteBackupRegister(BKP_DR2,openflag);
			}
		}
		else 
		{
			if(led_state==0)
			{
					if(battery >= BATTERYLEVEL)
					{
						LED_ON();
					}
					else 
					{
						LED_RED_ON();
					}
					led_state = !led_state;
				}
				else
				{
					if(battery >= BATTERYLEVEL)
					{
						LED_OFF();
					}
					else 
					{
						LED_RED_OFF();
					}
					led_state = !led_state;
				}
		}
		
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	}
}




