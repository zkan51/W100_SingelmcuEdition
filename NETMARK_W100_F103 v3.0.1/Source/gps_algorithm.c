#include "main.h"
#include "math.h"

//====================================================================//
// �﷨��ʽ: static double Str_To_Double(char *buf)
// ʵ�ֹ��ܣ� ��һ���ַ���ת���ɸ�����
// ��    �����ַ���
// �� �� ֵ��ת����˫����ֵ
//====================================================================//
double Str_To_Double(char *buf)
{
	double rev = 0;
	double dat;
	int integer = 1;
	char *str = buf;
	int i;
	
	while(*str != '\0')
	{
		switch(*str)
		{
			case '0':
				dat = 0;
				break;
			case '1':
				dat = 1;
				break;
			case '2':
				dat = 2;
				break;		
			case '3':
				dat = 3;
				break;
			case '4':
				dat = 4;
				break;
			case '5':
				dat = 5;
				break;
			case '6':
				dat = 6;
				break;
			case '7':
				dat = 7;
				break;
			case '8':
				dat = 8;
				break;
			case '9':
				dat = 9;
				break;
			case '.':
				dat = '.';
				break;
		}
		if(dat == '.')
		{
			integer = 0;
			i = 1;
			str++;
			continue;
		}
		if( integer == 1 )
		{
			rev = rev * 10 + dat;
		}
		else
		{
			rev = rev + dat / (10 * i);
			i = i * 10 ;
		}
		str++;
	}
	
	return rev;
}
												
//====================================================================//
// �﷨��ʽ: static double Get_Double_Number(char *s)
// ʵ�ֹ��ܣ��Ѹ����ַ�����һ������֮ǰ���ַ�ת����˫������[���޸�Ϊ����ֵΪlong������]
// ��    �����ַ���
// �� �� ֵ��ת����˫����ֵ
//====================================================================//
double Get_Double_Number(char *s)
{
	char buf[10];
	u8 i;
	double rev;
	long a,temp;
	
	i = GetComma(1, s);
	i = i - 1;
	strncpy(buf, s, i);
	buf[i] = 0;
	rev = Str_To_Double(buf);
	
	temp = rev/100;
	a = temp*600000 + (rev - temp*100)*10000;
	
	return a;	
}

double Get_Double_Number_sog(char *s)  //for sog
{
	char buf[10];
	u8 i;
	double rev;
	unsigned int a;
	
	i = GetComma(1, s);
	i = i - 1;
	strncpy(buf, s, i);
	buf[i] = 0;
	rev = Str_To_Double(buf);
	
	a = rev*10/1;
	
	return a;	
}

double Get_Double_Number_direction(char *s)  //for direction
{
	char buf[10];
	u8 i;
	double rev;
	unsigned int a;
	
	i = GetComma(1, s);
	i = i - 1;
	strncpy(buf, s, i);
	buf[i] = 0;
	rev = Str_To_Double(buf);
	
	a = rev*10/1; //a = rev/1 *10;
	
	return a;	
}

double Get_Double_Number_time(char *s)  //for direction
{
	char buf[10];
	u8 i;
	double rev;
	unsigned long a;
	
	i = GetComma(1, s);
	i = i - 1;
	strncpy(buf, s, i);
	buf[i] = 0;
	rev = Str_To_Double(buf);
	
	a = rev/1;
	
	return a;	
}
//====================================================================//
// �﷨��ʽ��static uchar GetComma(uchar num,char *str)
// ʵ�ֹ��ܣ������ַ����и������ŵ�λ��
// ��    �������ҵĶ����ǵڼ����ĸ�������Ҫ���ҵ��ַ���
// �� �� ֵ��0
//====================================================================//
u8 GetComma(u8 num, char *str)
{
	u8 i,j = 0;
	int len = strlen(str);

	for(i = 0; i < len; i++)
	{
		if(str[i] == ',')
			j++;
		if(j == num)
			return i + 1;	
	}

	return 0;	 
}


/*void RefreshGPS(void) //�ռ�GPS��Ϣ
{
				GPS->latitude = Get_Double_Number(&buf[GetComma( 3, buf)]);
				GPS->longitude = Get_Double_Number(&buf[GetComma( 5, buf)]);

				GPS->second = ((buf[7]-'0')*10+(buf[8]-'0'))*3600/2 + ((buf[9]-'0')*10+(buf[10]-'0'))*60/2 + ((buf[11]-'0')*10+(buf[12]-'0'))/2;	//��2��Ϊ��λ

				GPS->NS = buf[GetComma(4, buf)];
				GPS->EW = buf[GetComma(6, buf)];
		
				if(GPS -> NS == 'N') GPS->latitude  = GPS->latitude*1;  //γ��
				else GPS->latitude  = GPS->latitude*(-1);
				
				if(GPS -> EW == 'E') GPS->longitude = GPS->longitude*1;	//����
				else GPS->longitude  = GPS->longitude*(-1);

				jingdu=GPS->longitude; //���¾�γ����Ϣ
				weidu=GPS->latitude;
}*/

