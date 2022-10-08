#include "MY_STM32F10X_Conf.h"

//函数作用：外部中断使用初始化
//参数说明：GPIOx为需要设置外部中断的端口号；GPIO_Pin:需要设置外部中断的引脚号；NVIC_Group:中断优先级分组；\
			NVIC_PreemptionPriority:抢占优先级；NVIC_SubPriority:响应优先级；ExIntWorkMode:外部中断触发方式。
//测试结果：CHECK OK
//完成日期：14.02.22
void STM32_ExInterruptInit(GPIO_TypeDef *GPIOx,u16 GPIO_Pin,u8 NVIC_Group,u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,ExIntWorkMode_TypeDef ExIntWorkMode)
{
	u8 count=0;												//存放引脚序号
	u8 NVIC_Channel=0;										//存放中断通道
	for(count=0;count<16;count++)							//得到引脚序号
		if((GPIO_Pin>>count)==1) break;
	if(count<5) NVIC_Channel=6+count;						//得到中断通道号
	else if(count<10) NVIC_Channel=23;
	else NVIC_Channel=40;
	if(count<8)												//引脚设为上下拉输入
		GPIOx->CRL=(GPIOx->CRL&(~(0xf<<(count*4))))+(0x8<<(count*4));
	else
		GPIOx->CRH=(GPIOx->CRH&(~(0xf<<((count-8)*4))))+(0x8<<((count-8)*4));
	
	NVIC->ISER[NVIC_Channel/32]|=1<<(NVIC_Channel%32);		//中断使能
	STM32_NVICInit(NVIC_Channel,NVIC_Group,NVIC_PreemptionPriority,NVIC_SubPriority);		//中断分组及优先级设置	
	RCC->APB2ENR|=0x01;										//使能io复用时钟
	if(ExIntWorkMode&INTWORKMODE_RISING)					//上升沿触发中断
	{
		GPIOx->ODR&=~GPIO_Pin;	   							//引脚下拉
		EXTI->RTSR=0x1<<count;								//设置上升沿触发中断
	}
	if(ExIntWorkMode&INTWORKMODE_FALLING)					//下降沿触发中断		
	{
		GPIOx->ODR|=GPIO_Pin;	   							//引脚上拉
		EXTI->FTSR=0x1<<count;								//设置下降沿触发中断
	}
	AFIO->EXTICR[count/4]=(AFIO->EXTICR[count/4]&(~(0xf<<((count%4)*4))))+((((u32)GPIOx-GPIOA_BASE)/0x400)<<((count%4)*4));		//把线上中断映射到GPIOx口中断
	EXTI->IMR=0x1<<count;									//开启线上中断
	EXTI->EMR=0x1<<count;									//开启线上事件 
}

///////////////////////////////////////////////中断处理函数/////////////////////////////////////////////////////////
void EXTI15_10_IRQHandler(void)								//外部中断处理函数
{
	STM32_Delay_ms(20);
	EXTI->PR=1<<12;
}

