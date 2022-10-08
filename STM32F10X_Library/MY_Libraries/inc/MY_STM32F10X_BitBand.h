#ifndef wang_MY_STM32F10X_BitBand_hongge
#define wang_MY_STM32F10X_BitBand_hongge

/*λ�β�����������51��Ƭ����������λ���������ο���Cortex(TM)-M3�����ο��ֲᡷ�����ģ�������35~38ҳ*/
//���Խ����CHECK OK
//������ڣ�14.3.1

/*
	�÷�ʾ����
		#define key0 PAIn(0)
		#define led0 PBOut(0)
		if(key0==0)				//��ȡλ
		{
			led0=1;				//����λ
		}
*/

//���
#define PAOut(x) *(volatile u32*)(PERIPH_BASE+0x02000000+(GPIOA_BASE+0x0c-PERIPH_BASE)*32+x*4)
#define PBOut(x) *(volatile u32*)(PERIPH_BASE+0x02000000+(GPIOB_BASE+0x0c-PERIPH_BASE)*32+x*4)
#define PCOut(x) *(volatile u32*)(PERIPH_BASE+0x02000000+(GPIOC_BASE+0x0c-PERIPH_BASE)*32+x*4)
#define PDOut(x) *(volatile u32*)(PERIPH_BASE+0x02000000+(GPIOD_BASE+0x0c-PERIPH_BASE)*32+x*4)
#define PEOut(x) *(volatile u32*)(PERIPH_BASE+0x02000000+(GPIOE_BASE+0x0c-PERIPH_BASE)*32+x*4)
#define PFOut(x) *(volatile u32*)(PERIPH_BASE+0x02000000+(GPIOF_BASE+0x0c-PERIPH_BASE)*32+x*4)
#define PGOut(x) *(volatile u32*)(PERIPH_BASE+0x02000000+(GPIOG_BASE+0x0c-PERIPH_BASE)*32+x*4)

//����
#define PAIn(x) *(volatile u32*)(PERIPH_BASE+0x02000000+(GPIOA_BASE+0x08-PERIPH_BASE)*32+x*4)
#define PBIn(x) *(volatile u32*)(PERIPH_BASE+0x02000000+(GPIOB_BASE+0x08-PERIPH_BASE)*32+x*4)
#define PCIn(x) *(volatile u32*)(PERIPH_BASE+0x02000000+(GPIOC_BASE+0x08-PERIPH_BASE)*32+x*4)
#define PDIn(x) *(volatile u32*)(PERIPH_BASE+0x02000000+(GPIOD_BASE+0x08-PERIPH_BASE)*32+x*4)
#define PEIn(x) *(volatile u32*)(PERIPH_BASE+0x02000000+(GPIOE_BASE+0x08-PERIPH_BASE)*32+x*4)
#define PFIn(x) *(volatile u32*)(PERIPH_BASE+0x02000000+(GPIOF_BASE+0x08-PERIPH_BASE)*32+x*4)
#define PGIn(x) *(volatile u32*)(PERIPH_BASE+0x02000000+(GPIOG_BASE+0x08-PERIPH_BASE)*32+x*4)


#endif
