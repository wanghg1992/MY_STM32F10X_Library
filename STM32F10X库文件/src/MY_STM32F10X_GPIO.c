#include "MY_STM32F10X_Conf.h"

//�������ã���ʼ�����Ź���ģʽ
//����˵����GPIOx:Ҫ��ʼ������������GPIO�ڣ�GPIOInit_pst:���ڳ�ʼ�����ŵĽṹ�壨���stm32f10x_gpio.h��
//���Խ����NOT CHECK
//������ڣ�14.03.01
void STM32_GPIOInit(GPIO_TypeDef* GPIOx,GPIO_InitTypeDef* GPIOInit_pst)
{
	u8 pin=0,mode=0;								//pin���ڴ�0��15���������Ҳ������ţ�mode���ڵó����������ģʽ��ֵ
	for(pin=0;pin<16;pin++)
	{
		if(((GPIOInit_pst->GPIO_Pin>>pin)&0x1)==1)				//����pin����
		{
			if(((GPIOInit_pst->GPIO_Mode>>4)&0x1)==1)			//���
				mode=(GPIOInit_pst->GPIO_Mode&0xf)+GPIOInit_pst->GPIO_Speed;
			else												//����
			{
				mode=GPIOInit_pst->GPIO_Mode&0xf;
				if(((GPIOInit_pst->GPIO_Mode>>4)&0xf)==0x2)			//��������
					GPIOx->ODR&=~(1<<pin);
				else if(((GPIOInit_pst->GPIO_Mode>>4)&0xf)==0x4)	//��������
					GPIOx->ODR|=1<<pin;
			}
				
			if(pin<8)												
				GPIOx->CRL=(GPIOx->CRL&(~(0xf<<(pin*4))))+(mode<<(pin*4));			//�������Ź���ģʽ
			else
				GPIOx->CRH=(GPIOx->CRH&(~(0xf<<((pin-8)*4))))+(mode<<((pin-8)*4));	//�������Ź���ģʽ
		}
	}
}

//�������ã���IO�ڽ��а�λд����
//����˵����GPIOx:Ҫ����λ��������������GPIO�ڣ�GPIO_Pin:Ҫ���������ţ�֧�ֻ��������\
			BitVal:Ҫ����������Ϊ��ֵ��Bit_SET��Bit_RESET��
//���Խ����NOT CHECK
//������ڣ�14.03.01
void STM32_GPIOWriteBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, BitAction BitVal)
{
	if(Bit_SET==BitVal)			//pin�ø�
		GPIOx->BSRR=GPIO_Pin;
	else if(Bit_RESET==BitVal)	//pin�õ�
		GPIOx->BRR=GPIO_Pin;
}

//�������ã���IO�ڽ��а�λ������
//����˵����GPIOx:Ҫ����λ��������������GPIO�ڣ�GPIO_Pin:Ҫ����������
//����ֵ��Ҫ��ȡ���ŵĵ�ƽֵ��Bit_SET��Bit_RESET��
//���Խ����NOT CHECK
//������ڣ�14.03.01
BitAction STM32_GPIOReadBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	if(GPIOx->IDR&GPIO_Pin)		//�ߵ�ƽ
		return Bit_SET;
	else						//�͵�ƽ
		return Bit_RESET;
}

//�������ã���IO�ڽ�������д����
//����˵����GPIOx:Ҫ��������д������GPIO�ڣ�Data:Ҫд��IO�ڵ�ֵ
//���Խ����NOT CHECK
//������ڣ�14.03.01
void STM32_GPIOWriteData(GPIO_TypeDef* GPIOx,u16 Data)
{
	GPIOx->ODR=Data;
}

//�������ã���IO�ڽ������������
//����˵����GPIOx:Ҫ���������������GPIO��
//����ֵ��Ҫ��ȡ��GPIO�ڵ����ŵ�ƽֵ��16λ����
//���Խ����NOT CHECK
//������ڣ�14.03.01
u16 STM32_GPIOReadData(GPIO_TypeDef* GPIOx)
{
	return (GPIOx->IDR&0xffff);
}


