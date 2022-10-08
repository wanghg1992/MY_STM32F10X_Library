#include "MY_STM32F10X_Conf.h"
#include "usart.h"


static u16 count_us=0,count_ms=0;						//存放ms、us倍乘数

//函数作用：延时函数使用初始化
//参数说明：SystemClk:当前系统时钟
//测试结果：CHECK OK
//完成日期：14.02.23
void STM32_DelayInit(u32 SystemClk)
{
	SysTick->CTRL&=~(1<<2);							//选择外部时钟源（STCLK）
	count_us=SystemClk/8/1000000;				//1us所计数
	count_ms=1000*count_us;							//1ms所计数
}

#ifdef USE_SYSTICK_INT
void STM32_SysTickInterruptInit(u32 SystemClk,u32 nus)	//nus<1864135.11111111111us
{
	SysTick->CTRL&=~(1<<2);										//选择外部时钟源（STCLK）
	SysTick->LOAD=SystemClk/8/1000000*nus;		//n us所计数
	SysTick->CTRL|=0x3;												//定时器连续工作模式，并使能溢出中断
}
#endif

void STM32_SysTickInit(u32 SystemClk,float HeartBeatFreq)				//HeartBeatFreq>0.536441802978515625Hz
{
	STM32_DelayInit(SystemClk);
#ifdef USE_SYSTICK_INT
	STM32_SysTickInterruptInit(SystemClk,1.0*1000000/HeartBeatFreq);
#endif
}

#ifdef USE_SYSTICK_INT
//函数作用：微秒延时
//参数说明：nus:要延时的微秒数
//测试结果：CHECK OK
//完成日期：15.12.05
void STM32_Delay_us(u32 nus)
{
	u32 ticks=nus*count_us;										//用于一次性读取CTRL,以免两次读取中间CTRL的值变化
	u32 told,tnow,tcnt=0;
	tnow=SysTick->VAL;
	while(tcnt<ticks)
	{
		told=tnow;
		tnow=SysTick->VAL;
		if(tnow<=told)	//计数器还没溢出
		{
			tcnt+=told-tnow;
		}
		else
		{
			tcnt+=told+SysTick->LOAD-tnow;
		}
	}
}
//void delay_us(u32 nus)
//{		
//	u32 ticks;
//	u32 told,tnow,tcnt=0;
//	u32 reload=SysTick->LOAD;	//LOAD的值	    	 
//	ticks=nus*fac_us; 			//需要的节拍数	  		 
//	tcnt=0;
//	OSSchedLock();				//阻止ucos调度，防止打断us延时
//	told=SysTick->VAL;        	//刚进入时的计数器值
//	while(1)
//	{
//		tnow=SysTick->VAL;	
//		if(tnow!=told)
//		{	    
//			if(tnow<told)tcnt+=told-tnow;//这里注意一下SYSTICK是一个递减的计数器就可以了.
//			else tcnt+=reload-tnow+told;	    
//			told=tnow;
//			if(tcnt>=ticks)break;//时间超过/等于要延迟的时间,则退出.
//		}  
//	};
//	OSSchedUnlock();			//开启ucos调度 									    
//}

//函数作用：毫秒延时
//参数说明：nms:要延时的毫秒数
//测试结果：CHECK OK
//完成日期：15.12.05
void STM32_Delay_ms(u32 nms)				//这样可能可以几乎无限延时了
{
	u32 ticks=nms*count_ms;										//用于一次性读取CTRL,以免两次读取中间CTRL的值变化
	u32 told,tnow,tcnt=0;
	tnow=SysTick->VAL;
	while(tcnt<ticks)
	{
		told=tnow;
		tnow=SysTick->VAL;
		if(tnow<=told)	//计数器还没溢出
		{
			tcnt+=told-tnow;
		}
		else
		{
			tcnt+=told+SysTick->LOAD-tnow;
		}
	}
}

//函数作用：函数执行时间测量
//参数说明：Function为待测函数名；count为测试次数（取其中最大值）。返回值为被测函数的执行时间，单位为us
//测试结果：CHECK OK
//完成日期：15.12.06
//注意：对延时函数的测量结果有1us左右的误差。原因可能有以下几个：
//			1，延时函数延时不准
//			2，延时函数中的初始化消耗了时间
//			3，测量有误差
//	由于误差较小，不好处理，不作考虑，不作处理
static u32 FuncExctTimeMeasureIntCount=0;		//需要在所用到的中断中加1
u32 STM32_MeasureFuncExctTimeBySysTick(void(*Function)(void),u8 count)	//返回值小于477218588.444444us
{
	u8 i=0;
	u32 maxtcnt[2]={0,0},told=0,tnew=0,tcnt=0;
	for(i=0;i<count;i++)
	{
		FuncExctTimeMeasureIntCount=0;
		told=SysTick->VAL;
		Function();
		tnew=SysTick->VAL;
		tcnt=SysTick->LOAD*FuncExctTimeMeasureIntCount+told-tnew;			//-SysTick->VAL须放到前面，减小测量误差，否则系统综合正向误差能达到1us。还是不行，用原法吧。不用原法了，尽量减少误差
		if(maxtcnt[0]<tcnt)				//记录最大和次大的值
		{
			maxtcnt[1]=maxtcnt[0];
			maxtcnt[0]=tcnt;
		}
		else if(maxtcnt[1]<tcnt)
		{
			maxtcnt[1]=tcnt;
		}
	}
	
	if(count>1)														//多次测量
	{
		if(maxtcnt[0]*1.0/maxtcnt[1]<2.0)		//说明最大的值正常，返回最大值
			return maxtcnt[0]/count_us;			//-1，应该是由于指令误差，实验测试会有1us的正向误差,为什么有1us，我暂时不清楚。不用减1了，已找到误差。不行，还是不能完全去掉误差，用原法吧。不用原法了，尽量减少误差
		else																//说明最大值太大，可能不正常，返回次大值
			return maxtcnt[1]/count_us;
	}
	else
		return maxtcnt[0]/count_us;
}

void SysTick_Handler(void)
{
	if(SysTick->CTRL&(1<<16))						//计数器溢出中断
	{
		FuncExctTimeMeasureIntCount++;		//不能删
	}
}

#else
//函数作用：微秒延时
//参数说明：nus:要延时的微秒数
//测试结果：CHECK OK
//完成日期：14.02.23
void STM32_Delay_us(u32 nus)
{
	u32 i=0;										//用于一次性读取CTRL,以免两次读取中间CTRL的值变化
	SysTick->LOAD=nus*count_us;						//计数器重装值（以计时nus）
	SysTick->VAL=0;									//清零倒计数计数器的值
	SysTick->CTRL|=1<<0;							//使能滴答定时器，开始计数
	
	//while((SysTick->CTRL&0x1)&&((SysTick->CTRL>>16)&0x1)==0);		//等待计数器计数到零（此做法有问题）
	do
	{
		i=SysTick->CTRL;
	}
	while((i&0x01)&&!(i&(1<<16)));					//等待时间到达 
	
	SysTick->CTRL&=~(1<<0);							//关闭计数器
	SysTick->VAL=0;									//清空计数器的值
}

//函数作用：毫秒延时
//参数说明：nms:要延时的毫秒数
//测试结果：CHECK OK
//完成日期：14.02.23
void STM32_Delay_ms(u32 nms)
{
	u32 i=0;										//用于一次性读取CTRL,以免两次读取中间CTRL的值变化
	SysTick->LOAD=nms*count_ms;						//计数器重装值（以计时nus）
	SysTick->VAL=0;									//清零倒计数计数器的值
	SysTick->CTRL|=1<<0;							//使能滴答定时器，开始计数
	
	//while((SysTick->CTRL&0x1)&&((SysTick->CTRL>>16)&0x1)==0);		//等待计数器计数到零（此做法有问题）
	do
	{
		i=SysTick->CTRL;
	}
	while((i&0x01)&&!(i&(1<<16)));					//等待时间到达   

	SysTick->CTRL&=~(1<<0);							//关闭计数器
	SysTick->VAL=0;									//清空计数器的值
}
#endif





