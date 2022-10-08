#include "MY_STM32F10X_Conf.h"

//函数作用：初始化引脚工作模式
//参数说明：GPIOx:要初始化的引脚所属GPIO口；GPIOInit_pst:用于初始化引脚的结构体（详见stm32f10x_gpio.h）
//测试结果：NOT CHECK
//完成日期：14.03.01
void STM32_GPIOInit(GPIO_TypeDef* GPIOx,GPIO_InitTypeDef* GPIOInit_pst)
{
	u8 pin=0,mode=0;								//pin用于从0到15遍历，查找操作引脚；mode用于得出并存放引脚模式的值
	for(pin=0;pin<16;pin++)
	{
		if(((GPIOInit_pst->GPIO_Pin>>pin)&0x1)==1)				//操作pin引脚
		{
			if(((GPIOInit_pst->GPIO_Mode>>4)&0x1)==1)			//输出
				mode=(GPIOInit_pst->GPIO_Mode&0xf)+GPIOInit_pst->GPIO_Speed;
			else												//输入
			{
				mode=GPIOInit_pst->GPIO_Mode&0xf;
				if(((GPIOInit_pst->GPIO_Mode>>4)&0xf)==0x2)			//引脚下拉
					GPIOx->ODR&=~(1<<pin);
				else if(((GPIOInit_pst->GPIO_Mode>>4)&0xf)==0x4)	//引脚上拉
					GPIOx->ODR|=1<<pin;
			}
				
			if(pin<8)												
				GPIOx->CRL=(GPIOx->CRL&(~(0xf<<(pin*4))))+(mode<<(pin*4));			//配置引脚工作模式
			else
				GPIOx->CRH=(GPIOx->CRH&(~(0xf<<((pin-8)*4))))+(mode<<((pin-8)*4));	//配置引脚工作模式
		}
	}
}

//函数作用：对IO口进行按位写操作
//参数说明：GPIOx:要进行位操作的引脚所属GPIO口；GPIO_Pin:要操作的引脚（支持或操作）；\
			BitVal:要把引脚设置为的值（Bit_SET或Bit_RESET）
//测试结果：NOT CHECK
//完成日期：14.03.01
void STM32_GPIOWriteBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, BitAction BitVal)
{
	if(Bit_SET==BitVal)			//pin置高
		GPIOx->BSRR=GPIO_Pin;
	else if(Bit_RESET==BitVal)	//pin置低
		GPIOx->BRR=GPIO_Pin;
}

//函数作用：对IO口进行按位读操作
//参数说明：GPIOx:要进行位操作的引脚所属GPIO口；GPIO_Pin:要操作的引脚
//返回值：要读取引脚的电平值（Bit_SET或Bit_RESET）
//测试结果：NOT CHECK
//完成日期：14.03.01
BitAction STM32_GPIOReadBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	if(GPIOx->IDR&GPIO_Pin)		//高电平
		return Bit_SET;
	else						//低电平
		return Bit_RESET;
}

//函数作用：对IO口进行整体写操作
//参数说明：GPIOx:要进行整体写操作的GPIO口；Data:要写入IO口的值
//测试结果：NOT CHECK
//完成日期：14.03.01
void STM32_GPIOWriteData(GPIO_TypeDef* GPIOx,u16 Data)
{
	GPIOx->ODR=Data;
}

//函数作用：对IO口进行整体读操作
//参数说明：GPIOx:要进行整体读操作的GPIO口
//返回值：要读取的GPIO口的引脚电平值（16位数）
//测试结果：NOT CHECK
//完成日期：14.03.01
u16 STM32_GPIOReadData(GPIO_TypeDef* GPIOx)
{
	return (GPIOx->IDR&0xffff);
}


