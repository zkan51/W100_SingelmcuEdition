#include "main.h"

void EXTI_Configuration(void)
{
  EXTI_InitTypeDef EXTI_InitStructure;
  //GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource9);//PA9���ӵ��ⲿ�ж�ͨ��9

  EXTI_InitStructure.EXTI_Line = EXTI_Line17;			  //�ⲿ�ж�ͨ��17
  //EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	  //�ⲿ�ж�ģʽ
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //�����ش���
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;				  //ʹ��
  EXTI_Init(&EXTI_InitStructure);
}

void SoftReset(void)
{
 __set_FAULTMASK(1);      // ??????
 NVIC_SystemReset();// ??
}
/*
 * ��������NVIC_Configuration
 * ����  ������RTC���жϵ����ж����ȼ�Ϊ1�������ȼ�Ϊ0
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
 */
static void RTC_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  /* Enable the RTC Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTCAlarm_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/*
 * ��������RTC_Configuration
 * ����  ������RTC
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
 */
static void RTC_Configuration(void)
{
	//rtc_cnt=0;
  /* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Reset Backup Domain */
  //BKP_DeInit();

	
	RCC_LSICmd(ENABLE);

	while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
	{
	}
	
  /* Select LSI as RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
	
  /* Enable RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC registers synchronization */
  RTC_WaitForSynchro();

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Enable the RTC Alarm */
  RTC_ITConfig(RTC_IT_ALR, ENABLE);

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Set RTC prescaler: set RTC period to 1/37.5 sec ;make sure that the time of one counter is a AIS slot*/
  RTC_SetPrescaler(32767); /* fTR_CLK = fRTCCLK/(PRL[19:0]+1)  = 40K/32768 = 1.22HZ =0.82s */
	
	/* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
	
	EXTI_Configuration();
	
	/*Sets the RTC alarm value:Sets the RTC alarm value to 1800sec*/
	switch(intervalA)  //����flash��ļ��ֵ������rtc���Ѽ��
	{
		case 5:
			RTC_SetAlarm(36);//30S
		break;
		case 2:
			RTC_SetAlarm(73);//1����
		break;
		case 0:
			RTC_SetAlarm(219);//3����
		break;
		default:
			RTC_SetAlarm(73);
		break;
	}
	
	/* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
	
	/*Sets the RTC counter value:Sets the RTC counter value to 0*/
	RTC_SetCounter(0x0);

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
}

void RTC_Init(void)
{
	/* ����RTC���ж����ȼ� */
	RTC_NVIC_Configuration();

  /* RTC Configuration */
  RTC_Configuration();
}

uint16_t Get_RTC_Counter(void)
{
  uint16_t tmp = 0;
  tmp = RTC->CNTL;
  return (tmp) ;
}

void RTC_Enter_StandbyMode(void)
{
	switch(intervalA)  //����flash��ļ��ֵ������rtc���Ѽ��
	{
		case 5:
			RTC_SetAlarm(35);
		break;
		case 2:
			RTC_SetAlarm(74);
		break;
		case 0:
			RTC_SetAlarm(222);
		break;
		default:
			RTC_SetAlarm(74);
		break;
	}
	
	RTC_WaitForLastTask();
	
	PWR_EnterSTANDBYMode();
}

/*void RTC_IRQHandler(void)
{
	if (RTC_GetITStatus(RTC_IT_ALR) != RESET)
	{
		RTC_ClearITPendingBit(RTC_IT_ALR);
		
 		RTC_SetCounter(0x0);
 		RTC_WaitForLastTask();
		
USART_SendData(USART1, 0x5B);
while (!(USART1->SR & USART_FLAG_TXE)) ;
		
		PWR_WakeUpPinCmd(ENABLE);
		RTC_Enter_StandbyMode();
		
	}
	
	EXTI_ClearITPendingBit(EXTI_Line17);
}*/

void RTCAlarm_IRQHandler(void)
{     
	if(RTC_GetITStatus(RTC_IT_ALR) != RESET)
	{
		RTC_ClearITPendingBit(RTC_IT_ALR);
		RTC_WaitForLastTask();
		
		EXTI_ClearITPendingBit(EXTI_Line17);
		RTC_WaitForLastTask();
		
 		RTC_SetCounter(0x0);
 		RTC_WaitForLastTask();
		
USART_SendData(USART1, 0x5B);
while (!(USART1->SR & USART_FLAG_TXE)) ;
		
// 		PWR_WakeUpPinCmd(ENABLE);
// 		RTC_Enter_StandbyMode();		
	}
	
}














