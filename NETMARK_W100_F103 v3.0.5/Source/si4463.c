#include "si4463.h"
#ifdef SI4463

//4463初始化参数
u8 config_table[] = RADIO_CONFIGURATION_DATA_ARRAY ; 
u8 sigle_freq_config_table[] = SINGLE_FREQ_RADIO_CONFIGURATION_DATA_ARRAY;
/*
=================================================================================
SI4463_IOSET()
Function : 4463用IO口（不包括SPI）配置
		   PA5 = NSEL_CSN 输出，PA11 = si4463使能角, PB13 = SI_NIRQ(输入)	
INTPUT   : NONE
OUTPUT   : NONE
=================================================================================
*/
void SI4463_IOSET(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE );//A,B时钟使能 

	// PA5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;  // PA5 = NSEL CSN 推挽 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_5);
	
	// PB13
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;  //PB13<--NIRQ 上拉输入 产生中断时为低
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//GPIO_Pin_11控制LDO的开关
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;  //
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_11);	 
} 
/*
=================================================================================
u8 SPI_ExchangeByte(u8 TxData)
Function : wait the device ready to response a command
INTPUT   : NONE
OUTPUT   : NONE
=================================================================================
*/
u8 SPI_ExchangeByte(u8 TxData)
{  	
	u8 i,ret=0;	
	
	for(i=0;i<8;i++)
	{          
			if(TxData&0x80) // MSB first
				SI_SDO_HIGH;  // 即GPIO_SetBits，   RF_SDI=1;
			else
				SI_SDO_LOW;   // 即GPIO_ResetBits， RF_SDI=0;
			TxData<<=1; 	  
			ret<<=1;

// 生成SCK时钟，空闲时SCK默认为低电平,时钟速率大概在0.4MHz左右（周期约2.5us）
			SI_SCK_HIGH;  //RF_SCK_HIGH(); 准备好SDO上的数据后，将SCK拉高，形成上升沿，然后在下一句读取SDI上的数据
			delay_us(20);
			if(SI4463_SDI)
				ret|=1; 
			SI_SCK_LOW;   //RF_SCK_LOW();  完成数据读取后，将时钟恢复为低电平
	}	
	return ret;	// ret中存储的是从4463中接收到的Byte				    
}
/*
=================================================================================
SI446X_WAIT_CTS( );
Function : wait until the device is ready to response a command
INTPUT   : NONE
OUTPUT   : NONE
=================================================================================
*/
void SI446X_WAIT_CTS(void)
{
	u8 cts; // clear to send, 表明可以发射指令到RF芯片
	do
	{
			SI_CSN_LOW;				
			SPI_ExchangeByte( READ_CMD_BUFF );	// READ_CMD_BUFF = 0x44, 用于发起询问
			cts = SPI_ExchangeByte( 0xFF );		
			SI_CSN_HIGH;
	}while( cts != 0xFF );
}
/*
=================================================================================
SI446X_READ_RESPONSE(...);
--modified by Wangsi
描述 : 读取SI446X芯片返回的信息，时序参照《AN633:SI4x6x编程指南》 7.3节 figure 10
输入 : buffer：	存储返回的数据
       size：	返回的数据个数（byte）
输出 : NONE
=================================================================================
*/
void SI446X_READ_RESPONSE( u8 *buffer, u8 size )
{
    // 发送询问请求，并等待cts==0xFF
	u8 cts; // clear to send, 表明可以发射指令到RF芯片
	while(1){
		SI_CSN_LOW;				
        SPI_ExchangeByte( READ_CMD_BUFF );	// READ_CMD_BUFF = 0x44, 用于发起询问
        cts = SPI_ExchangeByte( 0xFF );	
		if(cts==0xFF){
			*buffer++ = 0xFF;
			break;
		}
        SI_CSN_HIGH;
	}
    //SI_CSN_LOW;
	//SPI_ExchangeByte( READ_CMD_BUFF );
	while( size -- )
    {
        *buffer++ = SPI_ExchangeByte( 0xFF );
    }
    SI_CSN_HIGH;
}
/*
=================================================================================
SI446X_CMD( );
Function : Send a command to the device
INTPUT   : cmd, the buffer stores the command array
           cmdsize, the size of the command array
OUTPUT   : NONE
=================================================================================
*/
void SI446X_CMD(u8 *cmd, u8 cmdsize)
{
		SI446X_WAIT_CTS( );	
		SI_CSN_LOW;
		while( cmdsize -- )
		{
				SPI_ExchangeByte( *cmd++ );
		}
		SI_CSN_HIGH;
}
/*
=================================================================================
SI4463_Init( );
Function : 
INTPUT   : 
           
OUTPUT   : NONE
=================================================================================
*/
void SI4463_Init(void)
{  	
	SI4463_IOSET();	
	// SI446X_RESET();	
	SI446X_CONFIG_INIT(); 	
}
/*
=================================================================================
SI446X_RESET( );
Function : reset the SI446x device
INTPUT   : NONE
OUTPUT   : NONE
=================================================================================
*/
void SI446X_RESET(void)
{		
	SI_SDN_HIGH;
	delay_ms(200);
	SI_SDN_LOW;
	SI_CSN_HIGH;
	delay_ms(100);
}
/*
=================================================================================
SI446X_SET_PROPERTY_1( );
Function : Set the PROPERTY of the device, only 1 byte
INTPUT   : GROUP_NUM, the group and number index
           prioriry,  the value to be set
OUTPUT   : NONE
=================================================================================
*/
void SI446X_SET_PROPERTY_1( SI446X_PROPERTY GROUP_NUM, u8 proirity )
{
    u8 cmd[5];

    cmd[0] = SET_PROPERTY;
    cmd[1] = GROUP_NUM>>8;
    cmd[2] = 1;
    cmd[3] = GROUP_NUM;
    cmd[4] = proirity;
    SI446X_CMD( cmd, 5 );
}
/*
=================================================================================
SI446X_SET_BT_0_4();
--modified by Wangsi
功能：将GMSK调制的BT值设为0.4，参考官网论坛的帖子http://community.silabs.com/t5/Wireless/si4463-GMSK-spectrum-spread/m-p/159921/highlight/true#M9375
输入：无
输出：无
=================================================================================
*/
void SI446X_SET_BT_0_4()
{
    u8 cmd[5];
	
    cmd[0] = 0x11;    cmd[1] = 0x20;    cmd[2] = 0x01;    cmd[3] = 0x0f;    cmd[4] = 0x52;
    SI446X_CMD( cmd, 5 );
	
	cmd[0] = 0x11;    cmd[1] = 0x20;    cmd[2] = 0x01;    cmd[3] = 0x10;    cmd[4] = 0x4f;
    SI446X_CMD( cmd, 5 );
	
	cmd[0] = 0x11;    cmd[1] = 0x20;    cmd[2] = 0x01;    cmd[3] = 0x11;    cmd[4] = 0x45;
    SI446X_CMD( cmd, 5 );
	
	cmd[0] = 0x11;    cmd[1] = 0x20;    cmd[2] = 0x01;    cmd[3] = 0x12;    cmd[4] = 0x37;
    SI446X_CMD( cmd, 5 );
	
	cmd[0] = 0x11;    cmd[1] = 0x20;    cmd[2] = 0x01;    cmd[3] = 0x13;    cmd[4] = 0x28;
    SI446X_CMD( cmd, 5 );
	
	cmd[0] = 0x11;    cmd[1] = 0x20;    cmd[2] = 0x01;    cmd[3] = 0x14;    cmd[4] = 0x1a;
    SI446X_CMD( cmd, 5 );
	
	cmd[0] = 0x11;    cmd[1] = 0x20;    cmd[2] = 0x01;    cmd[3] = 0x15;    cmd[4] = 0x10;
    SI446X_CMD( cmd, 5 );
	
	cmd[0] = 0x11;    cmd[1] = 0x20;    cmd[2] = 0x01;    cmd[3] = 0x16;    cmd[4] = 0x09;
    SI446X_CMD( cmd, 5 );
	
	cmd[0] = 0x11;    cmd[1] = 0x20;    cmd[2] = 0x01;    cmd[3] = 0x17;    cmd[4] = 0x04;
    SI446X_CMD( cmd, 5 );
}
/*
=================================================================================
SI446X_GPIO_CONFIG( );
Function : config the GPIOs, IRQ, SDO
INTPUT   :
OUTPUT   : NONE
=================================================================================
*/
void SI446X_GPIO_CONFIG( u8 G0, u8 G1, u8 G2, u8 G3,
                         u8 IRQ, u8 SDO, u8 GEN_CONFIG )
{
    u8 cmd[10];
    cmd[0] = GPIO_PIN_CFG;
    cmd[1] = G0;
    cmd[2] = G1;
    cmd[3] = G2;
    cmd[4] = G3;
    cmd[5] = IRQ;
    cmd[6] = SDO;
    cmd[7] = GEN_CONFIG;
    SI446X_CMD( cmd, 8 );
    SI446X_READ_RESPONSE( cmd, 8 );
}
/*
=================================================================================
SI446X_CONFIG_INIT( );
Function : configuration the device
INTPUT   : NONE
OUTPUT   : NONE
=================================================================================
*/
void SI446X_CONFIG_INIT(void)
{
	u8  i;
	u16 j = 0;

	// 发送WDS生成的头文件中的指令到SI4463芯片
	while( ( i = config_table[j] ) != 0 )
	{
		j += 1;		
		SI446X_CMD( config_table + j, i );			
		j += i;
	} 	
	
	SI446X_SET_BT_0_4(); // 将BT值设为0.4

	//重要： 4463的GDO2，GDO3控制射频开关，  0X20 ,0X21 
	//发射时必须： GDO2=1，GDO3=0
	//接收时必须： GDO2=0，GDO3=1
	SI446X_GPIO_CONFIG( 0, 0, 0x20, 0x21, 0, 0, 0 );//重要
}
/*
=================================================================================
SI446X_SINGLE_FREQ_CONFIG_INIT( );
Function : configuration the device
INTPUT   : NONE
OUTPUT   : NONE
=================================================================================
*/
void SI446X_SINGLE_FREQ_CONFIG_INIT(void)
{
	u8 i;
    u16 j = 0;

    while( ( i = sigle_freq_config_table[j] ) != 0 )
    {
        j += 1;		
        SI446X_CMD( sigle_freq_config_table + j, i );		
        j += i;
    } 	
#if PACKET_LENGTH > 0           //fixed packet length
				SI446X_SET_PROPERTY_1( PKT_FIELD_1_LENGTH_7_0, PACKET_LENGTH );
				SI446X_SET_PROPERTY_1( PKT_FIELD_1_CRC_CONFIG, 0xA2 );
				SI446X_SET_PROPERTY_1( PKT_CRC_CONFIG, 0x05 );
		
#else                           //variable packet length
    SI446X_SET_PROPERTY_1( PKT_CONFIG1, 0x00 );	
    SI446X_SET_PROPERTY_1( PKT_CRC_CONFIG, 0x00 );
    SI446X_SET_PROPERTY_1( PKT_LEN_FIELD_SOURCE, 0x01 );
    SI446X_SET_PROPERTY_1( PKT_LEN, 0x2A );
    SI446X_SET_PROPERTY_1( PKT_LEN_ADJUST, 0x00 );
    SI446X_SET_PROPERTY_1( PKT_FIELD_1_LENGTH_12_8, 0x00 );
    SI446X_SET_PROPERTY_1( PKT_FIELD_1_LENGTH_7_0, 0x01 );
    SI446X_SET_PROPERTY_1( PKT_FIELD_1_CONFIG, 0x00 );
    SI446X_SET_PROPERTY_1( PKT_FIELD_1_CRC_CONFIG, 0x00 );
    SI446X_SET_PROPERTY_1( PKT_FIELD_2_LENGTH_12_8, 0x00 );
    SI446X_SET_PROPERTY_1( PKT_FIELD_2_LENGTH_7_0, 0x10 );
    SI446X_SET_PROPERTY_1( PKT_FIELD_2_CONFIG, 0x00 );
    SI446X_SET_PROPERTY_1( PKT_FIELD_2_CRC_CONFIG, 0x00 );
#endif //PACKET_LENGTH

    //重要： 4463的GDO2，GDO3控制射频开关，  0X20 ,0X21 
    //发射时必须： GDO2=1，GDO3=0
    //接收时必须： GDO2=0，GDO3=1
    SI446X_GPIO_CONFIG( 0, 0, 0x20, 0x21, 0, 0, 0 );//重要 	
}
/*
=================================================================================
SI446X_RX_FIFO_RESET( );
Function : reset the RX FIFO of the device
INTPUT   : None
OUTPUT   : NONE
=================================================================================
*/
void SI446X_RX_FIFO_RESET( void )
{
    u8 cmd[2];

    cmd[0] = FIFO_INFO;
    cmd[1] = 0x02;
    SI446X_CMD( cmd, 2 );
}
/*
=================================================================================
SI446X_TX_FIFO_RESET( );
Function : reset the TX FIFO of the device
INTPUT   : None
OUTPUT   : NONE
=================================================================================
*/
void SI446X_TX_FIFO_RESET( void )
{
    u8 cmd[2];

    cmd[0] = FIFO_INFO;
    cmd[1] = 0x01;
    SI446X_CMD( cmd, 2 );
}
/*
=================================================================================
SI446X_TX_FIFO_SPACE( );
--modified by Wangsi
Function : 读取FIFO信息
INTPUT   : None
OUTPUT   : NONE
=================================================================================
*/
void SI446X_TX_FIFO_SPACE( u8* buffer )
{
    u8 cmd[2];

    cmd[0] = FIFO_INFO;
    cmd[1] = 0x00;
    SI446X_CMD( cmd, 2 );
	
	SI446X_READ_RESPONSE( buffer, 3 );
}
/*
=================================================================================
SI446X_PART_INFO( );
Function : Read the PART_INFO of the device, 9 bytes needed
INTPUT   : buffer, the buffer stores the part information
OUTPUT   : NONE
=================================================================================
*/
void SI446X_PART_INFO(u8 *buffer)
{
	u8 cmd = PART_INFO;

    SI446X_CMD( &cmd, 1 );
    SI446X_READ_RESPONSE( buffer, 9 );
}
/*
=================================================================================
SI446X_FUNC_INFO( );
--modified by Wangsi
Description : Read the FUNC_INFO of the device, 7 bytes
INTPUT   	: buffer: used to store the func information
OUTPUT   	: NONE
=================================================================================
*/
void SI446X_FUNC_INFO(u8 *buffer)
{
	u8 cmd = FUNC_INFO;

    SI446X_CMD( &cmd, 1 );
    SI446X_READ_RESPONSE( buffer, 7 );
}
/*
=================================================================================
SI446X_INT_STATUS( );
描述 : 读取芯片的中断状态（需要9 bytes）, 并清除相应的中断pending，寄存器含义参考si446x_API说明2.2.12
输入 : cmd--清中断指令
	   buffer--存放中断状态
=================================================================================
*/
void SI446X_INT_STATUS( u8 *buffer )
{
    u8 cmd[4];
    cmd[0] = GET_INT_STATUS;
    cmd[1] = 0;
    cmd[2] = 0;
    cmd[3] = 0;

    SI446X_CMD( cmd, 4 );
    SI446X_READ_RESPONSE( buffer, 9 );
}
/*
=================================================================================
SI446X_GET_PROPERTY_X()
--modified by Wangsi
功能： 
输入：
输出：
=================================================================================
*/
void SI446X_GET_PROPERTY(u8 GROUP, u8 NUM_PROPS, u8 START_PROP, u8* buffer )
{
    u8 cmd[4];
	
    cmd[0] = GET_PROPERTY/*0x12*/; /*cmd[1] = 0x20;    cmd[2] = 0x01;    cmd[3] = 0x0f;*/
    cmd[1] = GROUP;
	cmd[2] = NUM_PROPS;
	cmd[3] = START_PROP;
	
	SI446X_CMD( cmd, 4 );
	SI446X_READ_RESPONSE( buffer, NUM_PROPS+1 ); // buffer[0]为CTS（正常值为0xFF），后面的才是真正的Properties
}
/*
=================================================================================
SI446X_WRITE_TX_FIFO();
Function : Write data to TX FIFO
INTPUT   : buffer, the buffer stores the int status
OUTPUT   : NONE
=================================================================================
*/
void SI446X_WRITE_TX_FIFO(void)
{
    u8 cmd[2];
    cmd[0] = WRITE_TX_FIFO;
    cmd[1] = 0;
    SI446X_CMD( cmd, 2 );
}


/*
=================================================================================
SI446X_WRITE_TX_FIFO( );
Function : Write data to TX FIFO
INTPUT   : buffer, the buffer stores the int status
OUTPUT   : NONE
=================================================================================
*/
void SI446X_START_TX( u8 *buffer )
{
    u8 cmd[1];
    cmd[0] = START_TX;
    SI446X_CMD( cmd, 1 );
}

/*
=================================================================================
SI446X_SEND_PACKET( );
--modified by Wangsi
描述 : 	发射一个包，如果是超出FIFO长度（64bytes）的包，需要使能TX_FIFO_ALMOST_EMPTY(TFAE)中断，
		FIFO快空时，填满FIFO，直到最后一段数据，将其全部写入FIFO。参考AN633 si4x6x编程指南
		第10.16节
输入 : 	buffer--待发射数据
		size--数组长度
		channel--发射通道
		condition--发射完成后转移到的状态
		size_of_FIFO--芯片内置FIFO的容量
		tx_threshold--FIFO的可用空间达到此门限时，芯片将产生TFAE中断，并拉低NIRQ引脚的点平
输出 : 	无
=================================================================================
*/
void SI446X_SEND_PACKET( u8 *txbuffer, u16 size, u8 channel, u8 condition,\
	u8 size_of_FIFO/*FIFO大小为 64 bytes*/, u8 tx_threshold/*可以设置为48*/)
{	
    u8 cmd[5];
	u16 tx_len = size;	
	u8 int_status_buf[9] = {0};	// 存放芯片的中断状态
	u8 i; 						// 计数用
	u8 flag_last_seg = 0;  		// 表明是否已经发送到最后一段数据
	u8 *txbuf0 = txbuffer; 		// 指向发射数据的起始点

	//variables for test
	u8 FIFO_info[3] = {0};
	//

	// 先清空FIFO
    SI446X_TX_FIFO_RESET();	

	// 短包和长包的代码应该可以合并，有空改一下
	if(tx_len <= size_of_FIFO){ // 短包
		// 将待发送数据写入FIFO
		SI_CSN_LOW;
		SPI_ExchangeByte( WRITE_TX_FIFO );		
		while( size -- ) { SPI_ExchangeByte( *txbuffer++ ); }
		SI_CSN_HIGH;
		
		// 设置发射参数
		cmd[0] = START_TX;
		cmd[1] = channel;
		cmd[2] = condition;
		cmd[3] = (u8)(tx_len>>8);   // tx_len[15:8]
		cmd[4] = (u8)tx_len;		// tx_len[7:0]
		SI446X_CMD( cmd, 5 );   	// 开始发射

		/*
		while(SI4463_NIRQ!=0); 				// 等待TFAE中断
		SI446X_INT_STATUS(int_status_buf);  // 清TFAE中断

		while(SI4463_NIRQ!=0); 				// 等待发射完成（PACKET_SENT）中断
		SI446X_INT_STATUS(int_status_buf);  // 清PS中断
		*/
		
		do{
				SI446X_INT_STATUS(int_status_buf);		// 读取中断状态并清空中断Pending
		}while((int_status_buf[4]&0x02) != 0x02);	// 轮询TFAE中断
		
		do{
			SI446X_INT_STATUS(int_status_buf);		// 读取中断状态并清空中断Pending
		}while( (int_status_buf[4]&0x20) != 0x20 ); // 轮询PACKET_SENT中断
		
		// 回到“ready”状态
		cmd[0] = CHANGE_STATE;
		cmd[1] = 0x03;
		SI446X_CMD( cmd, 2 );
	}else{ // 长包
		// 首先将FIFO填满
		SI_CSN_LOW;
		SPI_ExchangeByte( WRITE_TX_FIFO );		
		for(i=0;i<size_of_FIFO;i++) { SPI_ExchangeByte( *txbuffer++ ); }
		SI_CSN_HIGH;	
	
		SI446X_TX_FIFO_SPACE( FIFO_info ); // 此时FIFO剩余空间应当为0
		
		// 设置发射参数
		cmd[0] = START_TX;
		cmd[1] = channel;
		cmd[2] = condition;
		cmd[3] = (u8)(tx_len>>8);   // tx_len[15:8]
		cmd[4] = (u8)tx_len;		// tx_len[7:0]
		SI446X_CMD( cmd, 5 );   	// 开始发射

		// 在最后一段待发数据写入FIFO之前，轮询FIFO将空(TX_FIFO_ALMOST_EMPTY)中断
		while(flag_last_seg!=1){ 	// 待发射数据尚未全部写入FIFO			
			do{
				SI446X_INT_STATUS(int_status_buf);		// 读取中断状态并清空中断Pending
			}while((int_status_buf[3]&0x02) != 0x02);	// 轮询TFAE中断
			
			// 继续将数据写入FIFO
			SI_CSN_LOW;
			SPI_ExchangeByte( WRITE_TX_FIFO );	 
			for(i=0;i<tx_threshold;i++){		 // 实验表明最好填满整个FIFO，否则可能会出问题
				if(txbuffer <= txbuf0+tx_len-1){ // txbuffer还没有超出待发数组的边界					
					SPI_ExchangeByte( *txbuffer++ );									
				}else{
					flag_last_seg = 1; 			 // 最后一个数据段也发完					
					break;
				}			
			}
			SI_CSN_HIGH;
			
			// 清中断Pending
			//SI446X_INT_STATUS(int_status_buf); // 这一步可能是多余的
		}
		
		do{
			SI446X_INT_STATUS(int_status_buf);		// 读取中断状态并清空中断Pending
		}while( (int_status_buf[4]&0x20) != 0x20 ); // 轮询PACKET_SENT中断

		// 回到“ready”状态
		cmd[0] = CHANGE_STATE;
		cmd[1] = 0x03;
		SI446X_CMD( cmd, 2 );				
	}
}

/*
=================================================================================
testinf( );
Function : configure the device
INTPUT   : NONE
OUTPUT   : NONE
=================================================================================
*/
void testinf(u8 Rf_Rec_Buf[])
{
    /*SI446X_CMD( &Rf_Rec_Buf[0], 4 );
	SI446X_POWER_UP( 0xffffff );
	SI446X_READ_RESPONSE( &Rf_Rec_Buf[0], 4 );
	SI446X_NOP( );
	SI446X_PART_INFO( &Rf_Rec_Buf[0] );
	SI446X_FUNC_INFO( &Rf_Rec_Buf[0] );
	SI446X_INT_STATUS( &Rf_Rec_Buf[0] );
	SI446X_GET_PROPERTY_X( 3, 4, &Rf_Rec_Buf[0]  );
	SI446X_SET_PROPERTY_X( 3, 4, &Rf_Rec_Buf[0]  );
	SI446X_SET_PROPERTY_1( 3, 3 );
	SI446X_GET_PROPERTY_1( 3 );
	SI446X_RESET( );
	SI446X_CONFIG_INIT(  );
	
	SI446X_W_TX_FIFO( &Rf_Rec_Buf[0], 4 );
	SI446X_SEND_PACKET( &Rf_Rec_Buf[0], 8, 4, 4 );
	SI446X_START_TX( 4, 4, 4 );
	SI446X_TX_FIFO_RESET(  );
	SI446X_RX_FIFO_RESET(  );
	SI446X_START_RX( 3, 3, 3,
                      3, 3, 3 );
	SI446X_PKT_INFO( &Rf_Rec_Buf[0], 3, 3, 3 );
	SI446X_FIFO_INFO( &Rf_Rec_Buf[0] );
	SI446X_READ_PACKET( &Rf_Rec_Buf[0] );*/ 
}
#if debug
/*
=================================================================================
Test_Printf( );
Function : configuration the device
INTPUT   : NONE
OUTPUT   : NONE
=================================================================================
*/
void Test_Printf(u8 name[20], u8 buf[], u8 length)
{
	u8 i = 0;
	for(i = 0; i < length; i++)
	{
		printf("%sbuf[%d] = %x\r\n", name, i, buf[i]);
	}	
}
#endif
															   

#endif
