/******************************************�⺯��ʹ��˵��*******************************************/
/*
	1���˿�������STM32F10Xϵ�е�Ƭ����
	2����Ҫʹ�ô˿��е�ĳ�����ܺ���������Ҫ������ͷ�ļ���Ȼ���ڹ����а���Ҫʹ�õĹ��ܺ���
		��Ӧ��.c�ļ���
	3��ÿ��ģ��ʹ��ϸ�ڣ��뿴��Ӧʹ��˵��
	4����Ҫʹ�ô˿⣬��ע�����ߣ�������
*/

#ifndef wang_MY_STM32F10X_Conf_hongge
#define wang_MY_STM32F10X_Conf_hongge


#include <stm32f10x.h>
#include "sys.h"									//����λ�β������÷���ͷ�ļ�

#define ui unsigned int
#define uc unsigned char

/******************************************ʱ������*************************************************/
/*���ܺ���ʹ��˵����
	1��STM32_ClockInit����ϵͳ��APB����ʱ�ӳ�ʼ��������HSE����Ƶ��ΪPLL���룬PLL���ϵͳʱ�ӣ�AHB
		����Ƶ��APB1����Ƶ��APB2����Ƶ��PllMultiplyΪPLL��Ƶ����SYSCLK=PLL����ʱ��*PllMultiply��
		APB1CLK=SYSCLK/2��APB2CLK=SYSCLK��
	2��STM32_ClockEnable����ʹ������ʱ�ӡ�AXBΪҪʹ��ʱ�ӵ�����������������RCC_AXBPeriphΪҪʹ��
		ʱ�ӵ������Ӧ��ʱ��λ�궨�壨�磺RCC_APB2Periph_AFIO����
*/
typedef enum
{
	AHB=0,APB1,APB2
}AXB_TypeDef;
void STM32_ClockInit(u8 PllMultiply);
void STM32_ClockEnable(AXB_TypeDef AXB,uint32_t RCC_AXBPeriph);


/******************************************GPIO����*************************************************/
/*���ܺ���ʹ��˵����
	1��STM32_GPIOInit������ʼ��GPIO���š�GPIOx:Ҫ��ʼ������������GPIO�ڣ�GPIOInit_pst:���ڳ�ʼ�����ŵĽṹ
		�壨���stm32f10x_gpio.h����
	2��STM32_GPIOWriteBit������IO�ڽ��а�λд������GPIOx:Ҫ����λ��������������GPIO�ڣ�GPIO_Pin:Ҫ��������
		�ţ�֧�ֻ��������BitVal:Ҫ����������Ϊ��ֵ��Bit_SET��Bit_RESET����
	3��STM32_GPIOReadBit������IO�ڽ��а�λ��������GPIOx:Ҫ����λ��������������GPIO�ڣ�GPIO_Pin:Ҫ���������š�
		����ֵ��Ҫ��ȡ���ŵĵ�ƽֵ��Bit_SET��Bit_RESET����
	4��STM32_GPIOWriteData������IO�ڽ�������д������GPIOx:Ҫ��������д������GPIO�ڣ�Data:Ҫд��IO�ڵ�ֵ��
	5��STM32_GPIOReadData������IO�ڽ��������������GPIOx:Ҫ���������������GPIO�ڡ�����ֵ��Ҫ��ȡ��GPIO�ڵ�
		���ŵ�ƽֵ��16λ������
*/
void STM32_GPIOInit(GPIO_TypeDef* GPIOx,GPIO_InitTypeDef* GPIOInit_pst);
void STM32_GPIOWriteBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, BitAction BitVal);
BitAction STM32_GPIOReadBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void STM32_GPIOWriteData(GPIO_TypeDef* GPIOx,u16 Data);
u16 STM32_GPIOReadData(GPIO_TypeDef* GPIOx);


/***************************************SysTick***************************************************/
/*���ܺ���ʹ��˵����
	1��STM32_SysTickInit����SysTickģ���ʼ����SystemClkΪ��ǰϵͳʱ�ӣ���λΪHz��HeartBeatFreqΪSysTick�ж�Ƶ�ʣ�
			��λΪHz��ȡֵ�������0.536441802978515625��ȡ���ʾ��Ч����������
	2��STM32_Delay_usΪus��ʱ������nusΪ��Ҫ��ʱ��us������USE_SYSTICK_INTģʽ�£���ʱ���ɴ�477204025us�����м�ֵ��
			��������u32λ�����ƣ�2^32/9=477204025��������һģʽ�£���ʱ���ɴ�1864135.111us���ܼ�������Чλ��24���ƣ���
	3��STM32_Delay_msΪms��ʱ������nmsΪ��Ҫ��ʱ��ms������USE_SYSTICK_INTģʽ�£���ʱ���ɴ�477204.025ms�����м�ֵ��
			��������u32λ�����ƣ�2^32/9000=477204.025��������һģʽ�£���ʱ���ɴ�1864.135111ms���ܼ�������Чλ��24���ƣ���
	4��STM32_MeasureFuncExctTimeBySysTickΪ����ִ��ʱ�����������FunctionΪ���⺯������countΪ���Դ�����ȡ�������ֵ����
			����ֵΪ���⺯����ִ��ʱ�䣬��λΪus������ֵ���477218588.444444us�������ԣ���SysTick�Ĳ����������ö�ʱ������ȷ��
	
ע��	
	1��ʹ��SysTick�жϵĻ����봦���жϺ���������void SysTick_Handler(void){if(SysTick->CTRL&(1<<16)){~~~;}}
	2��STM32_MeasureFuncExctTimeBySysTick��ʹ�������������ʼ���жϣ�ʵ���жϴ���������ʹFuncExctTimeMeasureIntCount��
			�ж��м�1�����£�
										//void SysTick_Handler(void)
										//{
										//	if(SysTick->CTRL&(1<<16))						//����������ж�
										//	{
										//		FuncExctTimeMeasureIntCount++;		//����ɾ
										//	}
										//}
*/
#define USE_SYSTICK_INT											//ʹ��SysTick�ж�
//void STM32_DelayInit(u32 SystemClk);
//void STM32_SysTickInterruptInit(u32 SystemClk,u32 nus);
void STM32_SysTickInit(u32 SystemClk,float HeartBeatFreq);
void STM32_Delay_us(u32 nus);
void STM32_Delay_ms(u32 nms);
#ifdef USE_SYSTICK_INT
u32 STM32_MeasureFuncExctTimeBySysTick(void(*Function)(void),u8 count);
#endif

///***************************************��ȷ��ʱ***************************************************/
///*���ܺ���ʹ��˵����
//	1��STM32_DelayInit������ʱģ���ʼ����SystemClkΪ��ǰϵͳʱ�ӣ���λΪHz
//	2��STM32_Delay_usΪus��ʱ������nusΪ��Ҫ��ʱ��us��
//	3��STM32_Delay_msΪms��ʱ������nmsΪ��Ҫ��ʱ��ms��
//	
//ע��	������ʱ����ʱҪע����ʱ��ȡֵ��Χ��ϵͳʱ��Ϊ72MHz������������ʱ1864ms��
//*/
//void STM32_DelayInit(u32 SystemClk);
//void STM32_Delay_us(u32 nus);
//void STM32_Delay_ms(u16 nms);


/******************************************NVICͨ������******************************************/
/*���ܺ���ʹ��˵����
	1��STM32_NVICSetVectorTable��������������ƫ�Ƶ�ַ��NVIC_VectTab:��ַ��NVIC_Offset:ƫ������
	2��STM32_NVICInit���������жϷ��顢���ȼ����ü��ж�ʹ�ܡ�NVIC_ChannelΪ�ж�ͨ�����жϱ�ţ���
		NVIC_GroupΪ�жϷ��飬NVIC_PreemptionPriorityΪ��ռ���ȼ���NVIC_SubPriorityΪ��Ӧ���ȼ���
*/
#define NVIC_GROUP 2
void STM32_NVICSetVectorTable(u32 NVIC_VectTab, u32 NVIC_Offset);			//��û������ʱ��Ū��
void STM32_NVICInit(u8 NVIC_Channel,u8 NVIC_Group,u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority);

/******************************************����ͨ��*********************************************/
/*���ܺ���ʹ��˵����
	1��STM32_USARTInit������ʼ�����ڡ�USARTxΪ��Ҫ��ʼ���Ĵ���(1~3)��PCLKΪAPBʱ�ӣ�BaudRateΪ������
	2��Uart_PutChar���������ַ����ݡ�USARTxΪ�跢�����ݵĴ��ڣ�chΪҪ���͵��ַ�����
	3��Uart_PutString���������ַ������ݡ�USARTxΪ�跢�����ݵĴ��ڣ�pΪ�跢�͵��ַ������׵�ַ��LengthΪ
		�跢�͵��ַ����ĳ���
	
ע�⣺�봦���жϺ���������void USART1_IRQHandler(void){if(USART1->SR&(1<<5)){~~~;}}
*/

void STM32_USARTInit(USART_TypeDef *USARTx,u32 PCLK,u32 BaudRate);
void STM32_USARTSendChar(USART_TypeDef *USARTx,u8 ch);							//����x�����ַ�����
void STM32_USARTSendString(USART_TypeDef *USARTx,char *p,u16 Length);			//����x�����ַ�������

/******************************************��ʱ��******************************************/
/*���ܺ���ʹ��˵����
	1��STM32_TimeInit�������ö�ʱ���жϣ����ö�ʱʱ��
		TimexΪ��ʱ��ѡ��TIM2~TIM7����arrΪ��������װֵ��pscΪ��ʱ��Ԥ��Ƶֵ��
	2,STM32_TimeInitUs�������ö�ʱ���жϣ����ö�ʱʱ�䡣TimexΪ��ʱ��ѡ��TIM2~TIM7����usΪҪ��ʱ��us����
	3��STM32_MeasureFuncExctTimeByTimerx�������Դ��⺯����ִ��ʱ�䡣TIMx:��ʱ����Function��Ҫ���Եĺ�����
		count:���ԵĴ�������β���ȡִ��ʱ�����ֵ��������ֵΪ���Ժ�����ִ��ʱ�䣬��λΪus�����
		Ϊ4294967296us���ܷ���ֵ����u32��λ�����ƣ��������ԣ��ö�ʱ���Ĳ�������û����SysTick��ȷ��
	
ע�⣺
	1���봦���жϺ���������void TIM2_IRQHandler(void){if(TIM2->SR&0X0001){~~~;}TIM2->SR&=~(1<<0);}
	2��STM32_MeasureFuncExctTimeByTimerx��ʹ������������ʵ����Ӧ��ʱ�����жϴ���������ʹFunctionTimeMeasureIntCount��
			�ж��м�1�����£�
										//void TIMx_IRQHandler(void)
										//{ 		    		  			    
										//	if(TIMx->SR&0X0001)			//����ж�
										//	{
										//		FunctionTimeMeasureIntCount++;
										//	}				   
										//	TIMx->SR&=~(1<<0);			//����жϱ�־λ
										//}
*/
void STM32_TimerInit(TIM_TypeDef *TIMx,u16 arr,u16 psc);
void STM32_TimerInitUs(TIM_TypeDef *TIMx,u32 us);				
u32 STM32_MeasureFuncExctTimeByTimerx(TIM_TypeDef *TIMx,void(*Function)(void),u8 count);

/******************************************ADת��******************************************/
/*���ܺ���ʹ��˵����
	1��STM32_ADC1Init������ʼ��ADCת������ADCChannelxΪ��Ҫʹ��ADת����ͨ����SimpleTimeΪADת������ʱ�䣻
	2��STM32_ADC1WatchDogInit������ʼ�����Ź��жϡ�ADCChannelxΪ���Ź����ͨ����HighThreshouldVoltΪ���
			�ĵ�ѹ���ޣ���λ��v����LowThreshouldVoltΪ���ĵ�ѹ���ޣ���λ��v����
	3��STM32_ADC1StartAConversion������ʼһ��ADת����
	4��STM32_ADC1GetVal������ȡADCChannelx����Ӧ��ͨ���ĵ�ѹת��ֵ������float���͵ĵ�ѹֵ��DoNewSimple��0��
			�������µĲ�����ֱ���üĴ����е�ֵת��Ϊ��ѹ��1�������µĲ�����������ת�����������üĴ����е�ֵ
			ת��Ϊ��ѹ��
	
ע�⣺1��VCCΪ��ǰADC�ο���ѹ��
			2�����ں���STM32_ADC1GetVal����������ж�ִ��ʱ��Ҫ��ϸߣ�
				��1����Ҫʹ�ò�������е�ѹת�����ȴ�ʱ��϶ࣨ��Ҫ�ǲ���+ת��ʱ�䣩����������ת�������жϣ����ж��ж�ȡת����ֵ��
						�Ա���ȴ�ʱ�䣻
				��2��������ʱ�併�ͣ����̴˳���ĵȴ�ʱ�䣨����취��֪���ܲ������Լ��ٵȴ�ʱ�䣬��û���ԣ���
*/
#define VCC 3.3
typedef enum				//ADCͨ�����壺0~17����18��ͨ��
{
	ADCChannel0=0,ADCChannel1,ADCChannel2,ADCChannel3,ADCChannel4,ADCChannel5,ADCChannel6,ADCChannel7,ADCChannel8
	,ADCChannel9,ADCChannel10,ADCChannel11,ADCChannel12,ADCChannel13,ADCChannel14,ADCChannel15
	,ADCChannel16,ADCChannel17
}ADCChannelx_TypeDef;
typedef enum				//����ʱ�䶨�壺1.5��7.5��13.5��28.5��41.5��55.5��71.5��239.5������
{
	SimpleTime_1_5=0,SimpleTime_7_5,SimpleTime_13_5,SimpleTime_28_5,SimpleTime_41_5
	,SimpleTime_55_5,SimpleTime_71_5,SimpleTime_239_5
}SimpleTime_TypeDef;
void STM32_ADC1Init(ADCChannelx_TypeDef ADCChannelx,SimpleTime_TypeDef SimpleTime);		//�˴���ADC1��PA��Ϊ��
void STM32_ADC1WatchDogInit(ADCChannelx_TypeDef ADCChannelx,float HighThreshouldVolt,float LowThreshouldVolt);									//�Թ���ͨ��Ϊ��float STM32_ADC1GetVal(void);
void STM32_ADC1StartAConversion(void);
float STM32_ADC1GetVal(u8 DoNewSimple);


/******************************************DMA�������ݴ���******************************************/
/*���ܺ���ʹ��˵����
	1��STM32_DMAInit������ʼ��DMA��ʹ�ã�DMA_CHxΪҪʹ�õ�DMAͨ����PeripheralAddrΪ�����ַ��MemoryAddrΪ
		�洢����ַ��DataLengthΪҪ���͵����ݵĳ���
	2��STM32_DMAEnableOnce��������һ��DMA���ͣ�DMA_CHxΪҪʹ�õ�DMAͨ��
	
ע�⣺
	1���ǵ�ʹ������ʱ�Ӻ�����DMA����λ����ʼ�������裩��
	2������һ��DMA���ͺ�ɶ������Ƿ�����ɽ��м�⣺while(1){if(DMA1->ISR&(1<<13)){DMA1->IFCR|=1<<13;break;}}
*/                                                         
void STM32_DMAInit(DMA_Channel_TypeDef *DMA_CHx,u32 PeripheralAddr,u32 MemoryAddr,u32 DataLength);	//�Ӵ洢��������
void STM32_DMAEnableOnce(DMA_Channel_TypeDef *DMA_CHx);		


/******************************************RTCʵʱʱ��ʵ��******************************************/
/*���ܺ���ʹ��˵����
	1��STM32_RTCInit������ʼ��ʵʱʱ��
	
ע�⣺�봦���жϺ���������void RTC_IRQHandler(void){if((RTC->CRL&1<<0)!=0){~~~;}RTC->CRL&=0xfffe;while((RTC->CRL&(1<<5))==0);}
*/
u8 STM32_RTCInit(void);


/*********************************************PWMʵ��***********************************************/
/*���ܺ���ʹ��˵����
	1��STM32_TIM4PWMInit���ڳ�ʼ��TIM4��Ӧ��PWM��TIMCLKΪ��ǰ����ʱ��Ƶ�ʣ�FreqenceΪ���Ƶ�ʣ�ȡֵΪ
		1KHz~14MHz���������ã���PulseWidthΪ�����ȣ�ȡֵΪ0~100.
	2��STM32_TIMxPWMInit���ڳ�ʼ��TIMx��Ӧ��PWM��TIMCLK:��ǰ����ʱ��Ƶ�ʣ�TIMChannelx:��ʱ��ͨ����
					Freqence:���Ƶ�ʣ�ȡֵΪ1Hz~3.4MHz���������ã���DutyRatio:PWMռ�ձȣ�ȡֵΪ0~1.0��
					ע�⣺����Freqence��Ϊ�������͵Ļ����������Ƶ�ʿ��Դﵽ0.01Hz.
	3,STM32_TIMxSetPWMDuty�����ı�TIMxһ��ͨ����PWM��ռ�ձ�.
*/
typedef enum
{
	PB6=6,PB7,PB8,PB9
}GPIOBPinx_TypeDef;
void STM32_TIM4PWMInit(u32 TIMCLK,GPIOBPinx_TypeDef GPIOBPinx,u32 Freqence,u8 PulseWidth);	//��TIM4��PB�����PWM������ӳ��Ϊ��

typedef enum
{
	TIMChannel1=0,TIMChannel2,TIMChannel3,TIMChannel4
}TIMChannelx_TypeDef;
void STM32_TIMxPWMInit(u32 TIMCLK,TIM_TypeDef* TIMx,TIMChannelx_TypeDef TIMChannelx,u32 Freqence,float DutyRatio);	//��ʱ�����PWM��ʼ��
void STM32_TIMxSetPWMDuty(TIM_TypeDef* TIMx,TIMChannelx_TypeDef TIMChannelx,float DutyRatio);	//���ö�ʱ�������PWM��ռ�ձ�

/*********************************************�ⲿ�ж�*********************************************/
/*���ܺ���ʹ��˵����
	1��STM32_ExInterruptInit�����ⲿ�жϳ�ʼ����GPIOxΪ��Ҫ�����ⲿ�жϵĶ˿ںţ�GPIO_PinΪ��Ҫ�����ⲿ�жϵ�
		���źţ�NVIC_GroupΪ�ж����ȼ����飬NVIC_PreemptionPriorityΪ��ռ���ȼ���NVIC_SubPriorityΪ��Ӧ���ȼ���
		ExIntWorkModeΪ�ⲿ�жϴ�����ʽ��

ע�⣺�봦���жϺ���������void EXTI15_10_IRQHandler(void){STM32_Delay_ms(20);~~~;EXTI->PR=1<<12;}

*/
typedef enum
{
	INTWORKMODE_RISING=1,INTWORKMODE_FALLING=2
}ExIntWorkMode_TypeDef;
void STM32_ExInterruptInit(GPIO_TypeDef *GPIOx,u16 GPIO_Pin,u8 NVIC_Group,u8 NVIC_PreemptionPriority,
				u8 NVIC_SubPriority,ExIntWorkMode_TypeDef ExIntWorkMode);

/********************************************SPIͨ��ʵ��***********************************************/
/*���ܺ���ʹ��˵����
	1��STM32_SPIxInit���ڳ�ʼ��SPI�ڣ�SPIxΪSPI�ڡ�
	2��STM32_SPIxSetSpeed����SPI�����ʷ�Ƶ��SPIxΪSPI�ڣ�������=Fpclk/��Ƶ����
	3��STM32_SPIxReadWriteByte����ͨ��SPI���߶�дһ���ֽڣ�SPIxΪSPI�ڣ�TxDataΪҪд��SPI�����ϵ����ݣ�
		�������ش�SPI�����϶�ȡ�����ݡ�
*/
// SPI���߲��������� 
typedef enum 
{
	SPIBaudFreqDiv_2=0,SPIBaudFreqDiv_4,SPIBaudFreqDiv_8,SPIBaudFreqDiv_16,
	SPIBaudFreqDiv_32,SPIBaudFreqDiv_64,SPIBaudFreqDiv_128,SPIBaudFreqDiv_256
}SPIBaudFreqDiv_TypeDef;
						  	    													  
void STM32_SPIxInit(SPI_TypeDef *SPIx);			 																							//��ʼ��SPI��
void STM32_SPIxSetBaudFreqDiv(SPI_TypeDef *SPIx,SPIBaudFreqDiv_TypeDef SPIBaudFreqDiv); 	//����SPI�����ʷ�Ƶ��������=Fpclk/��Ƶ��
u8 STM32_SPIxReadWriteByte(SPI_TypeDef *SPIx,u8 TxData);																	//SPI���߶�дһ���ֽ�

/*********************************************IICͨ��ʵ��***********************************************/
/*���ܺ���˵����
	1��STM32_IICInit����IICʹ�ó�ʼ����
	2��STM32_IICWriteOneByte������IIC�ӻ��ļĴ���д��һ���ֽڡ�IICSlaveAddrΪIIC�ӻ���ַ��RegAddrΪ�Ĵ�����ַ��
			ByteΪҪд����ֽڡ�
	3��STM32_IICWriteMultiByte������IIC�ӻ��ļĴ���д�����ֽڡ�IICSlaveAddrΪIIC�ӻ���ַ��RegAddrΪ�Ĵ�����ַ��
			pByteWriteΪҪд��Ķ���ֽڵ��׵�ַ��CountΪҪд����ֽڸ�����
	4��STM32_IICReadOneByte���ڴ�IIC�ӻ��ļĴ�����ȡһ���ֽڡ�IICSlaveAddrΪIIC�ӻ���ַ��RegAddrΪ�Ĵ�����ַ������ֵ
			Ϊ��ȡ���ֽڡ�
	5��STM32_IICReadMultiByte���ڴ�IIC�ӻ��ļĴ�����ȡ����ֽڡ�IICSlaveAddrΪIIC�ӻ���ַ��RegAddrΪ�Ĵ�����ַ��
			CountΪҪ��ȡ���ֽڵĸ�����pByteReadΪ��ȡ�Ķ���ֽڵı���λ�õ��׵�ַ��

	ע�⣺�˴�ʹ�õ���ģ��IIC��
*/

//IO��������
#define SDA_IN()  {GPIOC->CRH&=0XFFFF0FFF;GPIOC->CRH|=8<<12;}
#define SDA_OUT() {GPIOC->CRH&=0XFFFF0FFF;GPIOC->CRH|=3<<12;}

//IO����BitBand����	 
#define IIC_SCL    PCout(12) //SCL
#define IIC_SDA    PCout(11) //SDA	 
#define READ_SDA   PCin(11)  //����SDA 

//IIC�ӻ���ַ����
#define IIC_MPU6050_ADDRESS 0x68		//MPU6050��IIC��ַ��AD0�ӵأ�
#define IIC_HMC5883L_ADDRESS 0x1e		//HMC5883L��IIC��ַ

void STM32_IICInit(void);
void STM32_IICWriteOneByte(u8 IICSlaveAddr,u8 RegAddr,u8 Byte);
void STM32_IICWriteMultiByte(u8 IICSlaveAddr,u8 RegAddr,u8* pByteWrite,u8 Count);
u8 STM32_IICReadOneByte(u8 IICSlaveAddr,u8 RegAddr);
void STM32_IICReadMultiByte(u8 IICSlaveAddr,u8 RegAddr,u8 Count,u8* pByteRead);

/********************************************SCCBͨ��ʵ��***********************************************/
/*
	ע�⣺�˴�ʹ��ģ��SCCB
*/
#define SCCB_SDA_IN()  {GPIOD->CRH&=0XFF0FFFFF;GPIOD->CRH|=0X00800000;}
#define SCCB_SDA_OUT() {GPIOD->CRH&=0XFF0FFFFF;GPIOD->CRH|=0X00300000;}

//IO��������
#define SCCB_SCL    		PDout(3)	 		//SCL
#define SCCB_SDA    		PDout(13) 		//SDA	 

#define SCCB_READ_SDA   PDin(13)  		//����SDA    
#define SCCB_ID   			0X42  				//OV7670��ID

//SCCBͨ�Ż�������
void STM32_SCCBInit(void);
void STM32_SCCBStart(void);
void STM32_SCCBStop(void);
void STM32_SCCBNoAck(void);
u8 STM32_SCCBWriteByte(u8 dat);
u8 STM32_SCCBReadByte(void);
u8 STM32_SCCBWriteReg(u8 reg,u8 data);
u8 STM32_SCCBReadReg(u8 reg);

/************************************CANͨ��**********************************/
/*���ܺ���˵����
	1��STM32_CANInit����CAN1ʹ�ó�ʼ����CANBaudRate:�����ʡ�Ĭ��״̬������ӳ�䣬����������ģʽ��
			ʹ��CAN1�Ľ����жϣ������б�ʶ�����ˡ�
	2��STM32_CANSendData����CAN�������ݡ�CANx��CANģ�飻StdID����׼��ʶ����pData��Ҫ�������ݵ��׵�ַ��
			Count��Ҫ�������ݵĳ��ȡ�Ĭ��״̬�²�ʹ����չ��ʶ�������͵�Ϊ����֡������Զ��֡����
			
	ע�⣺���ڽ����жϺ����д�����յ������ݣ�����
	void USB_LP_CAN1_RX0_IRQHandler(void)
	{
		CanRxMsg CanRxMessage;
		CAN_Receive(CAN1,CAN_FIFO0,&CanRxMessage);
		printf("ID:%x;\r\n",CanRxMessage.StdId);
		printf("Data:%x,%x,%x,%x,%x,%x,%x,%x;\r\n",CanRxMessage.Data[0],CanRxMessage.Data[1],CanRxMessage.Data[2],
		CanRxMessage.Data[3],CanRxMessage.Data[4],CanRxMessage.Data[5],CanRxMessage.Data[6],CanRxMessage.Data[7]);
	}
*/

typedef enum
{
	CANBaudRate_5K=0,CANBaudRate_10K,CANBaudRate_20K,CANBaudRate_25K,CANBaudRate_40K,CANBaudRate_50K,
	CANBaudRate_62_5K,CANBaudRate_80K,CANBaudRate_100K,CANBaudRate_125K,CANBaudRate_200K,CANBaudRate_250K,
	CANBaudRate_400K,CANBaudRate_500K,CANBaudRate_800K,CANBaudRate_1M
}CANBaudRate_TypeDef;

void STM32_CAN1Init(CANBaudRate_TypeDef CANBaudRate);
void STM32_CANSendData(CAN_TypeDef* CANx,u16 StdID,u8 *pData,u8 Count);		//��������֡��ʹ�ñ�׼��ʶ��

#endif



