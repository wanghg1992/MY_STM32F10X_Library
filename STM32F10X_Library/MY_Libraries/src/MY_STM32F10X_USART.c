#include "MY_STM32F10X_Conf.h"

//函数作用：串口使用初始化
//参数说明：USARTx:要使用的串口(1~3)；PCLK:APB外设时钟；BaudRate:要设置的波特率
//测试结果：CHECK OK
//完成日期：14.02.24
//更新日期：15.10.14
void STM32_USARTInit(USART_TypeDef *USARTx,u32 PCLK,u32 BaudRate)
{
	u8 NVIC_Channel=0;										//中断通道
	//u32 BaudRateDivInt=0,BaudRateDivFlt=0;					//用于波特率配置，BaudRateInt保留波特率分频寄存器整数位，BaudRateFlt保留波特率分频寄存器小数位
	if(USARTx==USART1)
	{
		NVIC_Channel=USART1_IRQn;							//得到中断通道
		RCC->APB2ENR|=0x1<<2;								//使能A端口时钟
		GPIOA->CRH=(GPIOA->CRH&0xfffff00f)+0x000008b0;		//PA9复用推挽，PA10上下拉输入
		RCC->APB2ENR|=1<<14;								//串口1时钟使能
		RCC->APB2RSTR|=1<<14;								//串口1复位
		RCC->APB2RSTR&=~(1<<14);							//串口1停止复位
	}
	else if(USARTx==USART2)
	{
		NVIC_Channel=USART2_IRQn;							//得到中断通道
		RCC->APB2ENR|=0x1<<2;								//使能A端口时钟
		GPIOA->CRL=(GPIOA->CRL&0xffff00ff)+0x00008b00;		//PA2复用推挽，PA3上下拉输入
		RCC->APB1ENR|=1<<17;								//串口2时钟使能
		RCC->APB1RSTR|=1<<17;								//串口2复位
		RCC->APB1RSTR&=~(1<<17);							//串口2停止复位

	}
	else if(USARTx==USART3)
	{
		NVIC_Channel=USART3_IRQn;							//得到中断通道
		RCC->APB2ENR|=0x1<<3;								//使能B端口时钟
		GPIOB->CRH=(GPIOB->CRH&0xffff00ff)+0x00008b00;		//PB10复用推挽，PB11上下拉输入
		RCC->APB1ENR|=1<<18;								//串口3时钟使能
		RCC->APB1RSTR|=1<<18;								//串口3复位
		RCC->APB1RSTR&=~(1<<18);							//串口3停止复位
	}
	
//	BaudRateDivInt=(float)PCLK/(BaudRate*16);
//	BaudRateDivFlt=((float)PCLK/(BaudRate*16)-BaudRateDivInt)*16;
//	USARTx->BRR=(BaudRateDivInt<<4)+BaudRateDivFlt;			//波特率配置为BaudRate
	USARTx->BRR=PCLK/BaudRate;//波特率配置为BaudRate
	USARTx->CR1|=0x200c;								//USARTx模块使能、发送使能、接收使能
	USARTx->CR1|=((1<<8)+(1<<5));						//PE中断使能、接收缓冲区非空中断使能
	STM32_NVICInit(NVIC_Channel,3,1,1);
}

//函数作用：串口发送字符数据
//参数说明：USARTx:要使用的串口；ch:要发送的字符数据
//测试结果：CHECK OK
//完成日期：14.02.24
void STM32_USARTSendChar(USART_TypeDef *USARTx,u8 ch)						//串口x发送字符数据
{
  USARTx->DR=ch;
  while((USARTx->SR&0X40)==0);							//等待发送结束
}

//函数作用：串口发送字符串数据
//参数说明：USARTx:要使用的串口；p:要发送的字符串数据的首地址
//测试结果：CHECK OK
//完成日期：14.02.24
void STM32_USARTSendString(USART_TypeDef *USARTx,char *p,u16 Length)			//串口x发送字符串数据
{
	u16 i=0;
	for(i=0;i<Length;i++)
	{
		STM32_USARTSendChar(USARTx,p[i]);
	}
}


///////////////////////////////////////////////中断处理函数/////////////////////////////////////////////////////////
void USART1_IRQHandler(void)
{
	if(USART1->SR&(1<<5))								//接受到数据
	{
		if(USART1->DR ==0x04)
		{}
		else
			STM32_USARTSendChar(USART1,USART1->DR);
	}
}
