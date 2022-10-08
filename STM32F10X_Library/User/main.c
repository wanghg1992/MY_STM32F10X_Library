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
//	u16 i=0;																	//通用
//	u8 SendBuffer[100]="abcdefghijklmn";		//用于DMA串口数据传送实验
	u8 u8Data_a[8]={0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77};
	GPIO_InitTypeDef GPIO_InitStruct;					//用于IO口初始化
	
//时钟配置实验	
	STM32_ClockInit(9);												//配置系统时钟，给APB1和APB2提供时钟
	STM32_ClockEnable(APB2,RCC_APB2Periph_GPIOC);			//打开GPIOC端口时钟
	RCC->APB2ENR|=1<<5;     									//使能GPIOD时钟
	
//IO口配置及控制实验	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_9|GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;
	STM32_GPIOInit(GPIOC,&GPIO_InitStruct);	
	//直接寻址特殊功能寄存器
	//*((uint32_t*)(0x40021000+0x18))|=0x0010;  //时钟
	//*((uint32_t*)(0x40011000+0x04))=0x0010;   //10M推挽输出
	//*((uint32_t*)(0x40011000+0x0c))=0xffff;   //PC9输出0
	
//bitband实验	
	led0=0xffff;		//对位段区操作，等效为*((volatile unsigned long  *)(((GPIOC_BASE+12) & 0xF0000000)+0x2000000+
						//	(((GPIOC_BASE+12) &0xFFFFF)<<5)+(9<<2)))=0x0f0f;
	
//中断实验
	STM32_ExInterruptInit(GPIOD,GPIO_Pin_12,3,1,1,INTWORKMODE_FALLING);

//串口实验	USART1（PA9,TX;PA10,RX）
	STM32_USARTInit(USART1,72000000,9600);
	
//SysTick心跳、延时实验
	delay_init(72);	  		//延时初始化				加SYSTEM函数的话，为了让delay_ms，delay_us也能用
	STM32_SysTickInit(72000000,60);
	STM32_Delay_ms(500);
	printf("SysTick:%d;\r\n",STM32_MeasureFuncExctTimeBySysTick(delay,1));	//测量函数执行时间

//定时器实验
	STM32_TimerInit(TIM2,10000,7200);
	printf("TIM:%d;\r\n",STM32_MeasureFuncExctTimeByTimerx(TIM6,delay,1));	//测量函数执行时间
	
//ADC实验
	STM32_ADC1Init(ADCChannel0,SimpleTime_1_5);
	
//DMA实验
	//USART1->CR3=1<<7;           						//串口DMA使能发送（DMA发到串口）
	//STM32_DMAInit(DMA1_Channel4,(u32)&USART1->DR,(u32)SendBuffer,(u32)sizeof(SendBuffer));		//DMA串口发送初始化，并初次使能发送
	
//RTC实验
	//i=STM32_RTCInit();
	//if(i!=0) Uart1_PutString("error",strlen("error"));	//RTC初始化时钟失败，晶振有问题

//PWM实验
	//STM32_TIM4PWMInit(72000000,PB6,330,40);
	STM32_TIMxPWMInit(72000000,TIM4,TIMChannel1,330,0.4);
	
//CAN通信初始化
	STM32_CAN1Init(CANBaudRate_250K);
	STM32_CANSendData(CAN1,0x12,u8Data_a,8);
	
	while(1)
	{}
}


