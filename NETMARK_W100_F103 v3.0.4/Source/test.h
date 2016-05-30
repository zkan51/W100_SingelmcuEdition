#ifndef __TEST_H
#define __TEST_H

#define Level_PB12 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)
#define Level_PB14 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14)
void ProgramSelector(void);
void CurrentTest(void);
void EjectTest(void);

#endif
