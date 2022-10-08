#include "MY_STM32F10X_Conf.h"
#include "math.h"

//函数作用：定时器使用初始化
//参数说明：Timex:定时器，arr:计数器自动重装值，psc定时器时钟预分频值
//测试结果：CHECK OK
//完成日期：14.02.14
//更新时间：15.10.10
void STM32_TimerInit(TIM_TypeDef *TIMx,u16 arr,u16 psc)				
{
	uc TimeCount=0;																			//存放定时器的编号
	if((ui)TIMx>=TIM2_BASE&&(ui)TIMx<=TIM7_BASE)			//定时器2~7
	{
		TimeCount=((ui)TIMx-TIM2_BASE)/0x400+2;					//计算定时器编号
		RCC->APB1ENR|=1<<(TimeCount-2);										//使能定时器时钟
		TIMx->ARR=arr;																		//设置计数器自动重装值
		TIMx->PSC=psc-1;																	//设置时钟预分频寄存器
		TIMx->DIER|=((1<<0)+(1<<6));											//允许更新中断和触发中断
		TIMx->CR1|=1<<0;																	//使能计数器，DIR为0？？？
		switch(TimeCount)
		{
			case 2:STM32_NVICInit(TIM2_IRQn,2,1,3);break;									//中断分组及优先级设置  改定时器时，这个地方也要改
			case 3:STM32_NVICInit(TIM3_IRQn,2,1,3);break;									//中断分组及优先级设置  改定时器时，这个地方也要改
			case 4:STM32_NVICInit(TIM4_IRQn,2,1,3);break;									//中断分组及优先级设置  改定时器时，这个地方也要改
			case 5:STM32_NVICInit(TIM5_IRQn,2,1,3);break;									//中断分组及优先级设置  改定时器时，这个地方也要改
			case 6:STM32_NVICInit(TIM6_IRQn,2,1,3);break;									//中断分组及优先级设置  改定时器时，这个地方也要改
			case 7:STM32_NVICInit(TIM7_IRQn,2,1,3);break;									//中断分组及优先级设置  改定时器时，这个地方也要改
			default:;break;
		}
	}
}

//函数作用：定时器使用初始化
//参数说明：Timex:定时器，us：要定时的us数
//测试结果：CHECK OK
//完成日期：15.10.10
//理论可延时范围：1/72us到59.65232355555556s，实际延时范围：1~59652323us
void STM32_TimerInitUs(TIM_TypeDef *TIMx,u32 us)				
{
	u32 count=us*72;
	if(count<=65535)
	{
		STM32_TimerInit(TIMx,count,1);		
	}
	else
	{
		STM32_TimerInit(TIMx,sqrt(count),sqrt(count));				
	}	
}

//函数作用：测试函数的执行时间
//参数说明：Timex:定时器，Function：要测试的函数，count:测试的次数（多次测试取执行时间最大值）。
//					返回值为测试函数的执行时间，单位为us。
//测试结果：CHECK OK
//完成日期：15.12.06
//注意：必须实现相应定时器的中断处理函数，并使FunctionTimeMeasureIntCount在中断中加1，如下：
///*
//void TIMx_IRQHandler(void)
//{ 		    		  			    
//	if(TIMx->SR&0X0001)			//溢出中断
//	{
//		FunctionTimeMeasureIntCount++;
//	}				   
//	TIMx->SR&=~(1<<0);			//清除中断标志位
//}
//*/
static u32 FunctionTimeMeasureIntCount=0;		//需要在所用到的中断中加1
u32 STM32_MeasureFuncExctTimeByTimerx(TIM_TypeDef *TIMx,void(*Function)(void),u8 count)
{
	u8 i=0;
	u32 maxtime[2]={0,0},time=0;
	STM32_TimerInit(TIMx,65535,72);			//1us计数一次，65535us中断一次
//	delay_ms(2);											//实践证明至少要延时1ms，等设置稳定后，后面测量才不会出问题
	STM32_Delay_ms(2);
	for(i=0;i<count;i++)
	{
		FunctionTimeMeasureIntCount=0;
		time=TIMx->CNT;
		Function();
		time=65535*FunctionTimeMeasureIntCount+TIMx->CNT-time;
		if(maxtime[0]<time)				//记录最大和次大的值
		{
			maxtime[1]=maxtime[0];
			maxtime[0]=time;
		}
		else if(maxtime[1]<time)
		{
			maxtime[1]=time;
		}
	}
	TIMx->DIER&=~(((1<<0)+(1<<6)));													//不允许更新中断和触发中断
	TIMx->CR1&=~(1<<0);																			//除能计数器，DIR为0？？？
	
	if(count>1)														//多次测量
	{
		if(maxtime[0]*1.0/maxtime[1]<2.0)		//说明最大的值正常，返回最大值
			return maxtime[0];
		else																//说明最大值太大，可能不正常，返回次大值
			return maxtime[1];
	}
	else
		return maxtime[0];
}


///////////////////////////////////////////////中断处理函数/////////////////////////////////////////////////////////
//定时器中断服务程序 
/*用哪个初始化哪个，模板：
void TIMx_IRQHandler(void)
{ 		    		  			    
	if(TIMx->SR&0X0001)			//溢出中断
	{}				   
	TIMx->SR&=~(1<<0);			//清除中断标志位
}
*/

void TIM6_IRQHandler(void)		//用于测量函数执行时间，见STM32_MeasureFuncExctTimeByTimerx的说明
{ 		    		  			    
	if(TIM6->SR&0X0001)			//溢出中断
	{
		FunctionTimeMeasureIntCount++;
	}				   
	TIM6->SR&=~(1<<0);			//清除中断标志位
}

void TIM2_IRQHandler(void)
{ 		    		  			    
	if(TIM2->SR&0X0001)			//溢出中断
	{}				   
	TIM2->SR&=~(1<<0);			//清除中断标志位
}
