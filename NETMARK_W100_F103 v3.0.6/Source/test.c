#include "main.h"

void ProgramSelector(void) //拨码器程序选择
{
	if(Level_PB12 == Level_PB14)//正常工作
	{
		
	}	
	else if(Level_PB12 == 1 && Level_PB14 == 0 )//电老化
	{
		CurrentTest();
	}
	else if(Level_PB12 == 0 && Level_PB14 == 1)//电性能
	{
		EjectTest();
	}
}

void CurrentTest(void) //电老化测试程序
{
	u16 ii;
	u8 cmd[8] = {0};
	u8 start[] = {0x31};

 GPS_ON();  //GPS一直打开
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
		
		// 回到“ready”状态
		cmd[0] = 0x34;
		cmd[1] = 0x03;
		SI446X_CMD( cmd, 2 );	
		
		SI446X_CMD( start, 1 ); // 开始发射162.025MHz单频信号
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
	
void EjectTest(void)  //发射测试程序
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
		
		delay_ms(1000);//延迟1.6s
		delay_ms(600);
			
		SI4463_ON(); //使能4463
		delay_ms(100);
		SI446X_SINGLE_FREQ_CONFIG_INIT(); //配置4463参数
		// 回到“ready”状态
		cmd[0] = 0x34;
		cmd[1] = 0x03;
		SI446X_CMD( cmd, 2 );				
		
		PA_ON();
		SI446X_CMD( start, 1 ); // 开始发射162.025MHz单频信号
		
		delay_ms(1000); //延迟2s
		delay_ms(1000);
		
	 PA_OFF();
		SI4463_OFF();
		
		delay_ms(1000); //延迟4s
		delay_ms(1000);
		delay_ms(1000);
		delay_ms(1000);

	}
}
