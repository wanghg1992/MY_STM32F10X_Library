#include "MY_STM32F10X_Conf.h"

//函数作用：RTC使用初始化
//参数说明：无参数
//测试结果：CHECK OK
//完成日期：14.02.20
u8 STM32_RTCInit(void)
{
	u8 i=0;
	if(BKP->DR1!=0x1234)
	{
		RCC->APB1ENR|=0x3<<27;								//使能电源接口时钟和备份接口时钟
		PWR->CR|=1<<8;										//允许写入RTC和后备寄存器
		RCC->BDCR|=1<<16;									//复位整个备份区
		RCC->BDCR&=~(1<<16);								//整个备份区复位结束
		RCC->BDCR|=1<<0;									//外部低速振荡器使能，此句待验证
		while((RCC->BDCR&1<<1)==0&&((++i)<250)) 			//等待外部时钟就绪
			STM32_Delay_ms(10);	
		if(i>=250) return 1;								//初始化时钟失败，晶振有问题
		
		RCC->BDCR=(RCC->BDCR&(~(0x3<<8)))+(0x1<<8);			//选择LSE作为RTC时钟
		RCC->BDCR|=1<<15;									//RTC时钟使能
		while((RTC->CRL&(1<<5))==0);						//等待RTC寄存器写操作完成
		while((RTC->CRL&(1<<3))==0);						//等待RTC寄存器同步
		RTC->CRH|=1<<0;										//允许RTC秒中断
		while((RTC->CRL&(1<<5))==0);						//等待RTC寄存器写操作完成
		RTC->CRL|=1<<4;										//允许配置RTC_CNT、RTC_ALR、RTC_PRL寄存器
		while((RTC->CRL&(1<<5))==0);						//等待RTC寄存器写操作完成
		RTC->PRLH=0x0000;									//对RTCCLK32768分频，1Hz
		RTC->PRLL=32767;															
		RTC->CNTH=0x0000;									//设置RTC计数器初值
		RTC->CNTL=0xaaaa;
		RTC->CRL&=~(1<<4);									//配置RTC_CNT、RTC_ALR、RTC_PRL寄存器结束，并更新数据
		while((RTC->CRL&(1<<5))==0);						//等待RTC寄存器写操作完成
		BKP->DR1=0x1234;									//第一次操作标志
	}
	else
	{
		while((RTC->CRL&(1<<3))==0);						//等待RTC寄存器同步
		RTC->CRH|=1<<0;										//允许RTC秒中断
		while((RTC->CRL&(1<<5))==0);						//等待RTC寄存器写操作完成
	}
	STM32_NVICInit(RTC_IRQn,NVIC_GROUP,0,0);					//中断分组及优先级设置
	return 0;												//初始化成功
}

///////////////////////////////////////////////中断处理函数/////////////////////////////////////////////////////////
void RTC_IRQHandler(void)
{
	if((RTC->CRL&1<<0)!=0)									//秒中断
	{
		STM32_Delay_us(50);									//延时，等计数器值稳定，经测试至少应等待30us
	}
	RTC->CRL&=0xfffe;										//清除秒中断标志
	while((RTC->CRL&(1<<5))==0);							//等待RTC寄存器写操作完成
}
