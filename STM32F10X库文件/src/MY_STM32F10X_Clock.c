#include "MY_STM32F10X_Conf.h"

//�������ã���λ����ʱ�ӼĴ�������ʱ��ϸ�����ú�����£�
//����˵�����޲���
//���Խ����CHECK OK
//������ڣ�14.02.26
void STM32_ClockReset(void)
{
	RCC->APB1RSTR=0x00000000;	//��λ����
	RCC->APB2RSTR=0x00000000;	//��λ����
	
	RCC->AHBENR=0x00000014;		//˯��ģʽʱ����ӿڵ�·ʱ�Ӻ�SRAMʱ�ӿ����������ر�
	RCC->APB1ENR=0x00000000;	//����ʱ�ӹر�
	RCC->APB2ENR=0x00000000;	//����ʱ�ӹر�
	RCC->CR|=1<<0;				//ʹ���ڲ�����ʱ��HSI
	RCC->CFGR&=0xf8ff0000;		//��λMCO[2:0]��ADCPRE[1:0]��PPRE2[2:0]��PPRE1[2:0]��HPRE[3:0]��SWS[1:0]��SW[1:0]
	RCC->CR&=0xfef2ffff;		//��λPLLON��CSSON��HSEBYP��HSERDY��HSEON��
	RCC->CFGR&=0xff80ffff;		//��λUSBPRE��PLLMUL[3:0]��PLLXTPRE��PLLSRC
	RCC->CIR=0x00000000;		//�ر������жϣ��ر�PLL�����жϡ�HSE�����жϡ�HSI�����жϡ�LSE�����жϡ�LSI�����жϣ�������HSEʱ��ʧЧ�����İ�ȫϵͳ�жϣ���PLL����������ʱ�Ӿ����жϣ����ⲿ4-16MHz����������ʱ�Ӿ����жϣ����ڲ�8MHzRC����������ʱ�Ӿ����жϣ����ⲿ32KHz����������ʱ�Ӿ����жϣ����ڲ�40KHzRC����������ʱ�Ӿ����ж�
	
	//����������
#ifdef VECT_TAB_RAM
	STM32_NVICSetVectorTable(NVIC_VectTab_RAM,0x0);
#else
	STM32_NVICSetVectorTable(NVIC_VectTab_FLASH,0x0);
#endif
}

//�������ã�ʱ�����ó�ʼ��
//����˵����PllMultiply:PLL��Ƶ��
//���Խ����CHECK OK
//������ڣ�14.02.26
void STM32_ClockInit(u8 PllMultiply)
{
	u8 i=0;														//����RCC->CFGR���м䴦��
	STM32_ClockReset();											//��λ������������
	RCC->CR|=1<<16;												//HSE��������
	while((RCC->CR&(1<<17))==0);								//�ȴ��ⲿ����׼������
	RCC->CFGR=(RCC->CFGR&(~(0xf<<4)))+(0x0<<4);					//AHB����Ƶ��
	RCC->CFGR=(RCC->CFGR&(~(0x7<<8)))+(0x4<<8);					//APB1����Ƶ
	RCC->CFGR=(RCC->CFGR&(~(0x7<<11)))+(0x0<<11);				//APB1����Ƶ
	RCC->CFGR=(RCC->CFGR&(~(0xf<<18)))+((PllMultiply-2)<<18);	//����PLL��Ƶ
	RCC->CFGR|=1<<16;											//HSEʱ����ΪPLL����ʱ��
	FLASH->ACR|=0x32;											//FLASH 2����ʱ���ڣ���������
	
	RCC->CR|=1<<24;												//PLLʹ��
	while((RCC->CR&(1<<25))==0);								//�ȴ�PLLʱ�Ӿ���
	RCC->CFGR=(RCC->CFGR&(~(0x3<<0)))+(0x2<<0);					//ѡ��PLL�����Ϊϵͳʱ��
	do															//�ȴ�PLL���ѡ��Ϊϵͳʱ�ӳɹ�
	{
		i=(RCC->CFGR>>2)&0x3;
	}while(i!=0x2);
}

//�������ã�����ʱ��ʹ��
//����˵����AXB:��������RCC_AXBPeriph:����ʱ��λ�궨�壨�磺RCC_APB2Periph_AFIO��
//���Խ����NOT CHECK
//������ڣ�14.03.01
void STM32_ClockEnable(AXB_TypeDef AXB,uint32_t RCC_AXBPeriph)
{
	if(AHB==AXB)
		RCC->AHBENR|=RCC_AXBPeriph;
	else if(APB1==AXB)
		RCC->APB1ENR|=RCC_AXBPeriph;
	else if(APB2==AXB)
		RCC->APB2ENR|=RCC_AXBPeriph;
		
}
