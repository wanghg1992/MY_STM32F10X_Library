#include "MY_STM32F10X_Conf.h"

//�������ã�RTCʹ�ó�ʼ��
//����˵�����޲���
//���Խ����CHECK OK
//������ڣ�14.02.20
u8 STM32_RTCInit(void)
{
	u8 i=0;
	if(BKP->DR1!=0x1234)
	{
		RCC->APB1ENR|=0x3<<27;								//ʹ�ܵ�Դ�ӿ�ʱ�Ӻͱ��ݽӿ�ʱ��
		PWR->CR|=1<<8;										//����д��RTC�ͺ󱸼Ĵ���
		RCC->BDCR|=1<<16;									//��λ����������
		RCC->BDCR&=~(1<<16);								//������������λ����
		RCC->BDCR|=1<<0;									//�ⲿ��������ʹ�ܣ��˾����֤
		while((RCC->BDCR&1<<1)==0&&((++i)<250)) 			//�ȴ��ⲿʱ�Ӿ���
			STM32_Delay_ms(10);	
		if(i>=250) return 1;								//��ʼ��ʱ��ʧ�ܣ�����������
		
		RCC->BDCR=(RCC->BDCR&(~(0x3<<8)))+(0x1<<8);			//ѡ��LSE��ΪRTCʱ��
		RCC->BDCR|=1<<15;									//RTCʱ��ʹ��
		while((RTC->CRL&(1<<5))==0);						//�ȴ�RTC�Ĵ���д�������
		while((RTC->CRL&(1<<3))==0);						//�ȴ�RTC�Ĵ���ͬ��
		RTC->CRH|=1<<0;										//����RTC���ж�
		while((RTC->CRL&(1<<5))==0);						//�ȴ�RTC�Ĵ���д�������
		RTC->CRL|=1<<4;										//��������RTC_CNT��RTC_ALR��RTC_PRL�Ĵ���
		while((RTC->CRL&(1<<5))==0);						//�ȴ�RTC�Ĵ���д�������
		RTC->PRLH=0x0000;									//��RTCCLK32768��Ƶ��1Hz
		RTC->PRLL=32767;															
		RTC->CNTH=0x0000;									//����RTC��������ֵ
		RTC->CNTL=0xaaaa;
		RTC->CRL&=~(1<<4);									//����RTC_CNT��RTC_ALR��RTC_PRL�Ĵ�������������������
		while((RTC->CRL&(1<<5))==0);						//�ȴ�RTC�Ĵ���д�������
		BKP->DR1=0x1234;									//��һ�β�����־
	}
	else
	{
		while((RTC->CRL&(1<<3))==0);						//�ȴ�RTC�Ĵ���ͬ��
		RTC->CRH|=1<<0;										//����RTC���ж�
		while((RTC->CRL&(1<<5))==0);						//�ȴ�RTC�Ĵ���д�������
	}
	STM32_NVICInit(RTC_IRQn,NVIC_GROUP,0,0);					//�жϷ��鼰���ȼ�����
	return 0;												//��ʼ���ɹ�
}

///////////////////////////////////////////////�жϴ�����/////////////////////////////////////////////////////////
void RTC_IRQHandler(void)
{
	if((RTC->CRL&1<<0)!=0)									//���ж�
	{
		STM32_Delay_us(50);									//��ʱ���ȼ�����ֵ�ȶ�������������Ӧ�ȴ�30us
	}
	RTC->CRL&=0xfffe;										//������жϱ�־
	while((RTC->CRL&(1<<5))==0);							//�ȴ�RTC�Ĵ���д�������
}
