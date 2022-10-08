#include "MY_STM32F10X_Conf.h"
#include "math.h"

//�������ã���ʱ��ʹ�ó�ʼ��
//����˵����Timex:��ʱ����arr:�������Զ���װֵ��psc��ʱ��ʱ��Ԥ��Ƶֵ
//���Խ����CHECK OK
//������ڣ�14.02.14
//����ʱ�䣺15.10.10
void STM32_TimerInit(TIM_TypeDef *TIMx,u16 arr,u16 psc)				
{
	uc TimeCount=0;																			//��Ŷ�ʱ���ı��
	if((ui)TIMx>=TIM2_BASE&&(ui)TIMx<=TIM7_BASE)			//��ʱ��2~7
	{
		TimeCount=((ui)TIMx-TIM2_BASE)/0x400+2;					//���㶨ʱ�����
		RCC->APB1ENR|=1<<(TimeCount-2);										//ʹ�ܶ�ʱ��ʱ��
		TIMx->ARR=arr;																		//���ü������Զ���װֵ
		TIMx->PSC=psc-1;																	//����ʱ��Ԥ��Ƶ�Ĵ���
		TIMx->DIER|=((1<<0)+(1<<6));											//��������жϺʹ����ж�
		TIMx->CR1|=1<<0;																	//ʹ�ܼ�������DIRΪ0������
		switch(TimeCount)
		{
			case 2:STM32_NVICInit(TIM2_IRQn,2,1,3);break;									//�жϷ��鼰���ȼ�����  �Ķ�ʱ��ʱ������ط�ҲҪ��
			case 3:STM32_NVICInit(TIM3_IRQn,2,1,3);break;									//�жϷ��鼰���ȼ�����  �Ķ�ʱ��ʱ������ط�ҲҪ��
			case 4:STM32_NVICInit(TIM4_IRQn,2,1,3);break;									//�жϷ��鼰���ȼ�����  �Ķ�ʱ��ʱ������ط�ҲҪ��
			case 5:STM32_NVICInit(TIM5_IRQn,2,1,3);break;									//�жϷ��鼰���ȼ�����  �Ķ�ʱ��ʱ������ط�ҲҪ��
			case 6:STM32_NVICInit(TIM6_IRQn,2,1,3);break;									//�жϷ��鼰���ȼ�����  �Ķ�ʱ��ʱ������ط�ҲҪ��
			case 7:STM32_NVICInit(TIM7_IRQn,2,1,3);break;									//�жϷ��鼰���ȼ�����  �Ķ�ʱ��ʱ������ط�ҲҪ��
			default:;break;
		}
	}
}

//�������ã���ʱ��ʹ�ó�ʼ��
//����˵����Timex:��ʱ����us��Ҫ��ʱ��us��
//���Խ����CHECK OK
//������ڣ�15.10.10
//���ۿ���ʱ��Χ��1/72us��59.65232355555556s��ʵ����ʱ��Χ��1~59652323us
void STM32_TimerInitUs(TIM_TypeDef *TIMx,u32 us)				
{
	u32 count=us*72;
	if(count<=65535)
	{
		STM32_TimerInit(TIMx,count,1);		
	}
	else
	{
		STM32_TimerInit(TIMx,sqrt(count),sqrt(count));				
	}	
}

//�������ã����Ժ�����ִ��ʱ��
//����˵����Timex:��ʱ����Function��Ҫ���Եĺ�����count:���ԵĴ�������β���ȡִ��ʱ�����ֵ����
//					����ֵΪ���Ժ�����ִ��ʱ�䣬��λΪus��
//���Խ����CHECK OK
//������ڣ�15.12.06
//ע�⣺����ʵ����Ӧ��ʱ�����жϴ���������ʹFunctionTimeMeasureIntCount���ж��м�1�����£�
///*
//void TIMx_IRQHandler(void)
//{ 		    		  			    
//	if(TIMx->SR&0X0001)			//����ж�
//	{
//		FunctionTimeMeasureIntCount++;
//	}				   
//	TIMx->SR&=~(1<<0);			//����жϱ�־λ
//}
//*/
static u32 FunctionTimeMeasureIntCount=0;		//��Ҫ�����õ����ж��м�1
u32 STM32_MeasureFuncExctTimeByTimerx(TIM_TypeDef *TIMx,void(*Function)(void),u8 count)
{
	u8 i=0;
	u32 maxtime[2]={0,0},time=0;
	STM32_TimerInit(TIMx,65535,72);			//1us����һ�Σ�65535us�ж�һ��
//	delay_ms(2);											//ʵ��֤������Ҫ��ʱ1ms���������ȶ��󣬺�������Ų��������
	STM32_Delay_ms(2);
	for(i=0;i<count;i++)
	{
		FunctionTimeMeasureIntCount=0;
		time=TIMx->CNT;
		Function();
		time=65535*FunctionTimeMeasureIntCount+TIMx->CNT-time;
		if(maxtime[0]<time)				//��¼���ʹδ��ֵ
		{
			maxtime[1]=maxtime[0];
			maxtime[0]=time;
		}
		else if(maxtime[1]<time)
		{
			maxtime[1]=time;
		}
	}
	TIMx->DIER&=~(((1<<0)+(1<<6)));													//����������жϺʹ����ж�
	TIMx->CR1&=~(1<<0);																			//���ܼ�������DIRΪ0������
	
	if(count>1)														//��β���
	{
		if(maxtime[0]*1.0/maxtime[1]<2.0)		//˵������ֵ�������������ֵ
			return maxtime[0];
		else																//˵�����ֵ̫�󣬿��ܲ����������شδ�ֵ
			return maxtime[1];
	}
	else
		return maxtime[0];
}


///////////////////////////////////////////////�жϴ�����/////////////////////////////////////////////////////////
//��ʱ���жϷ������ 
/*���ĸ���ʼ���ĸ���ģ�壺
void TIMx_IRQHandler(void)
{ 		    		  			    
	if(TIMx->SR&0X0001)			//����ж�
	{}				   
	TIMx->SR&=~(1<<0);			//����жϱ�־λ
}
*/

void TIM6_IRQHandler(void)		//���ڲ�������ִ��ʱ�䣬��STM32_MeasureFuncExctTimeByTimerx��˵��
{ 		    		  			    
	if(TIM6->SR&0X0001)			//����ж�
	{
		FunctionTimeMeasureIntCount++;
	}				   
	TIM6->SR&=~(1<<0);			//����жϱ�־λ
}

void TIM2_IRQHandler(void)
{ 		    		  			    
	if(TIM2->SR&0X0001)			//����ж�
	{}				   
	TIM2->SR&=~(1<<0);			//����жϱ�־λ
}
