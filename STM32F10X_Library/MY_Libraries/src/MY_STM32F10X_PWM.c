#include "MY_STM32F10X_Conf.h"
#include "math.h"

//�������ã�TIM4PWMʹ�ó�ʼ��
//����˵����TIMCLK:��ǰ����ʱ��Ƶ�ʣ�Freqence:���Ƶ�ʣ�ȡֵΪ1Hz~3.4MHz���������ã���PulseWidth:�����ȣ�ȡֵΪ0~100��
//					ע�⣺����Freqence��Ϊ�������͵Ļ����������Ƶ�ʿ��Դﵽ0.01Hz
//���Խ����CHECK OK
//������ڣ�14.02.21
//�޸����ڣ�15.9.29
void STM32_TIM4PWMInit(u32 TIMCLK,GPIOBPinx_TypeDef GPIOBPinx,u32 Freqence,u8 PulseWidth)	//��TIM4��PB�����PWM������ӳ��Ϊ��
{
	u32 k=0;
	RCC->APB1ENR|=1<<2;															//TIM4ʱ��ʹ��
	RCC->APB2ENR|=1<<3;															//GPIOBʱ��ʹ��
	
	if(GPIOBPinx<8)																//��Ӧ��������Ϊ�����������
		GPIOB->CRL=(GPIOB->CRL&(~(0xf<<(GPIOBPinx*4))))+(0xb<<(GPIOBPinx*4));
	else
		GPIOB->CRH=(GPIOB->CRH&(~(0xf<<((GPIOBPinx-8)*4))))+(0xb<<((GPIOBPinx-8)*4));
	GPIOB->ODR|=1<<GPIOBPinx;													//��������

	//��������Ƶ�ʣ����ݹ�ʽ��Freqence=TIMCLK/(PSC*ARR)
	k=TIMCLK/Freqence;															
	if(k<=65535)
	{
		TIM4->PSC=0;
		TIM4->ARR=k;
	}
	else
	{
//		for(i=1;i<k/2;i++)
//		{
//			if(k<=i*i&&k>=(i-1)*(i-1))
//			{
//				if((k-(i-1)*(i-1))>(i*i-k))
//				{	
//					TIM4->PSC=i-1;
//					TIM4->ARR=i;
//				}
//				else
//				{
//					TIM4->PSC=i-2;
//					TIM4->ARR=i-1;
//				}
//			}
//		}
		TIM4->ARR=sqrt(k);
		TIM4->PSC=TIM4->ARR-1;
	}
	
	//ȷ��ռ�ձȡ�ѡ��PWMģʽ��Ԥװ��ʹ��
	switch(GPIOBPinx)
	{
		case 6:
			TIM4->CCR1=TIM4->ARR*PulseWidth/100;				//ȷ��ռ�ձȣ�0~100�����˴������ȳ��ٳ����������ݶ�ʧ�����Ϊ��
			TIM4->CCMR1=(TIM4->CCMR1&(~(7<<4)))+(0x6<<4);		//PWM1ģʽ
			TIM4->CCMR1|=1<<3;									//Ԥװ��ʹ��
			break;		
		case 7:
			TIM4->CCR2=TIM4->ARR*PulseWidth/100;
			TIM4->CCMR1=(TIM4->CCMR1&(~(7<<12)))+(0x6<<12);		//PWM1ģʽ
			TIM4->CCMR1|=1<<11;									//Ԥװ��ʹ��
			break;
		case 8:
			TIM4->CCR3=TIM4->ARR*PulseWidth/100;
			TIM4->CCMR2=(TIM4->CCMR2&(~(7<<4)))+(0x6<<4);		//PWM1ģʽ
			TIM4->CCMR2|=1<<3;									//Ԥװ��ʹ��
			break;
		case 9:
			TIM4->CCR4=TIM4->ARR*PulseWidth/100;
			TIM4->CCMR2=(TIM4->CCMR2&(~(7<<12)))+(0x6<<12);		//PWM1ģʽ
			TIM4->CCMR2|=1<<11;									//Ԥװ��ʹ��
			break;
		default:;
	}
	
	TIM4->CCER=(TIM4->CCER&(~(0x3<<((GPIOBPinx-PB6)*4))))+(0x1<<((GPIOBPinx-PB6)*4));	//OCx�ߵ�ƽ��Ч�����ʹ��
	TIM4->CR1|=0x1;																		//ʹ�ܼ�����
}
//�������ã�TIMxPWMʹ�ó�ʼ��(û����ӳ��)
//����˵����TIMCLK:��ǰ����ʱ��Ƶ�ʣ�TIMx����ʱ����TIMChannelx:��ʱ��ͨ����
//					Freqence:���Ƶ�ʣ�ȡֵΪ1Hz~3.4MHz���������ã���DutyRatio:PWMռ�ձȣ�ȡֵΪ0~1.0��
//					ע�⣺����Freqence��Ϊ�������͵Ļ����������Ƶ�ʿ��Դﵽ0.01Hz
//���Խ����CHECK OK
//������ڣ�15.10.10
void STM32_TIMxPWMInit(u32 TIMCLK,TIM_TypeDef* TIMx,TIMChannelx_TypeDef TIMChannelx,u32 Freqence,float DutyRatio)	//��TIM4��PB�����PWM������ӳ��Ϊ��
{
	u32 k=0,TIMCount,GPIOPinx;
	GPIO_TypeDef *GPIOx;
	
	//��ȡ��ʱ����Ҫ��������š�IO������Ϣ����ʹ����Ӧ��ʱ����IO��ʱ��
	switch((int)TIMx)
	{
		case TIM1_BASE:									//��û��
			TIMCount=1;										//�õ���ʱ�����
			GPIOPinx=8+TIMChannelx;				//�õ�ͨ����Ӧ�������
			GPIOx=GPIOA;									//�õ��õ���������
			RCC->APB2ENR|=1<<11;					//TIM1ʱ��ʹ��
			RCC->APB2ENR|=1<<2;						//GPIOAʱ��ʹ��
			break;
		case TIM2_BASE:
			TIMCount=2;										//�õ���ʱ�����
			GPIOPinx=0+TIMChannelx;				//�õ�ͨ����Ӧ�������
			GPIOx=GPIOA;									//�õ��õ���������
			RCC->APB1ENR|=1;							//TIM2ʱ��ʹ��
			RCC->APB2ENR|=1<<2;						//GPIOAʱ��ʹ��
			break;
		case TIM3_BASE:
			TIMCount=3;										//�õ���ʱ�����
			if(TIMChannelx<2)
			{
				GPIOPinx=6+TIMChannelx;	//�õ�ͨ����Ӧ�������
				GPIOx=GPIOA;								//�õ��õ���������
			}
			else
			{
				GPIOPinx=TIMChannelx-2;			//�õ�ͨ����Ӧ�������
				GPIOx=GPIOB;								//�õ��õ���������				
			}
			RCC->APB1ENR|=1<<1;						//TIM3ʱ��ʹ��
			RCC->APB2ENR|=3<<2;						//GPIOA\GPIOBʱ��ʹ��
			break;
		case TIM4_BASE:
			TIMCount=4;										//�õ���ʱ�����
			GPIOPinx=6+TIMChannelx;		//�õ�ͨ����Ӧ�������
			GPIOx=GPIOB;									//�õ��õ���������
			RCC->APB1ENR|=1<<2;						//TIM4ʱ��ʹ��
			RCC->APB2ENR|=1<<3;						//GPIOBʱ��ʹ��			
			break;
		case TIM5_BASE:									//����ӳ��Ļ�����TIM2���������غϡ����ڲ���
			TIMCount=5;										//�õ���ʱ�����
			break;
//		case TIM6_BASE:		//��ʱ��6�Ͷ�ʱ��7�������ڲ���PWM
//			TIMCount=6;
//			break;
//		case TIM7_BASE:
//			TIMCount=7;
//			break;
		case TIM8_BASE:
			TIMCount=8;										//�õ���ʱ�����
			GPIOPinx=6+TIMChannelx;		//�õ�ͨ����Ӧ�������
			GPIOx=GPIOC;									//�õ��õ���������
			RCC->APB2ENR|=1<<13;					//TIM8ʱ��ʹ��
			RCC->APB2ENR|=1<<4;						//GPIOCʱ��ʹ��
			break;
	}
		
	//��ʼ��IO��
	if(GPIOPinx<8)																//��Ӧ��������Ϊ�����������
		GPIOx->CRL=(GPIOx->CRL&(~(0xf<<(GPIOPinx*4))))+(0xb<<(GPIOPinx*4));
	else
		GPIOx->CRH=(GPIOx->CRH&(~(0xf<<((GPIOPinx-8)*4))))+(0xb<<((GPIOPinx-8)*4));
	GPIOx->ODR|=1<<GPIOPinx;													//��������

	//��������Ƶ�ʣ����ݹ�ʽ��Freqence=TIMCLK/(PSC*ARR)
	k=TIMCLK/Freqence;															
	if(k<=65535)
	{
		TIMx->PSC=0;
		TIMx->ARR=k;
	}
	else
	{
		TIMx->ARR=sqrt(k);
		TIMx->PSC=TIMx->ARR-1;
	}
	
	//ȷ��ռ�ձȡ�ѡ��PWMģʽ��Ԥװ��ʹ��
	switch(TIMChannelx)
	{
		case TIMChannel1:
			TIMx->CCR1=TIMx->ARR*DutyRatio;				//ȷ��ռ�ձȣ�0~100�����˴������ȳ��ٳ����������ݶ�ʧ�����Ϊ��
			TIMx->CCMR1=(TIMx->CCMR1&(~(7<<4)))+(0x6<<4);		//PWM1ģʽ
			TIMx->CCMR1|=1<<3;									//Ԥװ��ʹ��,�����û�б�Ҫ����ͷ���
			break;		
		case TIMChannel2:
			TIMx->CCR2=TIMx->ARR*DutyRatio;
			TIMx->CCMR1=(TIMx->CCMR1&(~(7<<12)))+(0x6<<12);		//PWM1ģʽ
			TIMx->CCMR1|=1<<11;									//Ԥװ��ʹ��
			break;
		case TIMChannel3:
			TIMx->CCR3=TIMx->ARR*DutyRatio;
			TIMx->CCMR2=(TIMx->CCMR2&(~(7<<4)))+(0x6<<4);		//PWM1ģʽ
			TIMx->CCMR2|=1<<3;									//Ԥװ��ʹ��
			break;
		case TIMChannel4:
			TIMx->CCR4=TIMx->ARR*DutyRatio;
			TIMx->CCMR2=(TIMx->CCMR2&(~(7<<12)))+(0x6<<12);		//PWM1ģʽ
			TIMx->CCMR2|=1<<11;									//Ԥװ��ʹ��
			break;
		default:;break;
	}
	
	TIMx->CCER=(TIMx->CCER&(~(0x3<<(TIMChannelx*4))))+(0x1<<(TIMChannelx*4));	//OCx�ߵ�ƽ��Ч�����ʹ��
	if(8==TIMCount||1==TIMCount)
		TIMx->BDTR=0X8000;		//ע�⣬TIM8�ӵģ���ͨ�ö�ʱ����ͬ��TIM1�Ƿ�Ҫ�ô�����
//	if(1==TIMCount)
//	{
//		//TIMx->BDTR=0X4000;
//		
//	}
	TIMx->CR1|=0x1;					//ʹ�ܼ�����	
}

//�������ã�TIMx�ı�PWMռ�ձ�(��������ģʽ��Ҳ����������ӳ��)
//����˵����TIMx:��ʱ����TIMChannelx:��ʱ��ͨ����
//					DutyRatio:PWMռ�ձȣ�ȡֵΪ0~1.0��
//���Խ����CHECK OK
//������ڣ�15.10.10
void STM32_TIMxSetPWMDuty(TIM_TypeDef* TIMx,TIMChannelx_TypeDef TIMChannelx,float DutyRatio)	//��TIM4��PB�����PWM������ӳ��Ϊ��
{
	switch(TIMChannelx)
	{
		case TIMChannel1:
			TIMx->CCR1=TIM8->ARR*DutyRatio;				//ȷ��ռ�ձȣ�0~100�����˴������ȳ��ٳ����������ݶ�ʧ�����Ϊ��
			break;		
		case TIMChannel2:
			TIMx->CCR2=TIM8->ARR*DutyRatio;
			break;
		case TIMChannel3:
			TIMx->CCR3=TIM8->ARR*DutyRatio;
			break;
		case TIMChannel4:
			TIMx->CCR4=TIM8->ARR*DutyRatio;
			break;
		default:;break;
	}
}
