#include "delay.h"
/*******************************************************************************
 配置延时（采用计数）
*******************************************************************************/

void  delay (u32 nCount)
{
  for(; nCount != 0; nCount--);
}

/*******************************************************************************
  采用SYSTICK 延时所需的 延时函数初始化
  延时函数实质：进入函数后启动定时计数，计数值达到0后标志位值1
                1个时钟计数值加1，合理设置计数初值可以定时1US或几US等
				通过查询的方式来判断计时是否完成
*******************************************************************************/

static u8  fac_us=0;//us延时倍乘数
static u16 fac_ms=0;//ms延时倍乘数
void delay_init(void)	 
{ 
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//选择外部时钟  HCLK/8
	fac_us=SystemCoreClock_8M/8000000;	//为系统时钟的1/8
	fac_ms=(u16)fac_us*1000;//非ucos下,代表每个ms需要的systick时钟数 
}
		    						   
void delay_us(u32 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=nus*fac_us; //时间加载	  		 
	SysTick->VAL=0x00;        //清空计数器
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //开始倒数	 
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//等待时间到达   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //关闭计数器
	SysTick->VAL =0X00;       //清空计数器	 
}

//延时n个ms
//注意nms的范围
//SysTick->LOAD为24位寄存器,所以,最大延时为:
//n ms<=0xffffff*8*1000/SYSCLK
//SYSCLK单位为Hz,nms单位为ms
//对72M条件下,nms<=1864 
void delay_ms(u16 nms)
{	 		  	  
	u32 temp;		   
	SysTick->LOAD=(u32)nms*fac_ms;//时间加载(SysTick->LOAD为24bit)
	SysTick->VAL =0x00;           //清空计数器
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //开始倒数  
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//等待时间到达   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //关闭计数器
	SysTick->VAL =0X00;       //清空计数器	  	    
} 
