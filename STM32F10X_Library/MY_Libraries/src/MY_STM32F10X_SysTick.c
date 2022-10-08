#include "MY_STM32F10X_Conf.h"
#include "usart.h"


static u16 count_us=0,count_ms=0;						//���ms��us������

//�������ã���ʱ����ʹ�ó�ʼ��
//����˵����SystemClk:��ǰϵͳʱ��
//���Խ����CHECK OK
//������ڣ�14.02.23
void STM32_DelayInit(u32 SystemClk)
{
	SysTick->CTRL&=~(1<<2);							//ѡ���ⲿʱ��Դ��STCLK��
	count_us=SystemClk/8/1000000;				//1us������
	count_ms=1000*count_us;							//1ms������
}

#ifdef USE_SYSTICK_INT
void STM32_SysTickInterruptInit(u32 SystemClk,u32 nus)	//nus<1864135.11111111111us
{
	SysTick->CTRL&=~(1<<2);										//ѡ���ⲿʱ��Դ��STCLK��
	SysTick->LOAD=SystemClk/8/1000000*nus;		//n us������
	SysTick->CTRL|=0x3;												//��ʱ����������ģʽ����ʹ������ж�
}
#endif

void STM32_SysTickInit(u32 SystemClk,float HeartBeatFreq)				//HeartBeatFreq>0.536441802978515625Hz
{
	STM32_DelayInit(SystemClk);
#ifdef USE_SYSTICK_INT
	STM32_SysTickInterruptInit(SystemClk,1.0*1000000/HeartBeatFreq);
#endif
}

#ifdef USE_SYSTICK_INT
//�������ã�΢����ʱ
//����˵����nus:Ҫ��ʱ��΢����
//���Խ����CHECK OK
//������ڣ�15.12.05
void STM32_Delay_us(u32 nus)
{
	u32 ticks=nus*count_us;										//����һ���Զ�ȡCTRL,�������ζ�ȡ�м�CTRL��ֵ�仯
	u32 told,tnow,tcnt=0;
	tnow=SysTick->VAL;
	while(tcnt<ticks)
	{
		told=tnow;
		tnow=SysTick->VAL;
		if(tnow<=told)	//��������û���
		{
			tcnt+=told-tnow;
		}
		else
		{
			tcnt+=told+SysTick->LOAD-tnow;
		}
	}
}
//void delay_us(u32 nus)
//{		
//	u32 ticks;
//	u32 told,tnow,tcnt=0;
//	u32 reload=SysTick->LOAD;	//LOAD��ֵ	    	 
//	ticks=nus*fac_us; 			//��Ҫ�Ľ�����	  		 
//	tcnt=0;
//	OSSchedLock();				//��ֹucos���ȣ���ֹ���us��ʱ
//	told=SysTick->VAL;        	//�ս���ʱ�ļ�����ֵ
//	while(1)
//	{
//		tnow=SysTick->VAL;	
//		if(tnow!=told)
//		{	    
//			if(tnow<told)tcnt+=told-tnow;//����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����.
//			else tcnt+=reload-tnow+told;	    
//			told=tnow;
//			if(tcnt>=ticks)break;//ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
//		}  
//	};
//	OSSchedUnlock();			//����ucos���� 									    
//}

//�������ã�������ʱ
//����˵����nms:Ҫ��ʱ�ĺ�����
//���Խ����CHECK OK
//������ڣ�15.12.05
void STM32_Delay_ms(u32 nms)				//�������ܿ��Լ���������ʱ��
{
	u32 ticks=nms*count_ms;										//����һ���Զ�ȡCTRL,�������ζ�ȡ�м�CTRL��ֵ�仯
	u32 told,tnow,tcnt=0;
	tnow=SysTick->VAL;
	while(tcnt<ticks)
	{
		told=tnow;
		tnow=SysTick->VAL;
		if(tnow<=told)	//��������û���
		{
			tcnt+=told-tnow;
		}
		else
		{
			tcnt+=told+SysTick->LOAD-tnow;
		}
	}
}

//�������ã�����ִ��ʱ�����
//����˵����FunctionΪ���⺯������countΪ���Դ�����ȡ�������ֵ��������ֵΪ���⺯����ִ��ʱ�䣬��λΪus
//���Խ����CHECK OK
//������ڣ�15.12.06
//ע�⣺����ʱ�����Ĳ��������1us���ҵ���ԭ����������¼�����
//			1����ʱ������ʱ��׼
//			2����ʱ�����еĳ�ʼ��������ʱ��
//			3�����������
//	��������С�����ô����������ǣ���������
static u32 FuncExctTimeMeasureIntCount=0;		//��Ҫ�����õ����ж��м�1
u32 STM32_MeasureFuncExctTimeBySysTick(void(*Function)(void),u8 count)	//����ֵС��477218588.444444us
{
	u8 i=0;
	u32 maxtcnt[2]={0,0},told=0,tnew=0,tcnt=0;
	for(i=0;i<count;i++)
	{
		FuncExctTimeMeasureIntCount=0;
		told=SysTick->VAL;
		Function();
		tnew=SysTick->VAL;
		tcnt=SysTick->LOAD*FuncExctTimeMeasureIntCount+told-tnew;			//-SysTick->VAL��ŵ�ǰ�棬��С����������ϵͳ�ۺ���������ܴﵽ1us�����ǲ��У���ԭ���ɡ�����ԭ���ˣ������������
		if(maxtcnt[0]<tcnt)				//��¼���ʹδ��ֵ
		{
			maxtcnt[1]=maxtcnt[0];
			maxtcnt[0]=tcnt;
		}
		else if(maxtcnt[1]<tcnt)
		{
			maxtcnt[1]=tcnt;
		}
	}
	
	if(count>1)														//��β���
	{
		if(maxtcnt[0]*1.0/maxtcnt[1]<2.0)		//˵������ֵ�������������ֵ
			return maxtcnt[0]/count_us;			//-1��Ӧ��������ָ����ʵ����Ի���1us���������,Ϊʲô��1us������ʱ����������ü�1�ˣ����ҵ������У����ǲ�����ȫȥ������ԭ���ɡ�����ԭ���ˣ������������
		else																//˵�����ֵ̫�󣬿��ܲ����������شδ�ֵ
			return maxtcnt[1]/count_us;
	}
	else
		return maxtcnt[0]/count_us;
}

void SysTick_Handler(void)
{
	if(SysTick->CTRL&(1<<16))						//����������ж�
	{
		FuncExctTimeMeasureIntCount++;		//����ɾ
	}
}

#else
//�������ã�΢����ʱ
//����˵����nus:Ҫ��ʱ��΢����
//���Խ����CHECK OK
//������ڣ�14.02.23
void STM32_Delay_us(u32 nus)
{
	u32 i=0;										//����һ���Զ�ȡCTRL,�������ζ�ȡ�м�CTRL��ֵ�仯
	SysTick->LOAD=nus*count_us;						//��������װֵ���Լ�ʱnus��
	SysTick->VAL=0;									//���㵹������������ֵ
	SysTick->CTRL|=1<<0;							//ʹ�ܵδ�ʱ������ʼ����
	
	//while((SysTick->CTRL&0x1)&&((SysTick->CTRL>>16)&0x1)==0);		//�ȴ��������������㣨�����������⣩
	do
	{
		i=SysTick->CTRL;
	}
	while((i&0x01)&&!(i&(1<<16)));					//�ȴ�ʱ�䵽�� 
	
	SysTick->CTRL&=~(1<<0);							//�رռ�����
	SysTick->VAL=0;									//��ռ�������ֵ
}

//�������ã�������ʱ
//����˵����nms:Ҫ��ʱ�ĺ�����
//���Խ����CHECK OK
//������ڣ�14.02.23
void STM32_Delay_ms(u32 nms)
{
	u32 i=0;										//����һ���Զ�ȡCTRL,�������ζ�ȡ�м�CTRL��ֵ�仯
	SysTick->LOAD=nms*count_ms;						//��������װֵ���Լ�ʱnus��
	SysTick->VAL=0;									//���㵹������������ֵ
	SysTick->CTRL|=1<<0;							//ʹ�ܵδ�ʱ������ʼ����
	
	//while((SysTick->CTRL&0x1)&&((SysTick->CTRL>>16)&0x1)==0);		//�ȴ��������������㣨�����������⣩
	do
	{
		i=SysTick->CTRL;
	}
	while((i&0x01)&&!(i&(1<<16)));					//�ȴ�ʱ�䵽��   

	SysTick->CTRL&=~(1<<0);							//�رռ�����
	SysTick->VAL=0;									//��ռ�������ֵ
}
#endif





