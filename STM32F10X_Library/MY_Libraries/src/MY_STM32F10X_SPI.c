#include "MY_STM32F10X_Conf.h"

//�������ã�SPIx��ʼ��
//����˵����SPIxΪSPI�ڣ�ȡֵΪ��SPI1��SPI2��SPI3
//���Խ����SPI2,CHECK OK
//������ڣ�15.02.05
//������SPIģ��ĳ�ʼ�����룬���ó�����ģʽ������SD Card/W25X16/24L01/JF24C							  
void STM32_SPIxInit(SPI_TypeDef *SPIx)
{	 
	//IO�ڡ�SPIʱ��ʹ�ܣ�IO�ڳ�ʼ��
	switch((u32)SPIx)
	{
		case SPI1_BASE:
			RCC->APB2ENR|=1<<2;  			//PORTAʱ��ʹ�� 
			RCC->APB2ENR|=1<<12;			//SPI1ʱ��ʹ��		
			GPIOA->CRL&=0X000FFFFF; 
			GPIOA->CRL|=0XBBB00000;		//PA5/6/7�����������    
			GPIOA->ODR|=0X7<<5;   		//PA5/6/7����
			break;
		case SPI2_BASE:
			RCC->APB2ENR|=1<<3;  			//PORTBʱ��ʹ�� 	 
			RCC->APB1ENR|=1<<14;   		//SPI2ʱ��ʹ�� 
			//����ֻ���SPI�ڳ�ʼ��
			GPIOB->CRH&=0X000FFFFF; 
			GPIOB->CRH|=0XBBB00000;		//PB13/14/15�����������   	    
			GPIOB->ODR|=0X7<<13;   		//PB13/14/15����
			break;
		case SPI3_BASE:
			RCC->APB2ENR|=1<<3;  			//PORTBʱ��ʹ�� 
			RCC->APB1ENR|=1<<15;			//SPI3ʱ��ʹ�� 
			GPIOB->CRH&=0XFF000FFF; 
			GPIOB->CRH|=0X00BBB000;		//PB3/4/5�����������   	    
			GPIOB->ODR|=0X7<<3;   		//PB3/4/5����
			break;
		default:break;
	}
	
	SPIx->CR1|=0<<10;//ȫ˫��ģʽ	
	SPIx->CR1|=1<<9; //���nss����
	SPIx->CR1|=1<<8;  

	SPIx->CR1|=1<<2; //SPI����
	SPIx->CR1|=0<<11;//8bit���ݸ�ʽ	
	SPIx->CR1|=1<<1; //����ģʽ��SCKΪ1 CPOL=1
	SPIx->CR1|=1<<0; //���ݲ����ӵڶ���ʱ����ؿ�ʼ,CPHA=1  
	SPIx->CR1|=7<<3; //Fsck=Fcpu/256
	SPIx->CR1|=0<<7; //MSBfirst   
	SPIx->CR1|=1<<6; //SPI�豸ʹ��
	STM32_SPIxReadWriteByte(SPIx,0xff);//��������		 
}

//�������ã�SPIx�����ʷ�Ƶ����
//����˵����SPIxΪSPI�ڣ�SPIBaudFreqDivΪ�����ʷ�Ƶ���ͱ���
//���Խ����SPI2,CHECK OK
//������ڣ�15.02.05
//�����ʷ�Ƶ����:
//SPIBaudFreqDiv_2   2��Ƶ   (SPI 36M@sys 72M)
//SPIBaudFreqDiv_8   8��Ƶ   (SPI 9M@sys 72M)
//SPIBaudFreqDiv_16  16��Ƶ  (SPI 4.5M@sys 72M)
//SPIBaudFreqDiv_256 256��Ƶ (SPI 281.25K@sys 72M)
void STM32_SPIxSetBaudFreqDiv(SPI_TypeDef *SPIx,SPIBaudFreqDiv_TypeDef SPIBaudFreqDiv) 	//����SPI�����ʷ�Ƶ  
{
	SPIx->CR1&=0XFFC7;
	SPIx->CR1|=SPIBaudFreqDiv<<3;	
	SPIx->CR1|=1<<6; //SPI�豸ʹ��	  
} 
//�������ã�SPIx ��дһ���ֽ�
//����˵����SPIxΪSPI�ڣ�TxDataΪҪд����ֽڡ��������ض�ȡ�����ֽ�
//���Խ����SPI2,CHECK OK
//������ڣ�15.02.05
u8 STM32_SPIxReadWriteByte(SPI_TypeDef *SPIx,u8 TxData)
{		
	u8 retry=0;				 
	while((SPIx->SR&1<<1)==0)	//�ȴ���������	
	{
		retry++;
		if(retry>200)return 0;
	}			  
	SPIx->DR=TxData;	 	  		//����һ��byte 
	retry=0;
	while((SPIx->SR&1<<0)==0) //�ȴ�������һ��byte  
	{
		retry++;
		if(retry>200)return 0;
	}	  						    
	return SPIx->DR;          //�����յ�������				    
}
