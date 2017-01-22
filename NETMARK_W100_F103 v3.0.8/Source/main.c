#include "main.h"
#include "stm32f10x_adc.h"
/*********************mkmsg.c*************************/
u8 msg[45]={0};//存储AIS消息
u8 len; //输入消息的字节数
unsigned long MMSI=412440046; //九位码
u8 boatnum[16]={0x60,0x34,0x93,0x8c,0x24,0x13,0x99,0x57,0x0d,0x73,0xcc,0x0c,0x31,0x00,0x00,0x00}; //船名 MINLIANYU0533@01
u8 Vender_ID[6]= {0x21,0x31,0x2d,0x38,0xd2,0xc0}; //制造商号,消息24B使用 HSD-NMK
u8 Call_sign[6]={0x00,0x00,0x00,0x00,0x00,0x00}; //呼号，消息24B使用
u16 boatlong; //船长
u8 boatwidth; //船宽
u16 boatsize_a ; //距船首 
u16 boatsize_b ; //距船尾 a+b = 船长
u8 boatsize_c ;	//距左舷
u8 boatsize_d ;	//距右舷 c+d = 船宽
unsigned long jingdu=0; //GPS经度
unsigned long weidu=0; //GPS纬度
unsigned int sog=0; //msg18 SOG
unsigned int direction=0; //msg18 实际航向
unsigned long gpstime=0; //gps时间
//////msg21/////
u8 dev_type = 4;//1;//助航设备类型 5bit
u8 dev_name[54]={0x48,0x05,0x04,0x0C,0x17,0x1C,0xB3,0x04,0x20,0xC4}; //APPLE123ABCD
u8 dev_local[5]={0x0C,0x21,0x24,0x0A,0x0A}; //尺寸位置参考点 30bit；共5个字节,只用低6位 ;6 6 6 6 6 6
u8 dev_time; //时戳 6bit
u8 dev_state; //AtoN状态 8bit
u8 dev_etype = 1; //电子定位装置类型 4bit 默认为GPS

////////////////
/*********************msg_slot.c*************************/
u8 msg18_add[256]={0};//存储消息18对应频率字查找表的地址，每一个元素取值范围在0~16。
u8 msg24_A[256]={0};//存储消息24A
u8 msg24_B[256]={0};//存储消息24B
u8 msg21_add[512]={0};//存储消息21对应频率字查找表的地址，每一个元素取值范围在0~16。
u8 msg18_24AB_add[768];//存储消息18和24A/B对应频率字查找表的地址，每一个元素取值范围在0~16。
//u16 msequence_poly=0x8805;	//16位PN码生成多项式，用来产生发射间隔的一个随机数，上位机无线写入。
u16 slot_increacement=0 ; //随机时隙增量，与发射间隔参数interval_s相加，随机化发射时间间隔。
//u16 interval_s=1125; //375;      //2250; //初始发射间隔，1min.   //6750   
u16 interval_s;  //间隔计数变化值 
u16 interval_num ; //发射间隔计数，变化值
u16 gps_invalid = 0x78; //gps失效的次数时间2分钟
//u16 gps_invalid_num; //失效次数值
//u16 time_o;//GPS前次时间存储
u16 intervalA;  //间隔计数次数 固定值
u16 intervalB;  //间隔计数次数 超时值
u8 cogsel=0; //流网或张网选择变量 默认0=流网
u8 txBuf[64]={0}; //发送AIS消息数组

//union dds_union dds_word_update[16][5]={0x00000000};  //矫正更新后的频率字查找表

ais_status sendTask=on;//发射任务开启标志
u8 flag_channel=0;		 //信道选择标志位,初始化为信道1
u8 flag_charging = 0; //充电标志位
/*********************gps.c*************************/									
GPS_INFO GPS; //gps信息结构体

/*********************exti.c*************************/	
u8 CodeNum[6]; //密码
u8 code_cnt=0; //密码正确与否标志。code_cnt=6时密码正确。
ais_status encryption=off; //是否写过密码标志，默认为未写过。
u16 activation_flag=0;  //激活标志，默认为0
u32 ChipUniqueID_Flash[3];   //芯片96bit UID

/*********************adc.c*************************/
u8 battery; //电量等级
void Delay(__IO u32 nCount)
{
  for(; nCount != 0; nCount--);
}

//经销商信息
unsigned long Time;
u8 AgencyName[16]={0};
u8 openflag; //开机

int main(void)
{
	SYS_CLK_DOWN();  //设置系统时钟为8M
	
	RCC->APB2ENR|=1<<2;     //先使能外设IO PORTA时钟
	RCC->APB2ENR|=1<<0;     //开启辅助时钟
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //中断优先级分组2
	delay_init();		        //实现精确延时函数
	GPIO_Configuration();  //GPIO

	USART1_Config();       //读写码接口
	USART2_Config();       //GPS接收
	TIM2_Configuration();  //GPS开启时间
	TIM4_Configuration();	 //开GPS时闪灯
	Adc_Init();
	BackupRegisterInit();
	Read_GPS_Info();
	ReadFlashInit();       //读取flash数据
	Exti_Init();
	// si4463初始化
	SPI1IO_Init();			      // IO模拟SPI
	SI4463_IOSET();	
	
	
	//判断是否为充电状态
	if(isCharging)
	{
		flag_charging  = 1;
	}
	
	ProgramSelector(); //拨码开关
	
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



