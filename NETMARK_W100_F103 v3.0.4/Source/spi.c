//#include "sys.h"
#include "spi.h"


void SPI1IO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;   	
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE );// 时钟使能

	// PB0
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0 ;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;       //PB0-->SCLK
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	// PA6
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;       //PA6-->SDO 单片机数据输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// PA7
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;  //PA7<--SDI 单片机数据输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
		 
}

  











