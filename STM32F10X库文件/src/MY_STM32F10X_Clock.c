#include "MY_STM32F10X_Conf.h"

//函数作用：复位所有时钟寄存器（抽时间细看，好好理解下）
//参数说明：无参数
//测试结果：CHECK OK
//完成日期：14.02.26
void STM32_ClockReset(void)
{
	RCC->APB1RSTR=0x00000000;	//复位结束
	RCC->APB2RSTR=0x00000000;	//复位结束
	
	RCC->AHBENR=0x00000014;		//睡眠模式时闪存接口电路时钟和SRAM时钟开启，其它关闭
	RCC->APB1ENR=0x00000000;	//外设时钟关闭
	RCC->APB2ENR=0x00000000;	//外设时钟关闭
	RCC->CR|=1<<0;				//使能内部高速时钟HSI
	RCC->CFGR&=0xf8ff0000;		//复位MCO[2:0]、ADCPRE[1:0]、PPRE2[2:0]、PPRE1[2:0]、HPRE[3:0]、SWS[1:0]、SW[1:0]
	RCC->CR&=0xfef2ffff;		//复位PLLON、CSSON、HSEBYP、HSERDY、HSEON、
	RCC->CFGR&=0xff80ffff;		//复位USBPRE、PLLMUL[3:0]、PLLXTPRE、PLLSRC
	RCC->CIR=0x00000000;		//关闭所有中断：关闭PLL就绪中断、HSE就绪中断、HSI就绪中断、LSE就绪中断、LSI就绪中断，并且无HSE时钟失效产生的安全系统中断，无PLL上锁产生的时钟就绪中断，无外部4-16MHz振荡器产生的时钟就绪中断，无内部8MHzRC振荡器产生的时钟就绪中断，无外部32KHz振荡器产生的时钟就绪中断，无内部40KHzRC振荡器产生的时钟就绪中断
	
	//配置向量表
#ifdef VECT_TAB_RAM
	STM32_NVICSetVectorTable(NVIC_VectTab_RAM,0x0);
#else
	STM32_NVICSetVectorTable(NVIC_VectTab_FLASH,0x0);
#endif
}

//函数作用：时钟配置初始化
//参数说明：PllMultiply:PLL倍频数
//测试结果：CHECK OK
//完成日期：14.02.26
void STM32_ClockInit(u8 PllMultiply)
{
	u8 i=0;														//用于RCC->CFGR的中间处理
	STM32_ClockReset();											//复位并配置向量表
	RCC->CR|=1<<16;												//HSE振荡器开启
	while((RCC->CR&(1<<17))==0);								//等待外部振荡器准备就绪
	RCC->CFGR=(RCC->CFGR&(~(0xf<<4)))+(0x0<<4);					//AHB不分频，
	RCC->CFGR=(RCC->CFGR&(~(0x7<<8)))+(0x4<<8);					//APB1二分频
	RCC->CFGR=(RCC->CFGR&(~(0x7<<11)))+(0x0<<11);				//APB1二分频
	RCC->CFGR=(RCC->CFGR&(~(0xf<<18)))+((PllMultiply-2)<<18);	//配置PLL倍频
	RCC->CFGR|=1<<16;											//HSE时钟作为PLL输入时钟
	FLASH->ACR|=0x32;											//FLASH 2个延时周期（不懂？）
	
	RCC->CR|=1<<24;												//PLL使能
	while((RCC->CR&(1<<25))==0);								//等待PLL时钟就绪
	RCC->CFGR=(RCC->CFGR&(~(0x3<<0)))+(0x2<<0);					//选择PLL输出作为系统时钟
	do															//等待PLL输出选定为系统时钟成功
	{
		i=(RCC->CFGR>>2)&0x3;
	}while(i!=0x2);
}

//函数作用：外设时钟使能
//参数说明：AXB:外设区；RCC_AXBPeriph:外设时钟位宏定义（如：RCC_APB2Periph_AFIO）
//测试结果：NOT CHECK
//完成日期：14.03.01
void STM32_ClockEnable(AXB_TypeDef AXB,uint32_t RCC_AXBPeriph)
{
	if(AHB==AXB)
		RCC->AHBENR|=RCC_AXBPeriph;
	else if(APB1==AXB)
		RCC->APB1ENR|=RCC_AXBPeriph;
	else if(APB2==AXB)
		RCC->APB2ENR|=RCC_AXBPeriph;
		
}
