#include "stm32f10x_pwr.h"
#include "stm32f10x_rcc.h"
#include "Lowpower.h"


// void  EnterSleep(void )
// {
// 	PWR_BackupAccessCmd(ENABLE);
// 		*(u32*)0xE000ED10 &= ~(1<<2);  	//SLEEPDEEP	=0  ?????,???????????(?????)??
// 		*(u32*)0xE000ED10 &= ~(1<<1);  	//SLEEPONEXIT =0     
// 		PWR->CR |= (1<<3);		//CSBF	??4??????	   
// 		PWR->CR |= (1<<2);		//CWUF
// 		PWR->CR &= ~(1<<1);	//PDDS
// 		PWR->CR &= ~(1<<0);	//LPDS	
// 		__WFI();
// 	
// }

void PWR_EnterSLEEPMode(u32 SysCtrl_Set, u8 PWR_SLEEPEntry)//(0,1)
{
    if (SysCtrl_Set)
        *(vu32 *) SCB_SysCtrl |= (1<<1);//SysCtrl_SLEEPONEXIT_Set;    // Set SLEEPONEXIT
    else
        *(vu32 *) SCB_SysCtrl &= ~(1<<1);//~SysCtrl_SLEEPONEXIT_Set;  // Reset SLEEPONEXIT

    *(vu32 *) SCB_SysCtrl &=~(1<<2); //~SysCtrl_SLEEPDEEP_Set;    // Clear SLEEPDEEP bit
    if(PWR_SLEEPEntry == 1)//PWR_SLEEPEntry_WFI)            // Select SLEEP mode entry
        __WFI();                                        // Request Wait For Interrupt
    else
        __WFE();                                        // Request Wait For Event
}

















