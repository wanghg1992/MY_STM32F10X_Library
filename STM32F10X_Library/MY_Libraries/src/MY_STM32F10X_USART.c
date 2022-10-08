#include "MY_STM32F10X_Conf.h"

//�������ã�����ʹ�ó�ʼ��
//����˵����USARTx:Ҫʹ�õĴ���(1~3)��PCLK:APB����ʱ�ӣ�BaudRate:Ҫ���õĲ�����
//���Խ����CHECK OK
//������ڣ�14.02.24
//�������ڣ�15.10.14
void STM32_USARTInit(USART_TypeDef *USARTx,u32 PCLK,u32 BaudRate)
{
	u8 NVIC_Channel=0;										//�ж�ͨ��
	//u32 BaudRateDivInt=0,BaudRateDivFlt=0;					//���ڲ��������ã�BaudRateInt���������ʷ�Ƶ�Ĵ�������λ��BaudRateFlt���������ʷ�Ƶ�Ĵ���С��λ
	if(USARTx==USART1)
	{
		NVIC_Channel=USART1_IRQn;							//�õ��ж�ͨ��
		RCC->APB2ENR|=0x1<<2;								//ʹ��A�˿�ʱ��
		GPIOA->CRH=(GPIOA->CRH&0xfffff00f)+0x000008b0;		//PA9�������죬PA10����������
		RCC->APB2ENR|=1<<14;								//����1ʱ��ʹ��
		RCC->APB2RSTR|=1<<14;								//����1��λ
		RCC->APB2RSTR&=~(1<<14);							//����1ֹͣ��λ
	}
	else if(USARTx==USART2)
	{
		NVIC_Channel=USART2_IRQn;							//�õ��ж�ͨ��
		RCC->APB2ENR|=0x1<<2;								//ʹ��A�˿�ʱ��
		GPIOA->CRL=(GPIOA->CRL&0xffff00ff)+0x00008b00;		//PA2�������죬PA3����������
		RCC->APB1ENR|=1<<17;								//����2ʱ��ʹ��
		RCC->APB1RSTR|=1<<17;								//����2��λ
		RCC->APB1RSTR&=~(1<<17);							//����2ֹͣ��λ

	}
	else if(USARTx==USART3)
	{
		NVIC_Channel=USART3_IRQn;							//�õ��ж�ͨ��
		RCC->APB2ENR|=0x1<<3;								//ʹ��B�˿�ʱ��
		GPIOB->CRH=(GPIOB->CRH&0xffff00ff)+0x00008b00;		//PB10�������죬PB11����������
		RCC->APB1ENR|=1<<18;								//����3ʱ��ʹ��
		RCC->APB1RSTR|=1<<18;								//����3��λ
		RCC->APB1RSTR&=~(1<<18);							//����3ֹͣ��λ
	}
	
//	BaudRateDivInt=(float)PCLK/(BaudRate*16);
//	BaudRateDivFlt=((float)PCLK/(BaudRate*16)-BaudRateDivInt)*16;
//	USARTx->BRR=(BaudRateDivInt<<4)+BaudRateDivFlt;			//����������ΪBaudRate
	USARTx->BRR=PCLK/BaudRate;//����������ΪBaudRate
	USARTx->CR1|=0x200c;								//USARTxģ��ʹ�ܡ�����ʹ�ܡ�����ʹ��
	USARTx->CR1|=((1<<8)+(1<<5));						//PE�ж�ʹ�ܡ����ջ������ǿ��ж�ʹ��
	STM32_NVICInit(NVIC_Channel,3,1,1);
}

//�������ã����ڷ����ַ�����
//����˵����USARTx:Ҫʹ�õĴ��ڣ�ch:Ҫ���͵��ַ�����
//���Խ����CHECK OK
//������ڣ�14.02.24
void STM32_USARTSendChar(USART_TypeDef *USARTx,u8 ch)						//����x�����ַ�����
{
  USARTx->DR=ch;
  while((USARTx->SR&0X40)==0);							//�ȴ����ͽ���
}

//�������ã����ڷ����ַ�������
//����˵����USARTx:Ҫʹ�õĴ��ڣ�p:Ҫ���͵��ַ������ݵ��׵�ַ
//���Խ����CHECK OK
//������ڣ�14.02.24
void STM32_USARTSendString(USART_TypeDef *USARTx,char *p,u16 Length)			//����x�����ַ�������
{
	u16 i=0;
	for(i=0;i<Length;i++)
	{
		STM32_USARTSendChar(USARTx,p[i]);
	}
}


///////////////////////////////////////////////�жϴ�����/////////////////////////////////////////////////////////
void USART1_IRQHandler(void)
{
	if(USART1->SR&(1<<5))								//���ܵ�����
	{
		if(USART1->DR ==0x04)
		{}
		else
			STM32_USARTSendChar(USART1,USART1->DR);
	}
}
