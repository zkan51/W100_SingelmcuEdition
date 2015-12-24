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
u8 msg24A_add[256]={0};//�洢��Ϣ24A��ӦƵ���ֲ��ұ�ĵ�ַ��ÿһ��Ԫ��ȡֵ��Χ��0~16��
u8 msg24B_add[256]={0};//�� ����Ϣ24B��ӦƵ���ֲ��ұ�ĵ�ַ��ÿһ��Ԫ��ȡֵ��Χ��0~16��
u8 msg21_add[512]={0};//�� ����Ϣ21��ӦƵ���ֲ��ұ�ĵ�ַ��ÿһ��Ԫ��ȡֵ��Χ��0~16��
u8 msg18_24AB_add[768];//�洢��Ϣ18��24A/B��ӦƵ���ֲ��ұ�ĵ�ַ��ÿһ��Ԫ��ȡֵ��Χ��0~16��
//u16 msequence_poly=0x8805;	//16λPN�����ɶ���ʽ������������������һ�����������λ������д�롣
u16 slot_increacement=0 ; //���ʱ϶�������뷢��������interval_s��ӣ����������ʱ������
u16 TIM3_CAM1=0;  //ʱ϶�ţ���RTC�������е�ʱ϶ֵ�Ƚ�
u16 TIM3_CAM2=0;
u16 TIM3_CAM3=0;
u16 TIM3_CAM4=0;
u16 TIM3_CAM5=0;  //PLL��ǰ�򿪵�ʱ϶
u16 var_m24; //��¼������Ϣbit������ֻ������Ϣ18ʱ��var_m24=256*5-1;��������Ϣ24ʱ��var_m24=512*5-1;
//u16 interval_s=1125; //375;      //2250; //��ʼ��������1min.   //6750   
u16 interval_s;  //��������仯ֵ 
u16 interval_num; //�������������仯ֵ
u16 gps_invalid; //gpsʧЧ�Ĵ���ʱ��
//u16 gps_invalid_num; //ʧЧ����ֵ
//u16 time_o;//GPSǰ��ʱ��洢
u16 intervalA;  //����������� �̶�ֵ
u16 intervalB;  //����������� ��ʱֵ
u8 cogsel=0; //����������ѡ����� Ĭ��0=����
int  dds_word_real[9][5] = {
														-197 ,-197 ,-197 ,-197 ,-197 ,
														-196 ,-190 ,-172 ,-126 ,-47 ,
														46 ,120 ,147 ,120 ,46 ,
														47 ,126 ,172 ,190 ,196 ,
														-47 ,-126 ,-172 ,-190 ,-196 ,
														-46 ,-120 ,-147 ,-120 ,-46 ,
														196 ,190 ,172 ,126 ,47 ,
														197 ,197 ,197 ,197 ,197 ,
	
														0,0,0,0,0
														}; //��Ƶƫ��Ƶ���ֲ��ұ�

//union dds_union dds_word_update[16][5]={0x00000000};  //�������º��Ƶ���ֲ��ұ�

ais_status task_flag2=on;//����������ɱ�־����ֵ������Ϊon,������ʶ�Ƿ���AIS��Ϣ
ais_status flag_initial_slot=on;  //��һ�η���ʱ϶��־����ֵһ����λon����һ��GPS��Ч��10s������AIS��Ϣ
ais_status flag_m24=on;//�Ƿ�����Ϣ24��־��on--������Ϣ18����Ϣ24��off--ֻ������Ϣ18��
ais_status task_flag1=off; //���η���ִ�б�־����ֵΪoff
u8 flag_channel=0;		 //�ŵ�ѡ���־λ,��ʼ��Ϊ�ŵ�1
									
/*********************gps.c*************************/									
GPS_INFO GPS; //gps��Ϣ�ṹ��

/*********************exti.c*************************/	
ais_status reset_flag=off; //��λ��־���ϵ�Ĭ��Ϊon.���ݸñ�־�ж��Ƿ�Ƶƫ����
u16 fre_cnt=0;  //Ƶƫ��������
								
u8 CodeNum[6]; //����
u8 code_cnt=0; //������ȷ����־��code_cnt=6ʱ������ȷ��
ais_status code_flag1=off; //�ж������Ƿ���ȷ��Ĭ�ϲ���ȷ��
ais_status code_flag2=off; //�Ƿ�д�������־��Ĭ��Ϊδд����

u16 activation_flag=0;  //�����־��Ĭ��Ϊ0
u32 ChipUniqueID_Flash[3];   //оƬ96bit UID

/*********************adc.c*************************/
ais_status charging_flag=off; //�ж��Ƿ��ڳ��״̬��־��Ĭ��off,���ڳ��״̬

//u8 rxbuf1[4] = {0x06,0x9F,0x6B,0xC7}; //temp////////////////////

u8 gps_led_on = 0; //�����̵�����

u8 battery; //�����ȼ�
static u8 IsCharge_num = 0; 
void Delay(__IO u32 nCount)
{
  for(; nCount != 0; nCount--);
}

// Cortex System Control register address
#define SCB_SysCtrl              ((uint32_t)0xE000ED10)
// SLEEPDEEP bit mask
#define SysCtrl_SLEEPDEEP_Set    ((uint32_t)0x00000004)

extern u8 swchflag;

u8 openflag; //����

int main(void)
{
	SYS_CLK_UP();  //����ϵͳʱ��Ϊ48M
	
	RCC->APB2ENR|=1<<2;     //��ʹ������IO PORTAʱ��
	RCC->APB2ENR|=1<<0;     //��������ʱ��
	GPIOA->CRL&=0XFFFFFFF0;//PA0���ó�����
	GPIOA->CRL|=0X00000008;
	
 stm32_init();  //GPIO
	USART1_Config(); //��д��ӿ�
	USART2_Config(); //GPS����
	//USART3_Config(); //24LE1ͨ�Žӿ�
	TIM4_Configuration();
	Adc_Init();
	Delay(0xff);

 RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);  //for �͹���ģʽ �ؿ�
 PWR_BackupAccessCmd(ENABLE);
	
	BackupRegisterInit();
	Read_GPS_Info();
	
	ReadFlashInit(); //��ȡflash����

 Is_Coded();    //�ж��Ƿ�д�������

	PA_OFF();
	GPS_OFF();
	PLL_OFF();
	LED_OFF();
	
	
 SKY72310_Init();
	
	while (1)
  {	
			 ProgramSelector();  //���뿪��
				if(charging_flag==off)   //ֻ���ڷǳ��״̬�²ŷ���AIS��Ϣ
				{
						send_on();
				}
				else //������״̬
				{
						if (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)==0x00) //�˳�����ж�
						{
										LED_RED_OFF();
										LED_OFF();
										charging_flag = off; 
										task_flag2 = on;
										interval_num = interval_s;
						}
				}
  }
}



