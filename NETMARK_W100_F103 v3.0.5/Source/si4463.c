#include "si4463.h"
#ifdef SI4463

//4463��ʼ������
u8 config_table[] = RADIO_CONFIGURATION_DATA_ARRAY ; 
u8 sigle_freq_config_table[] = SINGLE_FREQ_RADIO_CONFIGURATION_DATA_ARRAY;
/*
=================================================================================
SI4463_IOSET()
Function : 4463��IO�ڣ�������SPI������
		   PA5 = NSEL_CSN �����PA11 = si4463ʹ�ܽ�, PB13 = SI_NIRQ(����)	
INTPUT   : NONE
OUTPUT   : NONE
=================================================================================
*/
void SI4463_IOSET(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE );//A,Bʱ��ʹ�� 

	// PA5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;  // PA5 = NSEL CSN ���� 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_5);
	
	// PB13
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;  //PB13<--NIRQ �������� �����ж�ʱΪ��
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//GPIO_Pin_11����LDO�Ŀ���
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
				SI_SDO_HIGH;  // ��GPIO_SetBits��   RF_SDI=1;
			else
				SI_SDO_LOW;   // ��GPIO_ResetBits�� RF_SDI=0;
			TxData<<=1; 	  
			ret<<=1;

// ����SCKʱ�ӣ�����ʱSCKĬ��Ϊ�͵�ƽ,ʱ�����ʴ����0.4MHz���ң�����Լ2.5us��
			SI_SCK_HIGH;  //RF_SCK_HIGH(); ׼����SDO�ϵ����ݺ󣬽�SCK���ߣ��γ������أ�Ȼ������һ���ȡSDI�ϵ�����
			delay_us(20);
			if(SI4463_SDI)
				ret|=1; 
			SI_SCK_LOW;   //RF_SCK_LOW();  ������ݶ�ȡ�󣬽�ʱ�ӻָ�Ϊ�͵�ƽ
	}	
	return ret;	// ret�д洢���Ǵ�4463�н��յ���Byte				    
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
	u8 cts; // clear to send, �������Է���ָ�RFоƬ
	do
	{
			SI_CSN_LOW;				
			SPI_ExchangeByte( READ_CMD_BUFF );	// READ_CMD_BUFF = 0x44, ���ڷ���ѯ��
			cts = SPI_ExchangeByte( 0xFF );		
			SI_CSN_HIGH;
	}while( cts != 0xFF );
}
/*
=================================================================================
SI446X_READ_RESPONSE(...);
--modified by Wangsi
���� : ��ȡSI446XоƬ���ص���Ϣ��ʱ����ա�AN633:SI4x6x���ָ�ϡ� 7.3�� figure 10
���� : buffer��	�洢���ص�����
       size��	���ص����ݸ�����byte��
��� : NONE
=================================================================================
*/
void SI446X_READ_RESPONSE( u8 *buffer, u8 size )
{
    // ����ѯ�����󣬲��ȴ�cts==0xFF
	u8 cts; // clear to send, �������Է���ָ�RFоƬ
	while(1){
		SI_CSN_LOW;				
        SPI_ExchangeByte( READ_CMD_BUFF );	// READ_CMD_BUFF = 0x44, ���ڷ���ѯ��
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
���ܣ���GMSK���Ƶ�BTֵ��Ϊ0.4���ο�������̳������http://community.silabs.com/t5/Wireless/si4463-GMSK-spectrum-spread/m-p/159921/highlight/true#M9375
���룺��
�������
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

	// ����WDS���ɵ�ͷ�ļ��е�ָ�SI4463оƬ
	while( ( i = config_table[j] ) != 0 )
	{
		j += 1;		
		SI446X_CMD( config_table + j, i );			
		j += i;
	} 	
	
	SI446X_SET_BT_0_4(); // ��BTֵ��Ϊ0.4

	//��Ҫ�� 4463��GDO2��GDO3������Ƶ���أ�  0X20 ,0X21 
	//����ʱ���룺 GDO2=1��GDO3=0
	//����ʱ���룺 GDO2=0��GDO3=1
	SI446X_GPIO_CONFIG( 0, 0, 0x20, 0x21, 0, 0, 0 );//��Ҫ
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

    //��Ҫ�� 4463��GDO2��GDO3������Ƶ���أ�  0X20 ,0X21 
    //����ʱ���룺 GDO2=1��GDO3=0
    //����ʱ���룺 GDO2=0��GDO3=1
    SI446X_GPIO_CONFIG( 0, 0, 0x20, 0x21, 0, 0, 0 );//��Ҫ 	
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
Function : ��ȡFIFO��Ϣ
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
���� : ��ȡоƬ���ж�״̬����Ҫ9 bytes��, �������Ӧ���ж�pending���Ĵ�������ο�si446x_API˵��2.2.12
���� : cmd--���ж�ָ��
	   buffer--����ж�״̬
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
���ܣ� 
���룺
�����
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
	SI446X_READ_RESPONSE( buffer, NUM_PROPS+1 ); // buffer[0]ΪCTS������ֵΪ0xFF��������Ĳ���������Properties
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
���� : 	����һ����������ǳ���FIFO���ȣ�64bytes���İ�����Ҫʹ��TX_FIFO_ALMOST_EMPTY(TFAE)�жϣ�
		FIFO���ʱ������FIFO��ֱ�����һ�����ݣ�����ȫ��д��FIFO���ο�AN633 si4x6x���ָ��
		��10.16��
���� : 	buffer--����������
		size--���鳤��
		channel--����ͨ��
		condition--������ɺ�ת�Ƶ���״̬
		size_of_FIFO--оƬ����FIFO������
		tx_threshold--FIFO�Ŀ��ÿռ�ﵽ������ʱ��оƬ������TFAE�жϣ�������NIRQ���ŵĵ�ƽ
��� : 	��
=================================================================================
*/
void SI446X_SEND_PACKET( u8 *txbuffer, u16 size, u8 channel, u8 condition,\
	u8 size_of_FIFO/*FIFO��СΪ 64 bytes*/, u8 tx_threshold/*��������Ϊ48*/)
{	
    u8 cmd[5];
	u16 tx_len = size;	
	u8 int_status_buf[9] = {0};	// ���оƬ���ж�״̬
	u8 i; 						// ������
	u8 flag_last_seg = 0;  		// �����Ƿ��Ѿ����͵����һ������
	u8 *txbuf0 = txbuffer; 		// ָ�������ݵ���ʼ��

	//variables for test
	u8 FIFO_info[3] = {0};
	//

	// �����FIFO
    SI446X_TX_FIFO_RESET();	

	// �̰��ͳ����Ĵ���Ӧ�ÿ��Ժϲ����пո�һ��
	if(tx_len <= size_of_FIFO){ // �̰�
		// ������������д��FIFO
		SI_CSN_LOW;
		SPI_ExchangeByte( WRITE_TX_FIFO );		
		while( size -- ) { SPI_ExchangeByte( *txbuffer++ ); }
		SI_CSN_HIGH;
		
		// ���÷������
		cmd[0] = START_TX;
		cmd[1] = channel;
		cmd[2] = condition;
		cmd[3] = (u8)(tx_len>>8);   // tx_len[15:8]
		cmd[4] = (u8)tx_len;		// tx_len[7:0]
		SI446X_CMD( cmd, 5 );   	// ��ʼ����

		/*
		while(SI4463_NIRQ!=0); 				// �ȴ�TFAE�ж�
		SI446X_INT_STATUS(int_status_buf);  // ��TFAE�ж�

		while(SI4463_NIRQ!=0); 				// �ȴ�������ɣ�PACKET_SENT���ж�
		SI446X_INT_STATUS(int_status_buf);  // ��PS�ж�
		*/
		
		do{
				SI446X_INT_STATUS(int_status_buf);		// ��ȡ�ж�״̬������ж�Pending
		}while((int_status_buf[4]&0x02) != 0x02);	// ��ѯTFAE�ж�
		
		do{
			SI446X_INT_STATUS(int_status_buf);		// ��ȡ�ж�״̬������ж�Pending
		}while( (int_status_buf[4]&0x20) != 0x20 ); // ��ѯPACKET_SENT�ж�
		
		// �ص���ready��״̬
		cmd[0] = CHANGE_STATE;
		cmd[1] = 0x03;
		SI446X_CMD( cmd, 2 );
	}else{ // ����
		// ���Ƚ�FIFO����
		SI_CSN_LOW;
		SPI_ExchangeByte( WRITE_TX_FIFO );		
		for(i=0;i<size_of_FIFO;i++) { SPI_ExchangeByte( *txbuffer++ ); }
		SI_CSN_HIGH;	
	
		SI446X_TX_FIFO_SPACE( FIFO_info ); // ��ʱFIFOʣ��ռ�Ӧ��Ϊ0
		
		// ���÷������
		cmd[0] = START_TX;
		cmd[1] = channel;
		cmd[2] = condition;
		cmd[3] = (u8)(tx_len>>8);   // tx_len[15:8]
		cmd[4] = (u8)tx_len;		// tx_len[7:0]
		SI446X_CMD( cmd, 5 );   	// ��ʼ����

		// �����һ�δ�������д��FIFO֮ǰ����ѯFIFO����(TX_FIFO_ALMOST_EMPTY)�ж�
		while(flag_last_seg!=1){ 	// ������������δȫ��д��FIFO			
			do{
				SI446X_INT_STATUS(int_status_buf);		// ��ȡ�ж�״̬������ж�Pending
			}while((int_status_buf[3]&0x02) != 0x02);	// ��ѯTFAE�ж�
			
			// ����������д��FIFO
			SI_CSN_LOW;
			SPI_ExchangeByte( WRITE_TX_FIFO );	 
			for(i=0;i<tx_threshold;i++){		 // ʵ����������������FIFO��������ܻ������
				if(txbuffer <= txbuf0+tx_len-1){ // txbuffer��û�г�����������ı߽�					
					SPI_ExchangeByte( *txbuffer++ );									
				}else{
					flag_last_seg = 1; 			 // ���һ�����ݶ�Ҳ����					
					break;
				}			
			}
			SI_CSN_HIGH;
			
			// ���ж�Pending
			//SI446X_INT_STATUS(int_status_buf); // ��һ�������Ƕ����
		}
		
		do{
			SI446X_INT_STATUS(int_status_buf);		// ��ȡ�ж�״̬������ж�Pending
		}while( (int_status_buf[4]&0x20) != 0x20 ); // ��ѯPACKET_SENT�ж�

		// �ص���ready��״̬
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
