#include "MY_STM32F10X_Conf.h"
#include "usart.h"

///////////////////////////////////////////////////////////////////////////////����ת��Ϊ��
//�������ã�ADC1ʹ�ó�ʼ��
//����˵����GPIO_Pin:Ҫʹ��ADC���ѹ������
//���Խ����CHECK OK
//������ڣ�14.02.16
//����ʱ�䣺15.10.14
void STM32_ADC1Init(ADCChannelx_TypeDef ADCChannelx,SimpleTime_TypeDef SimpleTime)			//�˴���ADC1������ת��Ϊ��
{
	u32 GPIOPinx;
	GPIO_TypeDef *GPIOx;
	//uc count=0;	
	
	//��ʱû��ͨ��16��ͨ��17
	if(ADCChannelx<ADCChannel8)
	{
		GPIOx=GPIOA;
		GPIOPinx=ADCChannelx;
		RCC->APB2ENR|=1<<2;												//�˿�Aʱ��ʹ��
	}
	else if(ADCChannelx<10)
	{
		GPIOx=GPIOB;
		GPIOPinx=ADCChannelx-ADCChannel8;
		RCC->APB2ENR|=1<<3;												//�˿�Bʱ��ʹ��
	}
	else if(ADCChannelx<ADCChannel16)
	{
		GPIOx=GPIOC;
		GPIOPinx=ADCChannelx-ADCChannel10;
		RCC->APB2ENR|=1<<4;												//�˿�Cʱ��ʹ��
	}
	
	if(GPIOPinx<8)														//GPIO_Pin��������Ϊģ������
	{
		GPIOx->CRL&=~(0x0f<<(GPIOPinx*4));
	}
	else
	{
		GPIOx->CRH&=~(0x0f<<((GPIOPinx-8)*4));
	}
	
	RCC->APB2ENR|=1<<9;											//ADC1ʱ��ʹ��
	RCC->APB2RSTR|=1<<9;										//ADC1��λ
	RCC->APB2RSTR&=~(1<<9);									//ADC1ֹͣ��λ
	RCC->CFGR=(RCC->CFGR&(~(0x3<<14)))+(0x2<<14);					//ADCԤ��Ƶ��6��Ƶ
	
	ADC1->CR2|=1<<20;												//ʹ���ⲿ�¼���������ת��
	ADC1->CR2|=7<<17;												//ѡ���������ת����ʽ
	ADC1->CR2&=~(1<<11);										//�Ҷ���
	ADC1->CR2&=~(1<<1);											//����ת��ģʽ
	ADC1->SQR1&=~(0xf<<20);									//ֻת��һ��ͨ��
	ADC1->SQR3=((ADC1->SQR3)&0xfffffff0)+ADCChannelx;						//���ת�����ͨ������һ��ת��ͨ��count
	if(ADCChannelx<10) ADC1->SMPR2|=SimpleTime<<(ADCChannelx*3);					//ͨ��count��ת��ʱ��Ϊ239.5��ʱ������
	else ADC1->SMPR1|=SimpleTime<<((ADCChannelx-10)*3);
	
	ADC1->CR2|=1<<0;												//����ADC������ת��
	ADC1->CR2|=1<<3;												//ִ�и�λУ׼
	while(ADC1->CR2&(1<<3));								//�ȴ���λУ׼��ɣ���У׼�Ĵ�����ʼ��
	ADC1->CR2|=1<<2;												//ִ��ADУ׼
	while(ADC1->CR2&(1<<2));								//�ȴ�ADУ׼���
	
	//STM32_ADC1WatchDogInit(ADCChannelx,3.3,7.0/3);
	ADC1->CR1|=0x1<<5;													//�������ת��������EOC���ж�
	STM32_NVICInit(ADC1_2_IRQn,NVIC_GROUP,1,1);	//����ADC1�ж����ȼ�
}

//ʹ�ú���ǰ��������ʹ��ADC1��ʼ������
void STM32_ADC1WatchDogInit(ADCChannelx_TypeDef ADCChannelx,float HighThreshouldVolt,float LowThreshouldVolt)		//�Թ���ͨ������ͨ�����Ϊ��
{
	ADC1->CR1|=1<<23;															//�ڹ���ͨ���Ͽ���ģ�⿴�Ź�
	ADC1->CR1|=1<<9;															//�ڵ�һ��ͨ����ʹ��ģ�⿴�Ź�
	ADC1->CR1=(ADC1->CR1&(~(0x1f)))+ADCChannelx;	//ģ�⿴�Ź�ѡ��ͨ��10
	ADC1->HTR=HighThreshouldVolt*0xfff/VCC;				//���Ź��߷�ֵ,�˴����ܰ�0xfff��Ϊ4096����������
	ADC1->LTR=LowThreshouldVolt*0xfff/VCC;				//���Ź��ͷ�ֵ	
	ADC1->CR1|=1<<6;															//����ģ�⿴�Ź��ж�
	STM32_NVICInit(ADC1_2_IRQn,NVIC_GROUP,1,1);		//����ADC1�ж����ȼ�
}
//�������ã�ADC1����һ��ADת��
//����˵������
//���Խ����CHECK OK
//������ڣ�15.10.19
void STM32_ADC1StartAConversion(void)
{
	ADC1->CR2|=1<<22;		//����һ��ADCת��
}
//�������ã��õ���ʼ��ͨ����Ӧ��ADת����ѹֵ
//����˵����DoNewSimple��0���������µĲ�����ֱ���üĴ����е�ֵת��Ϊ��ѹ��1�������µĲ�����������ת��������\
						���üĴ����е�ֵת��Ϊ��ѹ
//����ֵ˵���������ѹֵ
//���Խ����CHECK OK
//������ڣ�14.02.16
//����ʱ�䣺15.10.19
//ע�⣺��������ж�ִ��ʱ��Ҫ��ϸߣ�
//				1����Ҫʹ�ò�������е�ѹת�����ȴ�ʱ��϶ࣨ��Ҫ�ǲ���+ת��ʱ�䣩����������ת�������жϣ����ж��ж�ȡת����ֵ��
//						�Ա���ȴ�ʱ�䣻
//				2��������ʱ�併�ͣ����̴˳���ĵȴ�ʱ�䣨����취��֪���ܲ������Լ��ٵȴ�ʱ�䣬��û���ԣ���
float STM32_ADC1GetVal(u8 DoNewSimple)
{
	if(DoNewSimple)
	{
		ADC1->CR2|=1<<22;																			//��ʼת������ͨ��
		while((ADC1->SR&(1<<1))==0);													//�ȴ�ת������
	}
	return ((ADC1->DR&0xfff)*(float)VCC/0xfff);						//ȡ��16λ,����ϲο���ѹ��������ѹֵ
}

/****************************�жϴ�����******************************/
void ADC1_2_IRQHandler(void)
{
	if(ADC1->SR&0x01)					//ģ�⿴�Ź���־λAWD��λ
	{
		ADC1->SR&=~(0x01);		//ģ�⿴�Ź���־λAWD��λ
	}
	else if(ADC1->SR&0x02)		//ת������λEOC��λ
	{
		ADC1->SR&=~(0x02);		//ת������λEOC��λ
	}
	else if(ADC1->SR&0x04)		//ע��ͨ��ת������λJEOC��λ
	{
		ADC1->SR&=~(0x04);		//ע��ͨ��ת������λJEOC��λ
	}
}
