#include "exit.h"
#include "led.h"
#include "delay.h"

/*void EXTI9_5_IRQHandler(void)
{
	delay_ms(10);//����
	if(KEY3==0)	 //����S5
	{
		LED0=!LED0;
	
	}		 
	EXTI->PR=1<<5;  //���LINE5�ϵ��жϱ�־λ  
}*/

/*
void Exit_Init(void)
{
	RCC->APB2ENR|=1<<5;     //ʹ��PORTDʱ��
	GPIOD->CRL&=0XFF0FFFFF;//PD5���ó�����	  
	GPIOD->CRL|=0X00800000;   
	GPIOD->ODR|=1<<5;	   //PD5����
   Ex_NVIC_Config(GPIO_D,5,FTIR);//�½��ش���
	MY_NVIC_Init(2,1,EXTI9_5_IRQn,2);//��ռ2�������ȼ�1����2	   
}*/

