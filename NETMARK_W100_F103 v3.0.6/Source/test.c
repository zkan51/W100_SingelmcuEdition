#include "main.h"

void ProgramSelector(void) //����������ѡ��
{
	if(Level_PB12 == Level_PB14)//��������
	{
		
	}	
	else if(Level_PB12 == 1 && Level_PB14 == 0 )//���ϻ�
	{
		CurrentTest();
	}
	else if(Level_PB12 == 0 && Level_PB14 == 1)//������
	{
		EjectTest();
	}
}

void CurrentTest(void) //���ϻ����Գ���
{
	u16 ii;
	u8 cmd[8] = {0};
	u8 start[] = {0x31};

 GPS_ON();  //GPSһֱ��
	PA_OFF();
 RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, DISABLE);


	while(1)
	{
		if(Level_PB12 == 0 || Level_PB14 == 1)
		{
				break;
		}
		
	SI4463_ON();
 delay_ms(100);
	SI446X_SINGLE_FREQ_CONFIG_INIT(); 
		
		LED_ON();
		delay_ms(200);
		
		// �ص���ready��״̬
		cmd[0] = 0x34;
		cmd[1] = 0x03;
		SI446X_CMD( cmd, 2 );	
		
		SI446X_CMD( start, 1 ); // ��ʼ����162.025MHz��Ƶ�ź�
		delay_ms(1000);
		delay_ms(1000);
		
		LED_OFF();  		
		SI4463_OFF();
	 PA_ON();	
  delay_ms(1000);
	 PA_OFF();
		
		//25s
  for(ii=25;ii;ii--)
		{
				delay_ms(1000); 
		}		
	}	
}
	
void EjectTest(void)  //������Գ���
{
	u8 cmd[8] = {0};
	u8 start[] = {0x31};

	GPS_OFF();
	PA_OFF();


	while(1)
	{
		if(Level_PB12 == 1 || Level_PB14 == 0)
		{
			break;
		}
		
		delay_ms(1000);//�ӳ�1.6s
		delay_ms(600);
			
		SI4463_ON(); //ʹ��4463
		delay_ms(100);
		SI446X_SINGLE_FREQ_CONFIG_INIT(); //����4463����
		// �ص���ready��״̬
		cmd[0] = 0x34;
		cmd[1] = 0x03;
		SI446X_CMD( cmd, 2 );				
		
		PA_ON();
		SI446X_CMD( start, 1 ); // ��ʼ����162.025MHz��Ƶ�ź�
		
		delay_ms(1000); //�ӳ�2s
		delay_ms(1000);
		
	 PA_OFF();
		SI4463_OFF();
		
		delay_ms(1000); //�ӳ�4s
		delay_ms(1000);
		delay_ms(1000);
		delay_ms(1000);

	}
}
