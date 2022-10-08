#include "MY_STM32F10X_Conf.h"
#include "usart.h"

///////////////////////////////////////////////////////////////////////////////单次转换为例
//函数作用：ADC1使用初始化
//参数说明：GPIO_Pin:要使用ADC测电压的引脚
//测试结果：CHECK OK
//完成日期：14.02.16
//更新时间：15.10.14
void STM32_ADC1Init(ADCChannelx_TypeDef ADCChannelx,SimpleTime_TypeDef SimpleTime)			//此处以ADC1，单次转换为例
{
	u32 GPIOPinx;
	GPIO_TypeDef *GPIOx;
	//uc count=0;	
	
	//暂时没用通道16和通道17
	if(ADCChannelx<ADCChannel8)
	{
		GPIOx=GPIOA;
		GPIOPinx=ADCChannelx;
		RCC->APB2ENR|=1<<2;												//端口A时钟使能
	}
	else if(ADCChannelx<10)
	{
		GPIOx=GPIOB;
		GPIOPinx=ADCChannelx-ADCChannel8;
		RCC->APB2ENR|=1<<3;												//端口B时钟使能
	}
	else if(ADCChannelx<ADCChannel16)
	{
		GPIOx=GPIOC;
		GPIOPinx=ADCChannelx-ADCChannel10;
		RCC->APB2ENR|=1<<4;												//端口C时钟使能
	}
	
	if(GPIOPinx<8)														//GPIO_Pin引脚设置为模拟输入
	{
		GPIOx->CRL&=~(0x0f<<(GPIOPinx*4));
	}
	else
	{
		GPIOx->CRH&=~(0x0f<<((GPIOPinx-8)*4));
	}
	
	RCC->APB2ENR|=1<<9;											//ADC1时钟使能
	RCC->APB2RSTR|=1<<9;										//ADC1复位
	RCC->APB2RSTR&=~(1<<9);									//ADC1停止复位
	RCC->CFGR=(RCC->CFGR&(~(0x3<<14)))+(0x2<<14);					//ADC预分频：6分频
	
	ADC1->CR2|=1<<20;												//使用外部事件触发规则转换
	ADC1->CR2|=7<<17;												//选择软件触发转换方式
	ADC1->CR2&=~(1<<11);										//右对齐
	ADC1->CR2&=~(1<<1);											//单次转换模式
	ADC1->SQR1&=~(0xf<<20);									//只转换一个通道
	ADC1->SQR3=((ADC1->SQR3)&0xfffffff0)+ADCChannelx;						//如果转换多个通道，第一次转换通道count
	if(ADCChannelx<10) ADC1->SMPR2|=SimpleTime<<(ADCChannelx*3);					//通道count的转换时间为239.5个时钟周期
	else ADC1->SMPR1|=SimpleTime<<((ADCChannelx-10)*3);
	
	ADC1->CR2|=1<<0;												//开启ADC并启动转换
	ADC1->CR2|=1<<3;												//执行复位校准
	while(ADC1->CR2&(1<<3));								//等待复位校准完成，即校准寄存器初始化
	ADC1->CR2|=1<<2;												//执行AD校准
	while(ADC1->CR2&(1<<2));								//等待AD校准完成
	
	//STM32_ADC1WatchDogInit(ADCChannelx,3.3,7.0/3);
	ADC1->CR1|=0x1<<5;													//允许产生转换结束（EOC）中断
	STM32_NVICInit(ADC1_2_IRQn,NVIC_GROUP,1,1);	//设置ADC1中断优先级
}

//使用函数前，必须先使用ADC1初始化程序
void STM32_ADC1WatchDogInit(ADCChannelx_TypeDef ADCChannelx,float HighThreshouldVolt,float LowThreshouldVolt)		//以规则通道，单通道检测为例
{
	ADC1->CR1|=1<<23;															//在规则通道上开启模拟看门狗
	ADC1->CR1|=1<<9;															//在单一的通道上使用模拟看门狗
	ADC1->CR1=(ADC1->CR1&(~(0x1f)))+ADCChannelx;	//模拟看门狗选择通道10
	ADC1->HTR=HighThreshouldVolt*0xfff/VCC;				//看门狗高阀值,此处不能把0xfff改为4096，否则会出错
	ADC1->LTR=LowThreshouldVolt*0xfff/VCC;				//看门狗低阀值	
	ADC1->CR1|=1<<6;															//允许模拟看门狗中断
	STM32_NVICInit(ADC1_2_IRQn,NVIC_GROUP,1,1);		//设置ADC1中断优先级
}
//函数作用：ADC1开启一次AD转换
//参数说明：无
//测试结果：CHECK OK
//完成日期：15.10.19
void STM32_ADC1StartAConversion(void)
{
	ADC1->CR2|=1<<22;		//开启一次ADC转换
}
//函数作用：得到初始化通道对应的AD转换电压值
//参数说明：DoNewSimple：0，不进行新的采样，直接用寄存器中的值转化为电压；1，进行新的采样，采样、转换结束后，\
						再用寄存器中的值转化为电压
//返回值说明：所测电压值
//测试结果：CHECK OK
//完成日期：14.02.16
//更新时间：15.10.19
//注意：如果程序中对执行时间要求较高：
//				1，不要使用采样后进行电压转换，等待时间较多（主要是采样+转换时间），可以利用转换结束中断，在中断中读取转换数值，
//						以避免等待时间；
//				2，将采样时间降低，缩短此程序的等待时间（这个办法不知道能不能明显减少等待时间，还没测试）。
float STM32_ADC1GetVal(u8 DoNewSimple)
{
	if(DoNewSimple)
	{
		ADC1->CR2|=1<<22;																			//开始转换规则通道
		while((ADC1->SR&(1<<1))==0);													//等待转换结束
	}
	return ((ADC1->DR&0xfff)*(float)VCC/0xfff);						//取低16位,并结合参考电压求出具体电压值
}

/****************************中断处理函数******************************/
void ADC1_2_IRQHandler(void)
{
	if(ADC1->SR&0x01)					//模拟看门狗标志位AWD置位
	{
		ADC1->SR&=~(0x01);		//模拟看门狗标志位AWD复位
	}
	else if(ADC1->SR&0x02)		//转换结束位EOC置位
	{
		ADC1->SR&=~(0x02);		//转换结束位EOC复位
	}
	else if(ADC1->SR&0x04)		//注入通道转换结束位JEOC置位
	{
		ADC1->SR&=~(0x04);		//注入通道转换结束位JEOC复位
	}
}
