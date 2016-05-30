#ifndef __MSG_SLOT_H
#define	__MSG_SLOT_H

#include "stm32f10x.h"
#define CNT_SLOT         (256*12)
#define CNT_GAUSS_INTER         (12)
#define MSG_18                  (0x00)
#define MSG_24A                 (0x01)
#define MSG_24B                (0x02)
#define AIS_L                   (0x01)
#define AIS_H                   (0x02)

u16 one_byte_crc(u8 msg,u16 reg);
u16 msg_crc(u8 *ptr,u8 num);
void bit_to_byte_msg(u8 *msg_ptr);
u16 bit_stuffing(void);
void nrzi(void);


void msg_send1(void);
void msg_send2(void);
void msg18_generating(void);
void send_on(void);

#endif /* __MSG_SLOT_H */

