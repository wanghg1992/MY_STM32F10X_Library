#include "exit.h"
#include "led.h"
#include "delay.h"

/*void EXTI9_5_IRQHandler(void)
{
	delay_ms(10);//消抖
	if(KEY3==0)	 //按键S5
	{
		LED0=!LED0;
	
	}		 
	EXTI->PR=1<<5;  //清除LINE5上的中断标志位  
}*/

/*
void Exit_Init(void)
{
	RCC->APB2ENR|=1<<5;     //使能PORTD时钟
	GPIOD->CRL&=0XFF0FFFFF;//PD5设置成输入	  
	GPIOD->CRL|=0X00800000;   
	GPIOD->ODR|=1<<5;	   //PD5上拉
   Ex_NVIC_Config(GPIO_D,5,FTIR);//下降沿触发
	MY_NVIC_Init(2,1,EXTI9_5_IRQn,2);//抢占2，子优先级1，组2	   
}*/

