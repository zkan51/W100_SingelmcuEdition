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
	u16 lh;	
	
	while(1)
	{
		
		if(Level_PB12 == 0 || Level_PB14 == 1)
		{
			break;
		}
		
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, DISABLE);
		
		PA_OFF();
		GPS_ON();  //GPS一直打开
			
		SI4463_ON();
		LED_ON();
		Delay(0xff);Delay(0xff);
		Delay(0xff);Delay(0xff);
		Delay(0xff);Delay(0xff);  //共354us Delay //Delay(0xff)：59us
		
		SKY72310_Init();
		SKY72310_Config(); //初始化配置总共花费100us

	  Delay(0xff);		
	  Delay(0xff);
		 Delay(0xff);	//共118us Delay //Delay(0xff)：59us

  	SKY72310_Out(0x08,0x00);  //address enable,DATA input,offset 0
  	SKY72310_Out(0x09,00);  //open
  	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);	
  	SPI1_CS_HIGH();

  for(lh=2800;lh;lh--)
		{
			SKY72310_Init();
			SKY72310_Config(); //初始化配置总共花费100us

			SKY72310_Out(0x08,0x00);  //address enable,DATA input,offset 0
			SKY72310_Out(0x09,00);  //open
			while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);	
			SPI1_CS_HIGH();
		
			Delay(0xff);Delay(0xff);		
		}
		
		SI4463_OFF();
		LED_OFF();
  		
		Delay(0xff);Delay(0xff);
		Delay(0xff);Delay(0xff);
		Delay(0xff);Delay(0xff);
		Delay(0xff);Delay(0xff);
		Delay(0xff);Delay(0xff);
		Delay(0xff);Delay(0xff);
		
	  PA_ON();	
  for(ii=60;ii;ii--)
		{
		   Delay(72033); //1s
		}
		
	  PA_OFF();
		Delay(0xff);Delay(0xff);
		Delay(0xff);Delay(0xff);
		Delay(0xff);Delay(0xff);
		Delay(0xff);Delay(0xff);
		Delay(0xff);Delay(0xff);
		Delay(0xff);Delay(0xff);
		
    for(ii=1500;ii;ii--)
		{
		   Delay(72033); //25s
		}		

	}	
}
	
void EjectTest(void)  //发射测试程序
{
	u16 ii;
	
	while(1)
	{
		if(Level_PB12 == 1 || Level_PB14 == 0)
		{
			break;
		}
		
		PA_OFF();
		GPS_OFF();
		
    for(ii=100;ii;ii--)
		{
		   Delay(72033); //发射延时1.6s
		}
				
		SI4463_ON();
		Delay(0xff);Delay(0xff);
		Delay(0xff);Delay(0xff);
		Delay(0xff);Delay(0xff);
		Delay(0xff);Delay(0xff);
		Delay(0xff);Delay(0xff);
		Delay(0xff);Delay(0xff);  //共354us Delay //Delay(0xff)：59us
		
		SKY72310_Init();
		SKY72310_Config(); //初始化配置总共花费100us

		Delay(0xff);		
		Delay(0xff);
		Delay(0xff);	//共118us Delay //Delay(0xff)：59us

		SKY72310_Out(0x08,0x00);  //address enable,DATA input,offset 0
		SKY72310_Out(0x09,00);  //open
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);	
		SPI1_CS_HIGH();
		
	 PA_ON();		
		for(ii=120;ii;ii--)
		{
		   Delay(72033); //2s
		}

		SI4463_OFF();
	 PA_OFF();	
  for(ii=240;ii;ii--)
		{
		   Delay(72033); //4s
		}

	}
}
