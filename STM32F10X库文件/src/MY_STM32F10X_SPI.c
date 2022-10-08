#include "MY_STM32F10X_Conf.h"

//函数作用：SPIx初始化
//参数说明：SPIx为SPI口，取值为：SPI1、SPI2或SPI3
//测试结果：SPI2,CHECK OK
//完成日期：15.02.05
//以下是SPI模块的初始化代码，配置成主机模式，访问SD Card/W25X16/24L01/JF24C							  
void STM32_SPIxInit(SPI_TypeDef *SPIx)
{	 
	//IO口、SPI时钟使能，IO口初始化
	switch((u32)SPIx)
	{
		case SPI1_BASE:
			RCC->APB2ENR|=1<<2;  			//PORTA时钟使能 
			RCC->APB2ENR|=1<<12;			//SPI1时钟使能		
			GPIOA->CRL&=0X000FFFFF; 
			GPIOA->CRL|=0XBBB00000;		//PA5/6/7复用推挽输出    
			GPIOA->ODR|=0X7<<5;   		//PA5/6/7上拉
			break;
		case SPI2_BASE:
			RCC->APB2ENR|=1<<3;  			//PORTB时钟使能 	 
			RCC->APB1ENR|=1<<14;   		//SPI2时钟使能 
			//这里只针对SPI口初始化
			GPIOB->CRH&=0X000FFFFF; 
			GPIOB->CRH|=0XBBB00000;		//PB13/14/15复用推挽输出   	    
			GPIOB->ODR|=0X7<<13;   		//PB13/14/15上拉
			break;
		case SPI3_BASE:
			RCC->APB2ENR|=1<<3;  			//PORTB时钟使能 
			RCC->APB1ENR|=1<<15;			//SPI3时钟使能 
			GPIOB->CRH&=0XFF000FFF; 
			GPIOB->CRH|=0X00BBB000;		//PB3/4/5复用推挽输出   	    
			GPIOB->ODR|=0X7<<3;   		//PB3/4/5上拉
			break;
		default:break;
	}
	
	SPIx->CR1|=0<<10;//全双工模式	
	SPIx->CR1|=1<<9; //软件nss管理
	SPIx->CR1|=1<<8;  

	SPIx->CR1|=1<<2; //SPI主机
	SPIx->CR1|=0<<11;//8bit数据格式	
	SPIx->CR1|=1<<1; //空闲模式下SCK为1 CPOL=1
	SPIx->CR1|=1<<0; //数据采样从第二个时间边沿开始,CPHA=1  
	SPIx->CR1|=7<<3; //Fsck=Fcpu/256
	SPIx->CR1|=0<<7; //MSBfirst   
	SPIx->CR1|=1<<6; //SPI设备使能
	STM32_SPIxReadWriteByte(SPIx,0xff);//启动传输		 
}

//函数作用：SPIx波特率分频设置
//参数说明：SPIx为SPI口，SPIBaudFreqDiv为波特率分频类型变量
//测试结果：SPI2,CHECK OK
//完成日期：15.02.05
//波特率分频设置:
//SPIBaudFreqDiv_2   2分频   (SPI 36M@sys 72M)
//SPIBaudFreqDiv_8   8分频   (SPI 9M@sys 72M)
//SPIBaudFreqDiv_16  16分频  (SPI 4.5M@sys 72M)
//SPIBaudFreqDiv_256 256分频 (SPI 281.25K@sys 72M)
void STM32_SPIxSetBaudFreqDiv(SPI_TypeDef *SPIx,SPIBaudFreqDiv_TypeDef SPIBaudFreqDiv) 	//设置SPI波特率分频  
{
	SPIx->CR1&=0XFFC7;
	SPIx->CR1|=SPIBaudFreqDiv<<3;	
	SPIx->CR1|=1<<6; //SPI设备使能	  
} 
//函数作用：SPIx 读写一个字节
//参数说明：SPIx为SPI口，TxData为要写入的字节。函数返回读取到的字节
//测试结果：SPI2,CHECK OK
//完成日期：15.02.05
u8 STM32_SPIxReadWriteByte(SPI_TypeDef *SPIx,u8 TxData)
{		
	u8 retry=0;				 
	while((SPIx->SR&1<<1)==0)	//等待发送区空	
	{
		retry++;
		if(retry>200)return 0;
	}			  
	SPIx->DR=TxData;	 	  		//发送一个byte 
	retry=0;
	while((SPIx->SR&1<<0)==0) //等待接收完一个byte  
	{
		retry++;
		if(retry>200)return 0;
	}	  						    
	return SPIx->DR;          //返回收到的数据				    
}
