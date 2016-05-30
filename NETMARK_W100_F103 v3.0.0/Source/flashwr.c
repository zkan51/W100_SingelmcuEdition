#include "main.h"

typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

//BKP_DR2 ����openflag,(ԭΪo_time)

//StartAddr[3] 0x0800B000 --- cogesl

//Ҫд��Flash�����ݵ��׵�ַ--FLASH��ʼ��ַ
u32 StartAddr[13]={0x08009800,0x0800A000,0x0800A800,0x0800B000,0x0800B800,
									 0x0800C000,0x0800C800,0x0800D000,0x0800D800,0x0800E000,
									 0x0800E800,0x0800F000,0x0800F800};
//Ҫд��Flash�����ݵ�ĩ��ַ--FLASH������ַ
u32 EndAddr[13] = {0x08009BFF,0x0800A3FF,0x0800ABFF,0x0800B3FF,0x0800BBFF,
									 0x0800C3FF,0x0800CBFF,0x0800D300,0x0800DB00,0x0800E300,
									 0x0800EB00,0x0800F300,0x0800FB00};

u32 FlashAddress=0x00;//Flash���ڲ���ַ	 
vu32 NbrOfPage = 0x00; //Ҫ������ҳ������
volatile FLASH_Status FLASHStatus;
volatile TestStatus MemoryProgramStatus;

/***********************************************************
 * ��������WriteflashBoatnum
 * ����  ���Ѵ�����Ϣ��CUPд��Flash
 * ����  : ��
 * ���  : ��
 ***********************************************************/
void WriteflashBoatnum(void) 
{
	u8 i;
	FLASHStatus = FLASH_COMPLETE;
	MemoryProgramStatus = PASSED;
	FLASH_Unlock();	//FLASH����
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//���־λ
	
	NbrOfPage = (EndAddr[0] - StartAddr[0]) / FLASH_PAGE_SIZE;//ҳ������ӳ���
	FLASHStatus = FLASH_ErasePage(StartAddr[0] + (FLASH_PAGE_SIZE * NbrOfPage));

	FlashAddress = StartAddr[0];
	for(i=0;i<4;i++)
	{
		FLASHStatus = FLASH_ProgramWord(FlashAddress, ((boatnum[i*4+3]<<24) + (boatnum[i*4+2]<<16) + (boatnum[i*4+1]<<8) + boatnum[i*4]));
		FlashAddress = FlashAddress + 4;
	}
}
/***********************************************************
 * ��������ReadflashBoatnum
 * ����  ���Ѵ�����Ϣ��Flash����CPU
 * ����  : ��
 * ���  : ��
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
 * ��������WriteflashMMSI
 * ����  ����MMSI�Ŵ�CPUд��Flash
 * ����  : ��
 * ���  : ��
 ***********************************************************/
void WriteflashMMSI(void) 
{
	FLASHStatus = FLASH_COMPLETE;
	MemoryProgramStatus = PASSED; 

	FLASH_Unlock();	//FLASH����
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//���־λ

	NbrOfPage = (EndAddr[1] - StartAddr[1]) / FLASH_PAGE_SIZE;//ҳ������ӳ���
	FLASHStatus = FLASH_ErasePage(StartAddr[1] + (FLASH_PAGE_SIZE * NbrOfPage)); 

	FlashAddress = StartAddr[1]; 
	FLASHStatus = FLASH_ProgramWord(FlashAddress, MMSI);
}
/***********************************************************
 * ��������ReadflashMMSI
 * ����  ����MMSI�Ŵ�Flash����CPU
 * ����  : ��
 * ���  : ��
 ***********************************************************/
void  ReadflashMMSI(void)
{
	MMSI =*(unsigned long*)StartAddr[1];
}
/***********************************************************
 * ��������WriteflashCode
 * ����  ����CodeNum��CPUд��Flash
 * ����  : ��
 * ���  : ��
 ***********************************************************/
void WriteflashCode(void) //д����
{
	FLASHStatus = FLASH_COMPLETE;
	MemoryProgramStatus = PASSED;

	FLASH_Unlock();	//FLASH����
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//���־λ

	NbrOfPage = (EndAddr[2] - StartAddr[2]) / FLASH_PAGE_SIZE;//ҳ������ӳ���
	FLASHStatus = FLASH_ErasePage(StartAddr[2] + (FLASH_PAGE_SIZE * NbrOfPage)); 
	FlashAddress = StartAddr[2];

	FLASHStatus = FLASH_ProgramWord(FlashAddress, ((CodeNum[3]<<24) + (CodeNum[2]<<16) + (CodeNum[1]<<8) + CodeNum[0]));
	FlashAddress = FlashAddress + 4;
	FLASHStatus = FLASH_ProgramWord(FlashAddress, ((CodeNum[5]<<8) + CodeNum[4]));
}
/***********************************************************
 * ��������ReadflashCode
 * ����  ����CodeNum��Flash����CPU
 * ����  : ��
 * ���  : ��
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
 * ��������WriteflashMPeriod
 * ����  ����interval_s��CPUд��Flash
 * ����  : ��
 * ���  : ��
 ***********************************************************/
void WriteflashMPeriod(void)
{
	FLASHStatus = FLASH_COMPLETE;
	MemoryProgramStatus = PASSED; 

	FLASH_Unlock();	//FLASH����
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//���־λ
	NbrOfPage = (EndAddr[4] - StartAddr[4]) / FLASH_PAGE_SIZE;//ҳ������ӳ���
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
 * ��������ReadflashMPeriod
 * ����  ����msequence_poly��Flash����CPU
 * ����  : ��
 * ���  : ��
 ***********************************************************/
void  ReadflashMPeriod(void) //��¼�����������̶�ֵ
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
 * ��������WriteflashMUID
 * ����  ���Ѵ�ChipUniqueID_Flashд��Flash,���ڳ��򼤻�
 * ����  : ��
 * ���  : ��
 ***********************************************************/
void WriteflashMUID(void)
{
	u8 i;
	FLASHStatus = FLASH_COMPLETE;
	MemoryProgramStatus = PASSED; 

	FLASH_Unlock();	//FLASH����
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//���־λ

	NbrOfPage = (EndAddr[5] - StartAddr[5]) / FLASH_PAGE_SIZE;//ҳ������ӳ���
	FLASHStatus = FLASH_ErasePage(StartAddr[5] + (FLASH_PAGE_SIZE * NbrOfPage)); 
	FlashAddress = StartAddr[5]; 
	for(i=0;i<=2;i++)
	{
		FLASHStatus = FLASH_ProgramWord(FlashAddress,ChipUniqueID_Flash[i]);  //��UIDд��flash
		FlashAddress += 4;
	}	
}
/***********************************************************
 * ��������ReadflashMUID
 * ����  ����ChipUniqueID_Flash��Flash����CPU
 * ����  : ��
 * ���  : ��
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
 * ��������WriteflashMActivation
 * ����  ����activation_flag��CPUд��Flash,���ڳ��򼤻�
 * ����  : ��
 * ���  : ��
 ***********************************************************/
void WriteflashMActivation(void) 
{
	FLASHStatus = FLASH_COMPLETE;
	MemoryProgramStatus = PASSED; 

	FLASH_Unlock();	//FLASH����
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//���־λ
	
	NbrOfPage = (EndAddr[6] - StartAddr[6]) / FLASH_PAGE_SIZE;//ҳ������ӳ���
	FLASHStatus = FLASH_ErasePage(StartAddr[6] + (FLASH_PAGE_SIZE * NbrOfPage));
	FlashAddress = StartAddr[6];
	FLASHStatus = FLASH_ProgramHalfWord(FlashAddress, activation_flag);
}
/***********************************************************
 * ��������ReadflashMActivation
 * ����  ����activation_flag��Flash����CPU
 * ����  : ��
 * ���  : ��
 ***********************************************************/
void  ReadflashMActivation(void)
{
	activation_flag = *(u16*)StartAddr[6];
}
/***********************************************************
 * ��������WriteflashBoatInfo
 * ����  ����activation_flag��CPUд��Flash,λ����Ϣ
 * ����  : ��
 * ���  : ��
 ***********************************************************/
void WriteflashBoatInfo(void) 
{
	FLASHStatus = FLASH_COMPLETE;
	MemoryProgramStatus = PASSED; 
	FLASH_Unlock();	//FLASH����
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//���־λ
	
	NbrOfPage = (EndAddr[7] - StartAddr[7]) / FLASH_PAGE_SIZE; //ҳ������ӳ���
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
 * ��������Write_GPS_Info
 * ����  ����GPS��Ϣ��CPUд��Flash
 * ����  : ��
 * ���  : ��
 ***********************************************************/
void Write_Flash_GPS_Info(void)
{
	FLASHStatus = FLASH_COMPLETE;
	MemoryProgramStatus = PASSED;
	FLASH_Unlock();	//FLASH����
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//���־λ
	
	NbrOfPage = (EndAddr[10] - StartAddr[10]) / FLASH_PAGE_SIZE; //ҳ������ӳ���
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
// 	FLASH_Unlock();	//FLASH����
// 	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//���־λ
// 	
// 	NbrOfPage = (EndAddr[10] - StartAddr[10]) / FLASH_PAGE_SIZE; //ҳ������ӳ���
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
 * ��������Read_GPS_Info
 * ����  ����λ����Ϣ��Flash����CPU
 * ����  : ��
 * ���  : ��
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
 * ��������Write_GPS_Info
 * ����  ����flag_channel��Ϣ��CPUд��Flash
 * ����  : ��
 * ���  : ��
 ***********************************************************/
void Write_TX_Channel(void)
{
// 	FLASHStatus = FLASH_COMPLETE;
// 	MemoryProgramStatus = PASSED; 
// 	FLASH_Unlock();	//FLASH����
// 	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//���־λ
// 	
// 	NbrOfPage = (EndAddr[11] - StartAddr[11]) / FLASH_PAGE_SIZE; //ҳ������ӳ���
// 	FLASHStatus = FLASH_ErasePage(StartAddr[11] + (FLASH_PAGE_SIZE * NbrOfPage));

// 	FlashAddress = StartAddr[11];

// 	FLASHStatus = FLASH_ProgramWord(FlashAddress, flag_channel);
	
	BKP_WriteBackupRegister(BKP_DR6,flag_channel);
}
/***********************************************************
 * ��������Read_TX_Channel
 * ����  ����λ����Ϣ��flag_channel����CPU
 * ����  : ��
 * ���  : ��
 ***********************************************************/
void Read_TX_Channel(void)
{	
// 	FlashAddress = StartAddr[11]; 
// 	
// 	flag_channel = *(unsigned char*)FlashAddress; 
	
	flag_channel = BKP_ReadBackupRegister(BKP_DR6);
}

void Write_Flash_Cogsel(void) //д��������ѡ��
{
	FLASHStatus = FLASH_COMPLETE;
	MemoryProgramStatus = PASSED; 
	FLASH_Unlock();	//FLASH����
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//���־λ
	
	NbrOfPage = (EndAddr[3] - StartAddr[3]) / FLASH_PAGE_SIZE; //ҳ������ӳ���
	FLASHStatus = FLASH_ErasePage(StartAddr[3] + (FLASH_PAGE_SIZE * NbrOfPage));

	FlashAddress = StartAddr[3];

	FLASHStatus = FLASH_ProgramWord(FlashAddress, cogsel);
}

void Read_Flash_Cogsel(void) //����������ѡ��
{	
	FlashAddress = StartAddr[3]; 
	
	cogsel = *(unsigned char*)FlashAddress; 
}

void Read_OpenOnce(void) //������һ�α�� ��ȡ
{	
	openflag = BKP_ReadBackupRegister(BKP_DR2);
	if(GetBatteryPower() >= BATTERYLEVEL) //���ǵ͵���ʱ�������̵�
	{
		if(openflag != 1) //��һ�ο�����δ��ֵ
		{
			TIM4_Configuration();
			TIM4_ON(); //�̵ƿ�������
		}
		else //���Ѻ�
		{
			openflag = 1;
			BKP_WriteBackupRegister(BKP_DR2,openflag);
		}
	}
}

/***********************************************************
 * ��������ReadflashInit
 * ����  ����Flash��ȡ������MMSI�����롢�����롢UID������ʱ������CPU
 * ����  : ��
 * ���  : ��
 ***********************************************************/
void ReadFlashInit(void) 
{
	u8 i;
	for(i=0;i<16;i++)//���봬��
	{
		boatnum[i] = *(unsigned char*)(StartAddr[0]+i);
	}

	MMSI = *(unsigned long*)StartAddr[1];//����MMSI

	for(i=0;i<6;i++)//��������
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
	
//	interval_s=*(u16*)StartAddr[4]; //���뷢��ʱ����interval_s
// 	USART_SendData(USART1, interval_s>>8);
// 	while (!(USART1->SR & USART_FLAG_TXE));
// 	USART_SendData(USART1, interval_s);
// 	while (!(USART1->SR & USART_FLAG_TXE));

//	ReadflashBoatInfo();
// 	Read_dev_etype();
// 	Read_dev_type();
}

void WriteBackup(void)  //3������ дflash��дbackup
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

void ReadBackup(void)  //3������ ��flash��дbackup ������γ��Ƶ��
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




