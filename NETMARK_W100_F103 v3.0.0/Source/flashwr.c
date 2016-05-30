#include "main.h"

typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

//BKP_DR2 用于openflag,(原为o_time)

//StartAddr[3] 0x0800B000 --- cogesl

//要写入Flash的数据的首地址--FLASH起始地址
u32 StartAddr[13]={0x08009800,0x0800A000,0x0800A800,0x0800B000,0x0800B800,
									 0x0800C000,0x0800C800,0x0800D000,0x0800D800,0x0800E000,
									 0x0800E800,0x0800F000,0x0800F800};
//要写入Flash的数据的末地址--FLASH结束地址
u32 EndAddr[13] = {0x08009BFF,0x0800A3FF,0x0800ABFF,0x0800B3FF,0x0800BBFF,
									 0x0800C3FF,0x0800CBFF,0x0800D300,0x0800DB00,0x0800E300,
									 0x0800EB00,0x0800F300,0x0800FB00};

u32 FlashAddress=0x00;//Flash的内部地址	 
vu32 NbrOfPage = 0x00; //要擦除的页面数量
volatile FLASH_Status FLASHStatus;
volatile TestStatus MemoryProgramStatus;

/***********************************************************
 * 函数名：WriteflashBoatnum
 * 描述  ：把船名信息从CUP写入Flash
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void WriteflashBoatnum(void) 
{
	u8 i;
	FLASHStatus = FLASH_COMPLETE;
	MemoryProgramStatus = PASSED;
	FLASH_Unlock();	//FLASH解锁
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//清标志位
	
	NbrOfPage = (EndAddr[0] - StartAddr[0]) / FLASH_PAGE_SIZE;//页面擦除子程序
	FLASHStatus = FLASH_ErasePage(StartAddr[0] + (FLASH_PAGE_SIZE * NbrOfPage));

	FlashAddress = StartAddr[0];
	for(i=0;i<4;i++)
	{
		FLASHStatus = FLASH_ProgramWord(FlashAddress, ((boatnum[i*4+3]<<24) + (boatnum[i*4+2]<<16) + (boatnum[i*4+1]<<8) + boatnum[i*4]));
		FlashAddress = FlashAddress + 4;
	}
}
/***********************************************************
 * 函数名：ReadflashBoatnum
 * 描述  ：把船名信息从Flash读入CPU
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void  ReadflashBoatnum(void)
{
	u8 i;
	for(i=0;i<16;i++)
	{
		boatnum[i] = *(unsigned char*)(StartAddr[0]+i);
	}
}
/***********************************************************
 * 函数名：WriteflashMMSI
 * 描述  ：把MMSI号从CPU写入Flash
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void WriteflashMMSI(void) 
{
	FLASHStatus = FLASH_COMPLETE;
	MemoryProgramStatus = PASSED; 

	FLASH_Unlock();	//FLASH解锁
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//清标志位

	NbrOfPage = (EndAddr[1] - StartAddr[1]) / FLASH_PAGE_SIZE;//页面擦除子程序
	FLASHStatus = FLASH_ErasePage(StartAddr[1] + (FLASH_PAGE_SIZE * NbrOfPage)); 

	FlashAddress = StartAddr[1]; 
	FLASHStatus = FLASH_ProgramWord(FlashAddress, MMSI);
}
/***********************************************************
 * 函数名：ReadflashMMSI
 * 描述  ：把MMSI号从Flash读入CPU
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void  ReadflashMMSI(void)
{
	MMSI =*(unsigned long*)StartAddr[1];
}
/***********************************************************
 * 函数名：WriteflashCode
 * 描述  ：把CodeNum从CPU写入Flash
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void WriteflashCode(void) //写密码
{
	FLASHStatus = FLASH_COMPLETE;
	MemoryProgramStatus = PASSED;

	FLASH_Unlock();	//FLASH解锁
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//清标志位

	NbrOfPage = (EndAddr[2] - StartAddr[2]) / FLASH_PAGE_SIZE;//页面擦除子程序
	FLASHStatus = FLASH_ErasePage(StartAddr[2] + (FLASH_PAGE_SIZE * NbrOfPage)); 
	FlashAddress = StartAddr[2];

	FLASHStatus = FLASH_ProgramWord(FlashAddress, ((CodeNum[3]<<24) + (CodeNum[2]<<16) + (CodeNum[1]<<8) + CodeNum[0]));
	FlashAddress = FlashAddress + 4;
	FLASHStatus = FLASH_ProgramWord(FlashAddress, ((CodeNum[5]<<8) + CodeNum[4]));
}
/***********************************************************
 * 函数名：ReadflashCode
 * 描述  ：把CodeNum从Flash读入CPU
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void  ReadflashCode(void)
{
	u8 i;
	for(i=0;i<6;i++)
	{
		CodeNum[i] = *(unsigned char*)(StartAddr[2]+i);
	}
}

/***********************************************************
 * 函数名：WriteflashMPeriod
 * 描述  ：把interval_s从CPU写入Flash
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void WriteflashMPeriod(void)
{
	FLASHStatus = FLASH_COMPLETE;
	MemoryProgramStatus = PASSED; 

	FLASH_Unlock();	//FLASH解锁
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//清标志位
	NbrOfPage = (EndAddr[4] - StartAddr[4]) / FLASH_PAGE_SIZE;//页面擦除子程序
	FLASHStatus = FLASH_ErasePage(StartAddr[4] + (FLASH_PAGE_SIZE * NbrOfPage)); 
	
	FlashAddress = StartAddr[4]; 
	FLASHStatus = FLASH_ProgramWord(FlashAddress, gps_invalid);
	
	FlashAddress = FlashAddress + 4;
	FLASHStatus = FLASH_ProgramWord(FlashAddress, intervalA);
	
	FlashAddress = FlashAddress + 4;
	FLASHStatus = FLASH_ProgramWord(FlashAddress, intervalB);
	
	FlashAddress = FlashAddress + 4;
	FLASHStatus = FLASH_ProgramWord(FlashAddress, interval_num);
	
	FlashAddress = FlashAddress + 4;
//	FLASHStatus = FLASH_ProgramWord(FlashAddress, time_o);
	
	FlashAddress = FlashAddress + 4;
	FLASHStatus = FLASH_ProgramHalfWord(FlashAddress, interval_s);

	BKP_WriteBackupRegister(BKP_DR1,interval_num);
	//BKP_WriteBackupRegister(BKP_DR2,time_o);
	BKP_WriteBackupRegister(BKP_DR3,interval_s);
}
/***********************************************************
 * 函数名：ReadflashMPeriod
 * 描述  ：把msequence_poly从Flash读入CPU
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void  ReadflashMPeriod(void) //记录发射间隔次数固定值
{
	FlashAddress = StartAddr[4]; 
		
	gps_invalid = *(u16*)FlashAddress;
	
	FlashAddress += 4;
	intervalA = *(u16*)FlashAddress;
	
	FlashAddress += 4;
	intervalB = *(u16*)FlashAddress;
	
	FlashAddress += 4;
	interval_num = *(u16*)FlashAddress;
	
	FlashAddress += 4;
	//time_o = *(u16*)FlashAddress;
	
	FlashAddress += 4;
	interval_s = *(u16*)FlashAddress;

	BKP_WriteBackupRegister(BKP_DR1,interval_num);
//	BKP_WriteBackupRegister(BKP_DR2,time_o);
	BKP_WriteBackupRegister(BKP_DR3,interval_s);
	
// 	interval_num = BKP_ReadBackupRegister(BKP_DR1);
// 	gps_invalid_num = BKP_ReadBackupRegister(BKP_DR2);
// 	interval_s = BKP_ReadBackupRegister(BKP_DR3);
}
/***********************************************************
 * 函数名：WriteflashMUID
 * 描述  ：把从ChipUniqueID_Flash写入Flash,用于程序激活
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void WriteflashMUID(void)
{
	u8 i;
	FLASHStatus = FLASH_COMPLETE;
	MemoryProgramStatus = PASSED; 

	FLASH_Unlock();	//FLASH解锁
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//清标志位

	NbrOfPage = (EndAddr[5] - StartAddr[5]) / FLASH_PAGE_SIZE;//页面擦除子程序
	FLASHStatus = FLASH_ErasePage(StartAddr[5] + (FLASH_PAGE_SIZE * NbrOfPage)); 
	FlashAddress = StartAddr[5]; 
	for(i=0;i<=2;i++)
	{
		FLASHStatus = FLASH_ProgramWord(FlashAddress,ChipUniqueID_Flash[i]);  //将UID写入flash
		FlashAddress += 4;
	}	
}
/***********************************************************
 * 函数名：ReadflashMUID
 * 描述  ：把ChipUniqueID_Flash从Flash读入CPU
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void  ReadflashMUID(void) 
{
	u8 i;
	FlashAddress = StartAddr[5]; 
	for(i=0;i<=2;i++)
	{
		ChipUniqueID_Flash[i] = *(u32*)FlashAddress; 
		FlashAddress += 4;
	}
	
}
/***********************************************************
 * 函数名：WriteflashMActivation
 * 描述  ：把activation_flag从CPU写入Flash,用于程序激活
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void WriteflashMActivation(void) 
{
	FLASHStatus = FLASH_COMPLETE;
	MemoryProgramStatus = PASSED; 

	FLASH_Unlock();	//FLASH解锁
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//清标志位
	
	NbrOfPage = (EndAddr[6] - StartAddr[6]) / FLASH_PAGE_SIZE;//页面擦除子程序
	FLASHStatus = FLASH_ErasePage(StartAddr[6] + (FLASH_PAGE_SIZE * NbrOfPage));
	FlashAddress = StartAddr[6];
	FLASHStatus = FLASH_ProgramHalfWord(FlashAddress, activation_flag);
}
/***********************************************************
 * 函数名：ReadflashMActivation
 * 描述  ：把activation_flag从Flash读入CPU
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void  ReadflashMActivation(void)
{
	activation_flag = *(u16*)StartAddr[6];
}
/***********************************************************
 * 函数名：WriteflashBoatInfo
 * 描述  ：把activation_flag从CPU写入Flash,位置信息
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void WriteflashBoatInfo(void) 
{
	FLASHStatus = FLASH_COMPLETE;
	MemoryProgramStatus = PASSED; 
	FLASH_Unlock();	//FLASH解锁
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//清标志位
	
	NbrOfPage = (EndAddr[7] - StartAddr[7]) / FLASH_PAGE_SIZE; //页面擦除子程序
	FLASHStatus = FLASH_ErasePage(StartAddr[7] + (FLASH_PAGE_SIZE * NbrOfPage)); 

	FlashAddress = StartAddr[7]; 

	FLASHStatus = FLASH_ProgramWord(FlashAddress, boatlong);
	FlashAddress = FlashAddress + 4;
	FLASHStatus = FLASH_ProgramWord(FlashAddress, boatwidth);
}

void ReadflashBoatInfo(void)
{
	FlashAddress = StartAddr[7]; 
	boatlong = *(u16*)StartAddr[7];
	FlashAddress += 4;
	boatwidth = *(u8*)FlashAddress;
	
	if ((boatlong %2))
	{
		boatsize_a = boatlong/2;
		boatsize_b = boatlong/2+1;
	}
	else
	{
		boatsize_a = boatlong/2;
		boatsize_b = boatlong/2;
	}
	if (boatwidth %2)
	{
		boatsize_c = boatwidth/2;
		boatsize_d = boatwidth/2+1;
	}
	else 
	{
		boatsize_c = boatwidth/2;
		boatsize_d = boatwidth/2;
	}
}
/***********************************************************
 * 函数名：Write_GPS_Info
 * 描述  ：把GPS信息从CPU写入Flash
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void Write_Flash_GPS_Info(void)
{
	FLASHStatus = FLASH_COMPLETE;
	MemoryProgramStatus = PASSED;
	FLASH_Unlock();	//FLASH解锁
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//清标志位
	
	NbrOfPage = (EndAddr[10] - StartAddr[10]) / FLASH_PAGE_SIZE; //页面擦除子程序
	FLASHStatus = FLASH_ErasePage(StartAddr[10] + (FLASH_PAGE_SIZE * NbrOfPage));

	FlashAddress = StartAddr[10];

	FLASHStatus = FLASH_ProgramWord(FlashAddress, jingdu);
	FlashAddress = FlashAddress + 4;
	FLASHStatus = FLASH_ProgramWord(FlashAddress, weidu);
	FlashAddress = FlashAddress + 4;
	FLASHStatus = FLASH_ProgramWord(FlashAddress, sog);
	FlashAddress = FlashAddress + 4;
	FLASHStatus = FLASH_ProgramWord(FlashAddress, direction);
	
	BKP_WriteBackupRegister(BKP_DR4,(u16)(jingdu >> 16));
	BKP_WriteBackupRegister(BKP_DR5,(u16)(weidu >> 16));
	BKP_WriteBackupRegister(BKP_DR7,(u16)jingdu);
	BKP_WriteBackupRegister(BKP_DR8,(u16)weidu);
	BKP_WriteBackupRegister(BKP_DR9,sog);
	BKP_WriteBackupRegister(BKP_DR10,direction);
}

void Write_GPS_Info(void)
{
// 	FLASHStatus = FLASH_COMPLETE;
// 	MemoryProgramStatus = PASSED; 
// 	FLASH_Unlock();	//FLASH解锁
// 	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//清标志位
// 	
// 	NbrOfPage = (EndAddr[10] - StartAddr[10]) / FLASH_PAGE_SIZE; //页面擦除子程序
// 	FLASHStatus = FLASH_ErasePage(StartAddr[10] + (FLASH_PAGE_SIZE * NbrOfPage)); 

// 	FlashAddress = StartAddr[10]; 

// 	FLASHStatus = FLASH_ProgramWord(FlashAddress, jingdu);
// 	FlashAddress = FlashAddress + 4;
// 	FLASHStatus = FLASH_ProgramWord(FlashAddress, weidu);
	
//	BKP_WriteBackupRegister(BKP_DR2,time_o);
	BKP_WriteBackupRegister(BKP_DR4,(u16)(jingdu >> 16));
	BKP_WriteBackupRegister(BKP_DR5,(u16)(weidu >> 16));
	BKP_WriteBackupRegister(BKP_DR7,(u16)jingdu);
	BKP_WriteBackupRegister(BKP_DR8,(u16)weidu);
	BKP_WriteBackupRegister(BKP_DR9,sog);
	BKP_WriteBackupRegister(BKP_DR10,direction);
}
/***********************************************************
 * 函数名：Read_GPS_Info
 * 描述  ：把位置信息从Flash读入CPU
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void Read_Flash_GPS_Info(void)
{	
	FlashAddress = StartAddr[10];
	
	jingdu = *(u32*)FlashAddress;
	FlashAddress += 4;
	weidu = *(u32*)FlashAddress;
	FlashAddress += 4;
	sog = *(u16*)FlashAddress;
	FlashAddress += 4;
	direction = *(u16*)FlashAddress;
	
	BKP_WriteBackupRegister(BKP_DR4,(u16)(jingdu >> 16));
	BKP_WriteBackupRegister(BKP_DR5,(u16)(weidu >> 16));
	BKP_WriteBackupRegister(BKP_DR7,(u16)jingdu);
	BKP_WriteBackupRegister(BKP_DR8,(u16)weidu);
	BKP_WriteBackupRegister(BKP_DR9,sog);
	BKP_WriteBackupRegister(BKP_DR10,direction);
}

void Read_GPS_Info(void)
{	
// 	FlashAddress = StartAddr[10]; 
// 	
// 	jingdu = *(u32*)FlashAddress;
// 	FlashAddress += 4;	
// 	weidu = *(u32*)FlashAddress;
	
	jingdu = 0; weidu = 0; sog = 0; direction = 0;
	
	jingdu = BKP_ReadBackupRegister(BKP_DR4);  
	jingdu = (jingdu << 16);
	jingdu = jingdu + BKP_ReadBackupRegister(BKP_DR7);  

	weidu = BKP_ReadBackupRegister(BKP_DR5);  
	weidu = (weidu << 16);
	weidu = weidu + BKP_ReadBackupRegister(BKP_DR8);	
	
	sog = BKP_ReadBackupRegister(BKP_DR9);
	
	direction = BKP_ReadBackupRegister(BKP_DR10);
}
/***********************************************************
 * 函数名：Write_GPS_Info
 * 描述  ：把flag_channel信息从CPU写入Flash
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void Write_TX_Channel(void)
{
// 	FLASHStatus = FLASH_COMPLETE;
// 	MemoryProgramStatus = PASSED; 
// 	FLASH_Unlock();	//FLASH解锁
// 	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//清标志位
// 	
// 	NbrOfPage = (EndAddr[11] - StartAddr[11]) / FLASH_PAGE_SIZE; //页面擦除子程序
// 	FLASHStatus = FLASH_ErasePage(StartAddr[11] + (FLASH_PAGE_SIZE * NbrOfPage));

// 	FlashAddress = StartAddr[11];

// 	FLASHStatus = FLASH_ProgramWord(FlashAddress, flag_channel);
	
	BKP_WriteBackupRegister(BKP_DR6,flag_channel);
}
/***********************************************************
 * 函数名：Read_TX_Channel
 * 描述  ：把位置信息从flag_channel读入CPU
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void Read_TX_Channel(void)
{	
// 	FlashAddress = StartAddr[11]; 
// 	
// 	flag_channel = *(unsigned char*)FlashAddress; 
	
	flag_channel = BKP_ReadBackupRegister(BKP_DR6);
}

void Write_Flash_Cogsel(void) //写流网张网选择
{
	FLASHStatus = FLASH_COMPLETE;
	MemoryProgramStatus = PASSED; 
	FLASH_Unlock();	//FLASH解锁
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//清标志位
	
	NbrOfPage = (EndAddr[3] - StartAddr[3]) / FLASH_PAGE_SIZE; //页面擦除子程序
	FLASHStatus = FLASH_ErasePage(StartAddr[3] + (FLASH_PAGE_SIZE * NbrOfPage));

	FlashAddress = StartAddr[3];

	FLASHStatus = FLASH_ProgramWord(FlashAddress, cogsel);
}

void Read_Flash_Cogsel(void) //读流网张网选择
{	
	FlashAddress = StartAddr[3]; 
	
	cogsel = *(unsigned char*)FlashAddress; 
}

void Read_OpenOnce(void) //开机第一次标记 读取
{	
	openflag = BKP_ReadBackupRegister(BKP_DR2);
	if(GetBatteryPower() >= BATTERYLEVEL) //当非低电量时，才闪绿灯
	{
		if(openflag != 1) //第一次开机，未赋值
		{
			TIM4_Configuration();
			TIM4_ON(); //绿灯开机慢闪
		}
		else //唤醒后
		{
			openflag = 1;
			BKP_WriteBackupRegister(BKP_DR2,openflag);
		}
	}
}

/***********************************************************
 * 函数名：ReadflashInit
 * 描述  ：从Flash读取船名、MMSI、密码、激活码、UID、发射时间间隔到CPU
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void ReadFlashInit(void) 
{
	u8 i;
	for(i=0;i<16;i++)//读入船名
	{
		boatnum[i] = *(unsigned char*)(StartAddr[0]+i);
	}

	MMSI = *(unsigned long*)StartAddr[1];//读入MMSI

	for(i=0;i<6;i++)//读入密码
	{
		CodeNum[i] = *(unsigned char*)(StartAddr[2]+i);
	}
	
	Read_Flash_Cogsel(); //cogsel =*(u8*)StartAddr[3];
	ReadflashBoatInfo();
	
	//ReadBackup(); //ReadflashMPeriod();
	FlashAddress = StartAddr[4];
	gps_invalid = *(u16*)FlashAddress;
	FlashAddress += 4;
	intervalA = *(u16*)FlashAddress;
	FlashAddress += 4;
	intervalB = *(u16*)FlashAddress;
	interval_num = BKP_ReadBackupRegister(BKP_DR1);
//	time_o = BKP_ReadBackupRegister(BKP_DR2);
	interval_s = BKP_ReadBackupRegister(BKP_DR3);
	Read_GPS_Info();
	flag_channel = BKP_ReadBackupRegister(BKP_DR6);
	
//	interval_s=*(u16*)StartAddr[4]; //读入发射时间间隔interval_s
// 	USART_SendData(USART1, interval_s>>8);
// 	while (!(USART1->SR & USART_FLAG_TXE));
// 	USART_SendData(USART1, interval_s);
// 	while (!(USART1->SR & USART_FLAG_TXE));

//	ReadflashBoatInfo();
// 	Read_dev_etype();
// 	Read_dev_type();
}

void WriteBackup(void)  //3个变量 写flash，写backup
{
	FlashAddress = FlashAddress + 4;
	FLASHStatus = FLASH_ProgramWord(FlashAddress, interval_num);
	
	FlashAddress = FlashAddress + 4;
//	FLASHStatus = FLASH_ProgramWord(FlashAddress, time_o);
	
	FlashAddress = FlashAddress + 4;
	FLASHStatus = FLASH_ProgramHalfWord(FlashAddress, interval_s);
	
	
	BKP_WriteBackupRegister(BKP_DR1,interval_num);
//	BKP_WriteBackupRegister(BKP_DR2,time_o);
	BKP_WriteBackupRegister(BKP_DR3,interval_s);
}

void ReadBackup(void)  //3个变量 读flash，写backup ，读经纬度频道
{
	FlashAddress = StartAddr[4];
		
	gps_invalid = *(u16*)FlashAddress;
	
	FlashAddress += 4;
	intervalA = *(u16*)FlashAddress;
	
	FlashAddress += 4;
	intervalB = *(u16*)FlashAddress;
	
	FlashAddress += 4;
	interval_num = *(u16*)FlashAddress;
	
	FlashAddress += 4;
	//time_o = *(u16*)FlashAddress;
	
	FlashAddress += 4;
	interval_s = *(u16*)FlashAddress;

	BKP_WriteBackupRegister(BKP_DR1,interval_num);
	//BKP_WriteBackupRegister(BKP_DR2,time_o);
	BKP_WriteBackupRegister(BKP_DR3,interval_s);

	Read_GPS_Info();
	
	flag_channel = BKP_ReadBackupRegister(BKP_DR6);
	
}




