#include "main.h"

void ProgramSelector(void) //拨码器程序选择
{
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)== 1)
	{
// USART_SendData(USART1, 0x5B);
// while (!(USART1->SR & USART_FLAG_TXE)) ;
	}	
	else if( (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12) == 0) && (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14) == 0) )
	{
		CurrentTest();
	}
	else if( (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12) == 0) && (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14) == 1) )
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
		
		if( (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12) != 0) || (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14) != 0) )
		{
			break;
		}
		
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, DISABLE);
		
		PA_OFF();
		GPS_ON();  //GPS一直打开
			
		PLL_ON();
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
		
		PLL_OFF();
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
		if( (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12) != 0) || (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14) != 1) )
		{
			break;
		}
		
		PA_OFF();
		GPS_OFF();
		
    for(ii=100;ii;ii--)
		{
		   Delay(72033); //发射延时1.6s
		}
				
		PLL_ON();
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

		PLL_OFF();
	  PA_OFF();	
    for(ii=240;ii;ii--)
		{
		   Delay(72033); //4s
		}

	}
}
