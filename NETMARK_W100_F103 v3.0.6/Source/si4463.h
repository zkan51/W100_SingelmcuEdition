#ifndef __SI4463_H__
#define __SI4463_H__
#include "stm32f10x.h"
#include "si446x_defs.h" 
#include "spi.h"

#include "radio_config_Si4463_GMSK_v0.0.6.1.h" // TX_FIFO门限设置为40
#include "radio_config_Si4060_162025M.h"

#define SI4463
//------NIRQ, SDN, CSN(即NSEL)在si4463.c的SI4463_IOSET()中配置------
//4463中断脚
#define	 SI4463_NIRQ     GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13)
//4463复位脚
#define  SI_SDN_LOW      ;
#define  SI_SDN_HIGH     ;
//4463SPI片选脚
#define  SI_CSN_LOW      GPIO_ResetBits(GPIOA, GPIO_Pin_5)
#define  SI_CSN_HIGH     GPIO_SetBits(GPIOA, GPIO_Pin_5)

//-------SCK, SDI, SDO在spi.c里配置------
//主机模式下：STM32SPI和4463时钟脚
#define  SI_SCK_LOW      GPIO_ResetBits(GPIOB, GPIO_Pin_0)
#define  SI_SCK_HIGH     GPIO_SetBits(GPIOB, GPIO_Pin_0)
//主机模式下：STM32SPI数据输出，4463输入
#define  SI_SDO_LOW      GPIO_ResetBits(GPIOA, GPIO_Pin_6)
#define  SI_SDO_HIGH     GPIO_SetBits(GPIOA, GPIO_Pin_6)
//主机模式下：STM32SPI数据输入，4463输出
#define  SI4463_SDI      GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7)

#define  PACKET_LENGTH   0 // 帧长可变

//函数声明
u8 SPI_ExchangeByte(u8 TxData);
void testinf(u8 Rf_Rec_Buf[]);
void SI446X_CMD(u8 *cmd, u8 cmdsize);
void SI4463_Init(void);
void SI446X_RESET(void);
void SI446X_CONFIG_INIT(void);
void SI446X_SINGLE_FREQ_CONFIG_INIT(void);
void SI446X_PART_INFO(u8 *buffer);
void SI446X_FUNC_INFO(u8 *buffer);
void SI446X_RX_FIFO_RESET(void);
void SI446X_TX_FIFO_RESET(void);
void SI446X_SEND_PACKET(u8 *txbuffer, u16 size, u8 channel, u8 condition, u8 size_of_FIFO, u8 tx_threshold);
void SI446X_START_TX( u8 *buffer );
void SI446X_WRITE_TX_FIFO(void);
void SI446X_INT_STATUS( u8 *buffer );
void SI446X_GET_PROPERTY(u8 GROUP, u8 NUM_PROPS, u8 START_PROP, u8* buffer );
void SI4463_IOSET(void);	
void SI4463_OFF(void);

#endif
