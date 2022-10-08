#include "MY_STM32F10X_Conf.h"

//�������ã��ⲿ�ж�ʹ�ó�ʼ��
//����˵����GPIOxΪ��Ҫ�����ⲿ�жϵĶ˿ںţ�GPIO_Pin:��Ҫ�����ⲿ�жϵ����źţ�NVIC_Group:�ж����ȼ����飻\
			NVIC_PreemptionPriority:��ռ���ȼ���NVIC_SubPriority:��Ӧ���ȼ���ExIntWorkMode:�ⲿ�жϴ�����ʽ��
//���Խ����CHECK OK
//������ڣ�14.02.22
void STM32_ExInterruptInit(GPIO_TypeDef *GPIOx,u16 GPIO_Pin,u8 NVIC_Group,u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,ExIntWorkMode_TypeDef ExIntWorkMode)
{
	u8 count=0;												//����������
	u8 NVIC_Channel=0;										//����ж�ͨ��
	for(count=0;count<16;count++)							//�õ��������
		if((GPIO_Pin>>count)==1) break;
	if(count<5) NVIC_Channel=6+count;						//�õ��ж�ͨ����
	else if(count<10) NVIC_Channel=23;
	else NVIC_Channel=40;
	if(count<8)												//������Ϊ����������
		GPIOx->CRL=(GPIOx->CRL&(~(0xf<<(count*4))))+(0x8<<(count*4));
	else
		GPIOx->CRH=(GPIOx->CRH&(~(0xf<<((count-8)*4))))+(0x8<<((count-8)*4));
	
	NVIC->ISER[NVIC_Channel/32]|=1<<(NVIC_Channel%32);		//�ж�ʹ��
	STM32_NVICInit(NVIC_Channel,NVIC_Group,NVIC_PreemptionPriority,NVIC_SubPriority);		//�жϷ��鼰���ȼ�����	
	RCC->APB2ENR|=0x01;										//ʹ��io����ʱ��
	if(ExIntWorkMode&INTWORKMODE_RISING)					//�����ش����ж�
	{
		GPIOx->ODR&=~GPIO_Pin;	   							//��������
		EXTI->RTSR=0x1<<count;								//���������ش����ж�
	}
	if(ExIntWorkMode&INTWORKMODE_FALLING)					//�½��ش����ж�		
	{
		GPIOx->ODR|=GPIO_Pin;	   							//��������
		EXTI->FTSR=0x1<<count;								//�����½��ش����ж�
	}
	AFIO->EXTICR[count/4]=(AFIO->EXTICR[count/4]&(~(0xf<<((count%4)*4))))+((((u32)GPIOx-GPIOA_BASE)/0x400)<<((count%4)*4));		//�������ж�ӳ�䵽GPIOx���ж�
	EXTI->IMR=0x1<<count;									//���������ж�
	EXTI->EMR=0x1<<count;									//���������¼� 
}

///////////////////////////////////////////////�жϴ�����/////////////////////////////////////////////////////////
void EXTI15_10_IRQHandler(void)								//�ⲿ�жϴ�����
{
	STM32_Delay_ms(20);
	EXTI->PR=1<<12;
}

