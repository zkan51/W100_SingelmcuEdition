#include "main.h"
#include "stm32f10x_adc.h"
/*********************mkmsg.c*************************/
u8 msg[45]={0};//�洢AIS��Ϣ
u8 len; //������Ϣ���ֽ���
unsigned long MMSI=412440046; //��λ��
u8 boatnum[16]={0x60,0x34,0x93,0x8c,0x24,0x13,0x99,0x57,0x0d,0x73,0xcc,0x0c,0x31,0x00,0x00,0x00}; //���� MINLIANYU0533@01
u8 Vender_ID[6]= {0x21,0x31,0x2d,0x38,0xd2,0xc0}; //�����̺�,��Ϣ24Bʹ�� HSD-NMK
u8 Call_sign[6]={0x00,0x00,0x00,0x00,0x00,0x00}; //���ţ���Ϣ24Bʹ��
u16 boatlong; //����
u8 boatwidth; //����
u16 boatsize_a ; //�ബ�� 
u16 boatsize_b ; //�ബβ a+b = ����
u8 boatsize_c ;	//������
u8 boatsize_d ;	//������ c+d = ����
unsigned long jingdu=0; //GPS����
unsigned long weidu=0; //GPSγ��
unsigned int sog=0; //msg18 SOG
unsigned int direction=0; //msg18 ʵ�ʺ���
unsigned long gpstime=0; //gpsʱ��
//////msg21/////
u8 dev_type = 4;//1;//�����豸���� 5bit
u8 dev_name[54]={0x48,0x05,0x04,0x0C,0x17,0x1C,0xB3,0x04,0x20,0xC4}; //APPLE123ABCD
u8 dev_local[5]={0x0C,0x21,0x24,0x0A,0x0A}; //�ߴ�λ�òο��� 30bit����5���ֽ�,ֻ�õ�6λ ;6 6 6 6 6 6
u8 dev_time; //ʱ�� 6bit
u8 dev_state; //AtoN״̬ 8bit
u8 dev_etype = 1; //���Ӷ�λװ������ 4bit Ĭ��ΪGPS

////////////////
/*********************msg_slot.c*************************/
u8 msg18_add[256]={0};//�洢��Ϣ18��ӦƵ���ֲ��ұ�ĵ�ַ��ÿһ��Ԫ��ȡֵ��Χ��0~16��
u8 msg24_A[256]={0};//�洢��Ϣ24A
u8 msg24_B[256]={0};//�洢��Ϣ24B
u8 msg21_add[512]={0};//�洢��Ϣ21��ӦƵ���ֲ��ұ�ĵ�ַ��ÿһ��Ԫ��ȡֵ��Χ��0~16��
u8 msg18_24AB_add[768];//�洢��Ϣ18��24A/B��ӦƵ���ֲ��ұ�ĵ�ַ��ÿһ��Ԫ��ȡֵ��Χ��0~16��
//u16 msequence_poly=0x8805;	//16λPN�����ɶ���ʽ������������������һ�����������λ������д�롣
u16 slot_increacement=0 ; //���ʱ϶�������뷢��������interval_s��ӣ����������ʱ������
//u16 interval_s=1125; //375;      //2250; //��ʼ��������1min.   //6750   
u16 interval_s;  //��������仯ֵ 
u16 interval_num ; //�������������仯ֵ
u16 gps_invalid = 0x78; //gpsʧЧ�Ĵ���ʱ��2����
//u16 gps_invalid_num; //ʧЧ����ֵ
//u16 time_o;//GPSǰ��ʱ��洢
u16 intervalA;  //����������� �̶�ֵ
u16 intervalB;  //����������� ��ʱֵ
u8 cogsel=0; //����������ѡ����� Ĭ��0=����
u8 txBuf[64]={0}; //����AIS��Ϣ����

//union dds_union dds_word_update[16][5]={0x00000000};  //�������º��Ƶ���ֲ��ұ�

ais_status sendTask=on;//������������־
u8 flag_channel=0;		 //�ŵ�ѡ���־λ,��ʼ��Ϊ�ŵ�1
u8 flag_charging = 0; //����־λ
/*********************gps.c*************************/									
GPS_INFO GPS; //gps��Ϣ�ṹ��

/*********************exti.c*************************/	
u8 CodeNum[6]; //����
u8 code_cnt=0; //������ȷ����־��code_cnt=6ʱ������ȷ��
ais_status encryption=off; //�Ƿ�д�������־��Ĭ��Ϊδд����
u16 activation_flag=0;  //�����־��Ĭ��Ϊ0
u32 ChipUniqueID_Flash[3];   //оƬ96bit UID

/*********************adc.c*************************/
u8 battery; //�����ȼ�
void Delay(__IO u32 nCount)
{
  for(; nCount != 0; nCount--);
}

//��������Ϣ
unsigned long Time;
u8 AgencyName[16]={0};
u8 openflag; //����

int main(void)
{
	SYS_CLK_DOWN();  //����ϵͳʱ��Ϊ8M
	
	RCC->APB2ENR|=1<<2;     //��ʹ������IO PORTAʱ��
	RCC->APB2ENR|=1<<0;     //��������ʱ��
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //�ж����ȼ�����2
	delay_init();		        //ʵ�־�ȷ��ʱ����
	GPIO_Configuration();  //GPIO

	USART1_Config();       //��д��ӿ�
	USART2_Config();       //GPS����
	TIM2_Configuration();  //GPS����ʱ��
	TIM4_Configuration();	 //��GPSʱ����
	Adc_Init();
	BackupRegisterInit();
	Read_GPS_Info();
	ReadFlashInit();       //��ȡflash����
	Exti_Init();
	// si4463��ʼ��
	SPI1IO_Init();			      // IOģ��SPI
	SI4463_IOSET();	
	
	
	//�ж��Ƿ�Ϊ���״̬
	if(isCharging)
	{
		flag_charging  = 1;
	}
	
	ProgramSelector(); //���뿪��
	
	while(1)
 {	
		if(flag_charging==0)
		{
			send_on();
		}
		else
		{
			chargingState();
			while(flag_charging);
			ExitCharging();
		}
 }
}



