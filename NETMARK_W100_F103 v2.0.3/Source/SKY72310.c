#include "main.h"

void SKY72310_Init(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	
	/*!< Configure SPI_FLASH_SPI pins: SCK */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/*!< Configure SPI_FLASH_SPI pins: MISO */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/*!< Configure SPI_FLASH_SPI pins: MOSI */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/*!< Configure SPI_FLASH_SPI_CS_PIN pin: SPI_FLASH Card CS pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	SPI1_CS_HIGH();
	/* SPI1 configuration */
	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;//SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;//  SPI_BaudRatePrescaler_16;//2.5Mbps             // SPI_BaudRatePrescaler_2;//10Mbps
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStructure);
	
	/* Enable SPI1  */
	SPI_Cmd(SPI1, ENABLE);
}

void SKY72310_Config(void)  //初始化配置总共花费100us
{
	u16 i;
	
	SKY72310_Out(0x07,0x200); //三态，开芯片配置
	for(i=0;i<200;i++);	
	SKY72310_Out(0x05,0x03);  //参考时钟12.8M，4分频，步进12.2Hz
	for(i=0;i<200;i++);
	SKY72310_Out(0x06,0x1F);  //鉴相增益31	
	for(i=0;i<200;i++);
	
	/////////13M晶振
  SKY72310_Out(0x00,0x12);  //Nerg				

	Read_TX_Channel();
	if(flag_channel == 1)
	{			
		SKY72310_Out(0x02,0x40);  // 162.025M  LSB 8bits
		SKY72310_Out(0x01,0x36B); // 162.025M  MSB 10bits
	}
	else
	{		
		SKY72310_Out(0x02,0x7C);  // 161.975M 
		SKY72310_Out(0x01,0x35B);   // 161.975M  			
	}
	/////////
	
	for(i=0;i<2000;i++);	

	SKY72310_Out(0x08,0x00);  //address enable,DATA input,offset 0
	SKY72310_Out(0x09,0x00);  //open
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);	
	SPI1_CS_HIGH();
	
}

void SPI1_SendByte(u16 byte)	   //修改为16bit数据
{
	SPI1_CS_HIGH();	   
	SPI1_CS_LOW();  

	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);	
	SPI_I2S_SendData(SPI1, byte);
}

void SKY72310_Out(unsigned char Address,short data)
{
	//unsigned long Output_Number;
	u16 Output_Number;

	if(data >= 0)
	{	
		Output_Number = ((Address&0x0f)<<12) | data;
		SPI1_SendByte(Output_Number);
	}
	else
	{
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
		Output_Number = (data & 0xfff) | 0x800;
		Output_Number = Output_Number | ((Address&0x0f)<<12);
		SPI1_SendByte(Output_Number);	
	}
	
}












