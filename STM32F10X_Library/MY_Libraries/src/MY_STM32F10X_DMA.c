#include "MY_STM32F10X_Conf.h"

u32 DMA1_DataLength[7],DMA2_DataLength[5];					//存放数据长度设置项

//函数作用：DMA使用初始化
//参数说明：DMA_CHx:DMA通道，PeripheralAddr:外设地址，MemoryAddr:存储器地址，DataLength:要传送的数据长度
//测试结果：CHECK OK
//完成日期：14.02.18
void STM32_DMAInit(DMA_Channel_TypeDef *DMA_CHx,u32 PeripheralAddr,u32 MemoryAddr,u32 DataLength)	//从存储器到外设
{
	u32 PeripheralAddrBuffer=PeripheralAddr;				//为外设地址的传送提供缓冲，我也不知道为什么
	if((u32)DMA_CHx<DMA2_BASE)								//DMA1
	{
		RCC->AHBENR|=1<<0;									//使能DMA1时钟
		__nop();                    						//等待时钟稳定,至少两个nop
		__nop();                    
		__nop();
		DMA1_DataLength[((u32)DMA_CHx-DMA1_BASE-8)/0x14]=DataLength;
	}
	else												//DMA2
	{
		RCC->AHBENR|=1<<1;                              //使能DMA2时钟
		__nop();                    					//等待时钟稳定,至少两个nop
		__nop();                    
		__nop();														
		DMA2_DataLength[((u32)DMA_CHx-DMA2_BASE-8)/0x14]=DataLength;
	}
	DMA_CHx->CPAR=PeripheralAddrBuffer;					//接收数据的外设地址
	DMA_CHx->CMAR=MemoryAddr;							//接收数据的存储器地址
	DMA_CHx->CNDTR=DataLength;							//要传输数据的长度
	DMA_CHx->CCR=0X00000000;							//复位寄存器
	DMA_CHx->CCR|=1<<4;									//从存储器读
	DMA_CHx->CCR&=~(1<<5);								//不执行循环操作
	DMA_CHx->CCR&=~(1<<6);								//外设地址非增量模式（此处只用一个外设）
	DMA_CHx->CCR|=1<<7;									//存储器地址增量模式
	DMA_CHx->CCR=(DMA_CHx->CCR&(~(0x3<<8)))+(0<<8);		//外设数据宽度：8
	DMA_CHx->CCR=(DMA_CHx->CCR&(~(0x3<<10)))+(0<<10);	//存储器数据宽度：8
	DMA_CHx->CCR=(DMA_CHx->CCR&(~(0x3<<12)))+(1<<12);	//通道优先级：中
	DMA_CHx->CCR&=~(1<<14);								//禁用存储器到存储器模式
	DMA_CHx->CCR|=1<<0;  								//第一次开启通道 
}

//函数作用：DMA开启一次数据传送
//参数说明：DMA_CHx:DMA通道
//测试结果：CHECK OK
//完成日期：14.02.18
void STM32_DMAEnableOnce(DMA_Channel_TypeDef *DMA_CHx)
{
	DMA_CHx->CCR&=~(1<<0);       						//通道关闭，为啥要关闭一次。。。单次传递，必须要先关闭，再开启
	if((u32)DMA_CHx<DMA2_BASE)							//DMA1
	{
		DMA_CHx->CNDTR=DMA1_DataLength[((u32)DMA_CHx-DMA1_BASE-8)/0x14];
	}
	else												//DMA2
	{
		DMA_CHx->CNDTR=DMA2_DataLength[((u32)DMA_CHx-DMA2_BASE-8)/0x14];
	}
	DMA_CHx->CCR|=1<<0;									//通道开启
}
