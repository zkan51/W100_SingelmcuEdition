//#include "sys.h"
#include "spi.h"


void SPI1IO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;   	
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA|RCC_APB2Periph_SPI1|RCC_APB2Periph_GPIOB, ENABLE );//GPIOA ʱ��ʹ��, SPI1 ʱ��ʹ��
	#if SOFTSPI // ���ģ��SPI
	// PB0
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0 ;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;       //PB0-->SCLK
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	// PA6
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;       //PA6-->SDO ��Ƭ���������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// PA7
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;  //PA7<--SDI ��Ƭ����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	#else // Ӳ��SPI(������)
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;  //GPIOA 5 6 7����������� 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	#endif	 	 		 
}

//SPI �ٶ����ú���
//SpeedSet:
//SPI_BaudRatePrescaler_2   2��Ƶ   
//SPI_BaudRatePrescaler_8   8��Ƶ   
//SPI_BaudRatePrescaler_16  16��Ƶ  
//SPI_BaudRatePrescaler_256 256��Ƶ 
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler)
{
  	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPI1->CR1&=0XFFC7;
	SPI1->CR1|=SPI_BaudRatePrescaler;	//����SPI1�ٶ� 
	SPI_Cmd(SPI1,ENABLE);  
} 
void SPI1Par_Set(void)
{	 		
  	SPI_InitTypeDef  SPI_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA|RCC_APB2Periph_SPI1, ENABLE );//GPIOA ʱ��ʹ��, SPI1 ʱ��ʹ��	
		 //  LOW 2 �з�Ӧ ���Զ�� ע��ӿڰ���
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low; //SPI_CPOL_Low;		//����ͬ��ʱ�ӵĿ���״̬Ϊ �ߵ�ƽ
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//����ͬ��ʱ�ӵĵ�1�������أ��������½������ݱ�����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ
	SPI_Init(SPI1, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
	SPI_Cmd(SPI1, ENABLE); //ʹ��SPI����	
	//SPI1_ReadWriteByte(0xff);//��������
}   
void SPI1_Init(void)
{

	#if SOFTSPI 
	SPI1IO_Init(); 
	#else
	SPI1IO_Init();
	SPI1Par_Set();
	#endif		
}
  











