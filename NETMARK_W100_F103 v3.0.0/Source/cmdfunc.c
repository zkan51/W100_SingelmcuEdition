#include "main.h"

/************************************************************
 * 函数名：Get_ChipID
 * 描述  ：获取芯片ID,与flash中存储的数据比较判断是否一致
 * 输入  ：无
 * 输出  ：chip_passed
 ************************************************************/
u8 Get_ChipID(void)
{
	u8 i=0;
	u8 chip_passed=0;
	u32 ChipUniqueID[3];   //芯片96bit UID
	ChipUniqueID[0] = *(__IO u32 *)(0X1FFFF7F0); // 高字节
	ChipUniqueID[1] = *(__IO u32 *)(0X1FFFF7EC); // 
	ChipUniqueID[2] = *(__IO u32 *)(0X1FFFF7E8); // 低字节
	for(i=0;i<=2;i++)
	{
		if(ChipUniqueID[i]==ChipUniqueID_Flash[i])
			chip_passed++;
	}
	return chip_passed;
}
/********************************************************
 * 函数名：Is_Coded
 * 描述  ：判断设备是否加密
 * 输入  ：
 * 输出  ：
*********************************************************/
void Is_Coded(void)
{
	u8 i;
	u8 code_cmp1=0;  //用于判断是否写入过密码
	u8 code_cmp2=0;
	/////////////判断是否写入过密码//////////////////////
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
