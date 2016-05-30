#include "main.h"
#include "math.h"

static u8 t_convert=0;   //记录收到有效位置信息的次数

/***********************************************************
 * 函数名: GPSIsOn
 * 描述  ：判断是否要开GPS
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void GPSIsOn(void)
{
		if (interval_num == 0) // 开GPS
		{
					GPS_ON();
					TIM4_ON();
					tim2_cnt = 0;
					TIM2_ON();
					interval_num++;
					if (intervalA == 0)  //发送间隔3分钟
								interval_num = 0;
					BKP_WriteBackupRegister(BKP_DR1,interval_num);
		}
		else  //30s 和1分钟  两分钟开一次GPS
		{
					if (intervalA == 5) // 发送间隔30s
					{
							interval_num++;
							if (interval_num == 4)
										interval_num = 0;
							BKP_WriteBackupRegister(BKP_DR1,interval_num);
							Enter_PowersaveMode();
					} 
					else if (intervalA == 2)  // 发送间隔为1分钟
					{
							interval_num++;
							if (interval_num == 2)
										interval_num = 0;
							BKP_WriteBackupRegister(BKP_DR1,interval_num);
							Enter_PowersaveMode();
					}
					else 
					{
					}
		}
}

int GPS_RMC_Parse(char *line, GPS_INFO *GPS)
{
	char *buf = line;
	u8 ch, status;
//	u16 time_n,delta_time;
	double tantemp1,tantemp2,tantemp3,tantemp4,tantemp5;
//	double tantemp3;
	double delta_jingdu,delta_weidu;
//	double sogtemp;
	
	ch = buf[5];
	status = buf[GetComma(2, buf)];
	
	if (ch == 'C')  //如果第五个字符是C，($GPRMC)
	{
//  		USART_SendData(USART1, status);
//   		while (!(USART1->SR & USART_FLAG_TXE)) ;
		
		if (status == 'A')  //如果数据有效，则分析
		{
			t_convert++;
			if(t_convert==6)
			{
	// 			tx1buf[0]='$'; tx1buf[1]=0x1C; tx1buf[2]=0x01; //GPS有效指令
	// 			com1sendback(); //
				
				GPS->latitude = Get_Double_Number(&buf[GetComma( 3, buf)]);
				GPS->longitude = Get_Double_Number(&buf[GetComma( 5, buf)]);

				GPS->second = ((buf[7]-'0')*10+(buf[8]-'0'))*3600/2 + ((buf[9]-'0')*10+(buf[10]-'0'))*60/2 + ((buf[11]-'0')*10+(buf[12]-'0'))/2;	//以2秒为单位

				GPS->NS = buf[GetComma(4, buf)];
				GPS->EW = buf[GetComma(6, buf)];
		
				if(GPS -> NS == 'N') GPS->latitude  = GPS->latitude*1;  //纬度
				else GPS->latitude  = GPS->latitude*(-1);
				
				if(GPS -> EW == 'E') GPS->longitude = GPS->longitude*1;	//经度
				else GPS->longitude  = GPS->longitude*(-1);

				tantemp4 = jingdu;  tantemp5 = weidu; //旧				
				jingdu=GPS->longitude; //更新经纬度信息
				weidu=GPS->latitude;
				tantemp1 = weidu; tantemp2 = jingdu; //新
				delta_jingdu = tantemp2 - tantemp4;
				delta_weidu = tantemp1 - tantemp5;		
				
				//航速  time_o的backup挪作他用
				{
					/*time_n = GPS->second;	
					if(time_n > time_o)  
					{
						delta_time = time_n-time_o;
						delta_jingdu = delta_jingdu/10000; delta_jingdu = delta_jingdu/60;
						delta_weidu = delta_weidu/10000; delta_weidu = delta_weidu/60;
						sogtemp = sqrt(delta_jingdu*delta_jingdu + delta_weidu*delta_weidu);
						sogtemp = sogtemp*57267.6886495/(delta_time*2);
						sog = (u16)(sogtemp*10);
					}
					else sog = 0;

					time_o = time_n;*/
					
					sog = Get_Double_Number_sog(&buf[GetComma( 7, buf)]);
				}
				
				//航向
				Read_Flash_Cogsel();
				if(cogsel == 0)  //流网
				{
					if(delta_jingdu > 0 && delta_weidu > 0)
						tantemp3 = atan2(delta_jingdu,delta_weidu) * 1800 / 3.14159;
					else if(delta_jingdu > 0 && delta_weidu < 0)
						tantemp3 = 1800 - atan2(delta_jingdu,(-1)*(delta_weidu)) * 1800 / 3.14159;
					else if(delta_jingdu < 0 && delta_weidu < 0)
						tantemp3 = 1800 + atan2((-1)*(delta_jingdu),(-1)*(delta_weidu)) * 1800 / 3.14159;
					else if(delta_jingdu < 0 && delta_weidu > 0)
						tantemp3 = 3600 - atan2((-1)*(delta_jingdu),delta_weidu) * 1800 / 3.14159;

					direction = (u16)tantemp3;
					
					if(delta_jingdu == 0 && delta_weidu > 0) direction = 0;
					else if(delta_jingdu == 0 && delta_weidu < 0) direction = 1800;
					else if(delta_jingdu == 0 && delta_weidu == 0) direction = 0;
					else if(delta_jingdu > 0 && delta_weidu == 0) direction = 900;
					else if(delta_jingdu < 0 && delta_weidu == 0) direction = 2700;
					
					//direction = Get_Double_Number_direction(&buf[GetComma( 8, buf)]);
				}
				else //张网
				{
					direction = 0;
				}
				
				Write_GPS_Info();
				
				GPS_OFF(); //GPS更新一次信息即关闭
				TIM2_OFF();
				TIM4_OFF();
				LED_OFF();
				Enter_PowersaveMode();
		 }
		}
		
	}
	
	return 1;
}
