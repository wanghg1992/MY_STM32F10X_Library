#include "MY_STM32F10X_Conf.h"
#include "math.h"

//函数作用：TIM4PWM使用初始化
//参数说明：TIMCLK:当前输入时钟频率；Freqence:输出频率，取值为1Hz~3.4MHz（测试所得）；PulseWidth:脉冲宽度，取值为0~100。
//					注意：若把Freqence改为浮点类型的话，估计最低频率可以达到0.01Hz
//测试结果：CHECK OK
//完成日期：14.02.21
//修改日期：15.9.29
void STM32_TIM4PWMInit(u32 TIMCLK,GPIOBPinx_TypeDef GPIOBPinx,u32 Freqence,u8 PulseWidth)	//以TIM4，PB口输出PWM，无重映射为例
{
	u32 k=0;
	RCC->APB1ENR|=1<<2;															//TIM4时钟使能
	RCC->APB2ENR|=1<<3;															//GPIOB时钟使能
	
	if(GPIOBPinx<8)																//对应引脚设置为复用推挽输出
		GPIOB->CRL=(GPIOB->CRL&(~(0xf<<(GPIOBPinx*4))))+(0xb<<(GPIOBPinx*4));
	else
		GPIOB->CRH=(GPIOB->CRH&(~(0xf<<((GPIOBPinx-8)*4))))+(0xb<<((GPIOBPinx-8)*4));
	GPIOB->ODR|=1<<GPIOBPinx;													//引脚上拉

	//设置脉冲频率，依据公式：Freqence=TIMCLK/(PSC*ARR)
	k=TIMCLK/Freqence;															
	if(k<=65535)
	{
		TIM4->PSC=0;
		TIM4->ARR=k;
	}
	else
	{
//		for(i=1;i<k/2;i++)
//		{
//			if(k<=i*i&&k>=(i-1)*(i-1))
//			{
//				if((k-(i-1)*(i-1))>(i*i-k))
//				{	
//					TIM4->PSC=i-1;
//					TIM4->ARR=i;
//				}
//				else
//				{
//					TIM4->PSC=i-2;
//					TIM4->ARR=i-1;
//				}
//			}
//		}
		TIM4->ARR=sqrt(k);
		TIM4->PSC=TIM4->ARR-1;
	}
	
	//确定占空比、选择PWM模式、预装载使能
	switch(GPIOBPinx)
	{
		case 6:
			TIM4->CCR1=TIM4->ARR*PulseWidth/100;				//确定占空比（0~100），此处运算先乘再除，以免数据丢失，结果为零
			TIM4->CCMR1=(TIM4->CCMR1&(~(7<<4)))+(0x6<<4);		//PWM1模式
			TIM4->CCMR1|=1<<3;									//预装载使能
			break;		
		case 7:
			TIM4->CCR2=TIM4->ARR*PulseWidth/100;
			TIM4->CCMR1=(TIM4->CCMR1&(~(7<<12)))+(0x6<<12);		//PWM1模式
			TIM4->CCMR1|=1<<11;									//预装载使能
			break;
		case 8:
			TIM4->CCR3=TIM4->ARR*PulseWidth/100;
			TIM4->CCMR2=(TIM4->CCMR2&(~(7<<4)))+(0x6<<4);		//PWM1模式
			TIM4->CCMR2|=1<<3;									//预装载使能
			break;
		case 9:
			TIM4->CCR4=TIM4->ARR*PulseWidth/100;
			TIM4->CCMR2=(TIM4->CCMR2&(~(7<<12)))+(0x6<<12);		//PWM1模式
			TIM4->CCMR2|=1<<11;									//预装载使能
			break;
		default:;
	}
	
	TIM4->CCER=(TIM4->CCER&(~(0x3<<((GPIOBPinx-PB6)*4))))+(0x1<<((GPIOBPinx-PB6)*4));	//OCx高电平有效，输出使能
	TIM4->CR1|=0x1;																		//使能计数器
}
//函数作用：TIMxPWM使用初始化(没有重映射)
//参数说明：TIMCLK:当前输入时钟频率；TIMx：定时器；TIMChannelx:定时器通道；
//					Freqence:输出频率，取值为1Hz~3.4MHz（测试所得）；DutyRatio:PWM占空比，取值为0~1.0。
//					注意：若把Freqence改为浮点类型的话，估计最低频率可以达到0.01Hz
//测试结果：CHECK OK
//完成日期：15.10.10
void STM32_TIMxPWMInit(u32 TIMCLK,TIM_TypeDef* TIMx,TIMChannelx_TypeDef TIMChannelx,u32 Freqence,float DutyRatio)	//以TIM4，PB口输出PWM，无重映射为例
{
	u32 k=0,TIMCount,GPIOPinx;
	GPIO_TypeDef *GPIOx;
	
	//提取定时器需要、引脚序号、IO口组信息，并使能相应定时器和IO口时钟
	switch((int)TIMx)
	{
		case TIM1_BASE:									//还没用
			TIMCount=1;										//得到定时器序号
			GPIOPinx=8+TIMChannelx;				//得到通道对应引脚序号
			GPIOx=GPIOA;									//得到用到的引脚组
			RCC->APB2ENR|=1<<11;					//TIM1时钟使能
			RCC->APB2ENR|=1<<2;						//GPIOA时钟使能
			break;
		case TIM2_BASE:
			TIMCount=2;										//得到定时器序号
			GPIOPinx=0+TIMChannelx;				//得到通道对应引脚序号
			GPIOx=GPIOA;									//得到用到的引脚组
			RCC->APB1ENR|=1;							//TIM2时钟使能
			RCC->APB2ENR|=1<<2;						//GPIOA时钟使能
			break;
		case TIM3_BASE:
			TIMCount=3;										//得到定时器序号
			if(TIMChannelx<2)
			{
				GPIOPinx=6+TIMChannelx;	//得到通道对应引脚序号
				GPIOx=GPIOA;								//得到用到的引脚组
			}
			else
			{
				GPIOPinx=TIMChannelx-2;			//得到通道对应引脚序号
				GPIOx=GPIOB;								//得到用到的引脚组				
			}
			RCC->APB1ENR|=1<<1;						//TIM3时钟使能
			RCC->APB2ENR|=3<<2;						//GPIOA\GPIOB时钟使能
			break;
		case TIM4_BASE:
			TIMCount=4;										//得到定时器序号
			GPIOPinx=6+TIMChannelx;		//得到通道对应引脚序号
			GPIOx=GPIOB;									//得到用到的引脚组
			RCC->APB1ENR|=1<<2;						//TIM4时钟使能
			RCC->APB2ENR|=1<<3;						//GPIOB时钟使能			
			break;
		case TIM5_BASE:									//不重映射的话，与TIM2所用引脚重合。现在不用
			TIMCount=5;										//得到定时器序号
			break;
//		case TIM6_BASE:		//定时器6和定时器7不能用于产生PWM
//			TIMCount=6;
//			break;
//		case TIM7_BASE:
//			TIMCount=7;
//			break;
		case TIM8_BASE:
			TIMCount=8;										//得到定时器序号
			GPIOPinx=6+TIMChannelx;		//得到通道对应引脚序号
			GPIOx=GPIOC;									//得到用到的引脚组
			RCC->APB2ENR|=1<<13;					//TIM8时钟使能
			RCC->APB2ENR|=1<<4;						//GPIOC时钟使能
			break;
	}
		
	//初始化IO口
	if(GPIOPinx<8)																//对应引脚设置为复用推挽输出
		GPIOx->CRL=(GPIOx->CRL&(~(0xf<<(GPIOPinx*4))))+(0xb<<(GPIOPinx*4));
	else
		GPIOx->CRH=(GPIOx->CRH&(~(0xf<<((GPIOPinx-8)*4))))+(0xb<<((GPIOPinx-8)*4));
	GPIOx->ODR|=1<<GPIOPinx;													//引脚上拉

	//设置脉冲频率，依据公式：Freqence=TIMCLK/(PSC*ARR)
	k=TIMCLK/Freqence;															
	if(k<=65535)
	{
		TIMx->PSC=0;
		TIMx->ARR=k;
	}
	else
	{
		TIMx->ARR=sqrt(k);
		TIMx->PSC=TIMx->ARR-1;
	}
	
	//确定占空比、选择PWM模式、预装载使能
	switch(TIMChannelx)
	{
		case TIMChannel1:
			TIMx->CCR1=TIMx->ARR*DutyRatio;				//确定占空比（0~100），此处运算先乘再除，以免数据丢失，结果为零
			TIMx->CCMR1=(TIMx->CCMR1&(~(7<<4)))+(0x6<<4);		//PWM1模式
			TIMx->CCMR1|=1<<3;									//预装载使能,这个有没有必要，回头查查
			break;		
		case TIMChannel2:
			TIMx->CCR2=TIMx->ARR*DutyRatio;
			TIMx->CCMR1=(TIMx->CCMR1&(~(7<<12)))+(0x6<<12);		//PWM1模式
			TIMx->CCMR1|=1<<11;									//预装载使能
			break;
		case TIMChannel3:
			TIMx->CCR3=TIMx->ARR*DutyRatio;
			TIMx->CCMR2=(TIMx->CCMR2&(~(7<<4)))+(0x6<<4);		//PWM1模式
			TIMx->CCMR2|=1<<3;									//预装载使能
			break;
		case TIMChannel4:
			TIMx->CCR4=TIMx->ARR*DutyRatio;
			TIMx->CCMR2=(TIMx->CCMR2&(~(7<<12)))+(0x6<<12);		//PWM1模式
			TIMx->CCMR2|=1<<11;									//预装载使能
			break;
		default:;break;
	}
	
	TIMx->CCER=(TIMx->CCER&(~(0x3<<(TIMChannelx*4))))+(0x1<<(TIMChannelx*4));	//OCx高电平有效，输出使能
	if(8==TIMCount||1==TIMCount)
		TIMx->BDTR=0X8000;		//注意，TIM8加的，与通用定时器不同，TIM1是否要用待检验
//	if(1==TIMCount)
//	{
//		//TIMx->BDTR=0X4000;
//		
//	}
	TIMx->CR1|=0x1;					//使能计数器	
}

//函数作用：TIMx改变PWM占空比(除了正常模式，也可适用于重映射)
//参数说明：TIMx:定时器；TIMChannelx:定时器通道；
//					DutyRatio:PWM占空比，取值为0~1.0。
//测试结果：CHECK OK
//完成日期：15.10.10
void STM32_TIMxSetPWMDuty(TIM_TypeDef* TIMx,TIMChannelx_TypeDef TIMChannelx,float DutyRatio)	//以TIM4，PB口输出PWM，无重映射为例
{
	switch(TIMChannelx)
	{
		case TIMChannel1:
			TIMx->CCR1=TIM8->ARR*DutyRatio;				//确定占空比（0~100），此处运算先乘再除，以免数据丢失，结果为零
			break;		
		case TIMChannel2:
			TIMx->CCR2=TIM8->ARR*DutyRatio;
			break;
		case TIMChannel3:
			TIMx->CCR3=TIM8->ARR*DutyRatio;
			break;
		case TIMChannel4:
			TIMx->CCR4=TIM8->ARR*DutyRatio;
			break;
		default:;break;
	}
}
