#include <stm32f10x.h>
#include <led.h>
void led_init(void)
{	
	GPIOC->CRL&=0x00000000;
	GPIOC->CRL|=0x33333333;
	GPIOC->CRH&=0x00000000;
	GPIOC->CRH|=0x33333333;
	//GPIOC->ODR=0xffff;
}
