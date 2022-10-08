#include "MY_STM32F10X_Conf.h"
#include "usart.h"
#include "delay.h"

#define led0 PCout(9)

void delay(void)
{
	STM32_Delay_ms(180);
	//STM32_Delay_ms(122864);
}

int main(void)
{	
//	u16 i=0;																	//ͨ��
//	u8 SendBuffer[100]="abcdefghijklmn";		//����DMA�������ݴ���ʵ��
	u8 u8Data_a[8]={0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77};
	GPIO_InitTypeDef GPIO_InitStruct;					//����IO�ڳ�ʼ��
	
//ʱ������ʵ��	
	STM32_ClockInit(9);												//����ϵͳʱ�ӣ���APB1��APB2�ṩʱ��
	STM32_ClockEnable(APB2,RCC_APB2Periph_GPIOC);			//��GPIOC�˿�ʱ��
	RCC->APB2ENR|=1<<5;     									//ʹ��GPIODʱ��
	
//IO�����ü�����ʵ��	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_9|GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;
	STM32_GPIOInit(GPIOC,&GPIO_InitStruct);	
	//ֱ��Ѱַ���⹦�ܼĴ���
	//*((uint32_t*)(0x40021000+0x18))|=0x0010;  //ʱ��
	//*((uint32_t*)(0x40011000+0x04))=0x0010;   //10M�������
	//*((uint32_t*)(0x40011000+0x0c))=0xffff;   //PC9���0
	
//bitbandʵ��	
	led0=0xffff;		//��λ������������ЧΪ*((volatile unsigned long  *)(((GPIOC_BASE+12) & 0xF0000000)+0x2000000+
						//	(((GPIOC_BASE+12) &0xFFFFF)<<5)+(9<<2)))=0x0f0f;
	
//�ж�ʵ��
	STM32_ExInterruptInit(GPIOD,GPIO_Pin_12,3,1,1,INTWORKMODE_FALLING);

//����ʵ��	USART1��PA9,TX;PA10,RX��
	STM32_USARTInit(USART1,72000000,9600);
	
//SysTick��������ʱʵ��
	delay_init(72);	  		//��ʱ��ʼ��				��SYSTEM�����Ļ���Ϊ����delay_ms��delay_usҲ����
	STM32_SysTickInit(72000000,60);
	STM32_Delay_ms(500);
	printf("SysTick:%d;\r\n",STM32_MeasureFuncExctTimeBySysTick(delay,1));	//��������ִ��ʱ��

//��ʱ��ʵ��
	STM32_TimerInit(TIM2,10000,7200);
	printf("TIM:%d;\r\n",STM32_MeasureFuncExctTimeByTimerx(TIM6,delay,1));	//��������ִ��ʱ��
	
//ADCʵ��
	STM32_ADC1Init(ADCChannel0,SimpleTime_1_5);
	
//DMAʵ��
	//USART1->CR3=1<<7;           						//����DMAʹ�ܷ��ͣ�DMA�������ڣ�
	//STM32_DMAInit(DMA1_Channel4,(u32)&USART1->DR,(u32)SendBuffer,(u32)sizeof(SendBuffer));		//DMA���ڷ��ͳ�ʼ����������ʹ�ܷ���
	
//RTCʵ��
	//i=STM32_RTCInit();
	//if(i!=0) Uart1_PutString("error",strlen("error"));	//RTC��ʼ��ʱ��ʧ�ܣ�����������

//PWMʵ��
	//STM32_TIM4PWMInit(72000000,PB6,330,40);
	STM32_TIMxPWMInit(72000000,TIM4,TIMChannel1,330,0.4);
	
//CANͨ�ų�ʼ��
	STM32_CAN1Init(CANBaudRate_250K);
	STM32_CANSendData(CAN1,0x12,u8Data_a,8);
	
	while(1)
	{}
}


