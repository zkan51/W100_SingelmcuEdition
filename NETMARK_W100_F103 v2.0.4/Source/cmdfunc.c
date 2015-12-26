#include "main.h"

/************************************************************
 * ��������Get_ChipID
 * ����  ����ȡоƬID,��flash�д洢�����ݱȽ��ж��Ƿ�һ��
 * ����  ����
 * ���  ��chip_passed
 ************************************************************/
u8 Get_ChipID(void)
{
	u8 i=0;
	u8 chip_passed=0;
	u32 ChipUniqueID[3];   //оƬ96bit UID
	ChipUniqueID[0] = *(__IO u32 *)(0X1FFFF7F0); // ���ֽ�
	ChipUniqueID[1] = *(__IO u32 *)(0X1FFFF7EC); // 
	ChipUniqueID[2] = *(__IO u32 *)(0X1FFFF7E8); // ���ֽ�
	for(i=0;i<=2;i++)
	{
		if(ChipUniqueID[i]==ChipUniqueID_Flash[i])
			chip_passed++;
	}
	return chip_passed;
}
/********************************************************
 * ��������Is_Coded
 * ����  ���ж��豸�Ƿ����
 * ����  ��
 * ���  ��
*********************************************************/
void Is_Coded(void)
{
	u8 i;
	u8 code_cmp1=0;  //�����ж��Ƿ�д�������
	u8 code_cmp2=0;
	/////////////�ж��Ƿ�д�������//////////////////////
	for(i=0;i<6;i++)
	{
		if(CodeNum[i]==0xff)
			code_cmp1++;
	}
	for(i=0;i<6;i++)
	{
		if(CodeNum[i]==0x00)
			code_cmp2++;
	}
	if(   (code_cmp1==6) || (code_cmp2==6)  )
	{
		code_flag1=off;
		code_flag2=off;
	}
	else
	{
		code_flag1=off;
		code_flag2=on;
	}
}
