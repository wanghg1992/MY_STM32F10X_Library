#include "MY_STM32F10X_Conf.h"

u32 DMA1_DataLength[7],DMA2_DataLength[5];					//������ݳ���������

//�������ã�DMAʹ�ó�ʼ��
//����˵����DMA_CHx:DMAͨ����PeripheralAddr:�����ַ��MemoryAddr:�洢����ַ��DataLength:Ҫ���͵����ݳ���
//���Խ����CHECK OK
//������ڣ�14.02.18
void STM32_DMAInit(DMA_Channel_TypeDef *DMA_CHx,u32 PeripheralAddr,u32 MemoryAddr,u32 DataLength)	//�Ӵ洢��������
{
	u32 PeripheralAddrBuffer=PeripheralAddr;				//Ϊ�����ַ�Ĵ����ṩ���壬��Ҳ��֪��Ϊʲô
	if((u32)DMA_CHx<DMA2_BASE)								//DMA1
	{
		RCC->AHBENR|=1<<0;									//ʹ��DMA1ʱ��
		__nop();                    						//�ȴ�ʱ���ȶ�,��������nop
		__nop();                    
		__nop();
		DMA1_DataLength[((u32)DMA_CHx-DMA1_BASE-8)/0x14]=DataLength;
	}
	else												//DMA2
	{
		RCC->AHBENR|=1<<1;                              //ʹ��DMA2ʱ��
		__nop();                    					//�ȴ�ʱ���ȶ�,��������nop
		__nop();                    
		__nop();														
		DMA2_DataLength[((u32)DMA_CHx-DMA2_BASE-8)/0x14]=DataLength;
	}
	DMA_CHx->CPAR=PeripheralAddrBuffer;					//�������ݵ������ַ
	DMA_CHx->CMAR=MemoryAddr;							//�������ݵĴ洢����ַ
	DMA_CHx->CNDTR=DataLength;							//Ҫ�������ݵĳ���
	DMA_CHx->CCR=0X00000000;							//��λ�Ĵ���
	DMA_CHx->CCR|=1<<4;									//�Ӵ洢����
	DMA_CHx->CCR&=~(1<<5);								//��ִ��ѭ������
	DMA_CHx->CCR&=~(1<<6);								//�����ַ������ģʽ���˴�ֻ��һ�����裩
	DMA_CHx->CCR|=1<<7;									//�洢����ַ����ģʽ
	DMA_CHx->CCR=(DMA_CHx->CCR&(~(0x3<<8)))+(0<<8);		//�������ݿ�ȣ�8
	DMA_CHx->CCR=(DMA_CHx->CCR&(~(0x3<<10)))+(0<<10);	//�洢�����ݿ�ȣ�8
	DMA_CHx->CCR=(DMA_CHx->CCR&(~(0x3<<12)))+(1<<12);	//ͨ�����ȼ�����
	DMA_CHx->CCR&=~(1<<14);								//���ô洢�����洢��ģʽ
	DMA_CHx->CCR|=1<<0;  								//��һ�ο���ͨ�� 
}

//�������ã�DMA����һ�����ݴ���
//����˵����DMA_CHx:DMAͨ��
//���Խ����CHECK OK
//������ڣ�14.02.18
void STM32_DMAEnableOnce(DMA_Channel_TypeDef *DMA_CHx)
{
	DMA_CHx->CCR&=~(1<<0);       						//ͨ���رգ�ΪɶҪ�ر�һ�Ρ��������δ��ݣ�����Ҫ�ȹرգ��ٿ���
	if((u32)DMA_CHx<DMA2_BASE)							//DMA1
	{
		DMA_CHx->CNDTR=DMA1_DataLength[((u32)DMA_CHx-DMA1_BASE-8)/0x14];
	}
	else												//DMA2
	{
		DMA_CHx->CNDTR=DMA2_DataLength[((u32)DMA_CHx-DMA2_BASE-8)/0x14];
	}
	DMA_CHx->CCR|=1<<0;									//ͨ������
}
