#include "MY_STM32F10X_Conf.h"
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ο�������guanfu_wang���롣
//ALIENTEKս��STM32������
//SCCB ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/14
//�汾��V1.0		    							    							  
//////////////////////////////////////////////////////////////////////////////////

//��ʼ��SCCB�ӿ�
//CHECK OK
void STM32_SCCBInit(void)
{											   
	RCC->APB2ENR|=1<<8;		//��ʹ������PORTGʱ��	   
  	RCC->APB2ENR|=1<<5;		//��ʹ������PORTDʱ��
	//PORTG13 IN	
	GPIOD->CRH&=0XFF0FFFFF;	//G->D
	GPIOD->CRH|=0X00800000;    
	GPIOD->ODR|=1<<13; 	
	//PD3 OUT  
	GPIOD->CRL&=0XFFFF0FFF;
	GPIOD->CRL|=0X00003000;    
	GPIOD->ODR|=1<<3; 
	
	SCCB_SDA_OUT();	   
}			 

//SCCB��ʼ�ź�
//��ʱ��Ϊ�ߵ�ʱ��,�����ߵĸߵ���,ΪSCCB��ʼ�ź�
//�ڼ���״̬��,SDA��SCL��Ϊ�͵�ƽ
void STM32_SCCBStart(void)
{
    SCCB_SDA=1;     //�����߸ߵ�ƽ	   
    SCCB_SCL=1;	    //��ʱ���߸ߵ�ʱ���������ɸ�����
    STM32_Delay_us(50);  
    SCCB_SDA=0;
    STM32_Delay_us(50);	 
    SCCB_SCL=0;	    //�����߻ָ��͵�ƽ��������������Ҫ	  
}

//SCCBֹͣ�ź�
//��ʱ��Ϊ�ߵ�ʱ��,�����ߵĵ͵���,ΪSCCBֹͣ�ź�
//����״����,SDA,SCL��Ϊ�ߵ�ƽ
void STM32_SCCBStop(void)
{
    SCCB_SDA=0;
    STM32_Delay_us(50);	 
    SCCB_SCL=1;	
    STM32_Delay_us(50); 
    SCCB_SDA=1;	
    STM32_Delay_us(50);
}  
//����NA�ź�
void STM32_SCCBNoAck(void)
{
	STM32_Delay_us(50);
	SCCB_SDA=1;	
	SCCB_SCL=1;	
	STM32_Delay_us(50);
	SCCB_SCL=0;	
	STM32_Delay_us(50);
	SCCB_SDA=0;	
	STM32_Delay_us(50);
}
//SCCB,д��һ���ֽ�
//����ֵ:0,�ɹ�;1,ʧ��. 
u8 STM32_SCCBWriteByte(u8 dat)
{
	u8 j,res;	 
	for(j=0;j<8;j++) //ѭ��8�η�������
	{
		if(dat&0x80)SCCB_SDA=1;	
		else SCCB_SDA=0;
		dat<<=1;
		STM32_Delay_us(50);
		SCCB_SCL=1;	
		STM32_Delay_us(50);
		SCCB_SCL=0;		   
	}			 
	SCCB_SDA_IN();		//����SDAΪ���� 
	STM32_Delay_us(50);
	SCCB_SCL=1;			//���յھ�λ,���ж��Ƿ��ͳɹ�
	STM32_Delay_us(50);
	if(SCCB_READ_SDA)res=1;  //SDA=1����ʧ�ܣ�����1
	else res=0;         //SDA=0���ͳɹ�������0
	SCCB_SCL=0;		 
	SCCB_SDA_OUT();		//����SDAΪ���    
	return res;  
}	 
//SCCB ��ȡһ���ֽ�
//��SCL��������,��������
//����ֵ:����������
u8 STM32_SCCBReadByte(void)
{
	u8 temp=0,j;    
	SCCB_SDA_IN();		//����SDAΪ����  
	for(j=8;j>0;j--) 	//ѭ��8�ν�������
	{		     	  
		STM32_Delay_us(50);
		SCCB_SCL=1;
		temp=temp<<1;
		if(SCCB_READ_SDA)temp++;   
		STM32_Delay_us(50);
		SCCB_SCL=0;
	}	
	SCCB_SDA_OUT();		//����SDAΪ���    
	return temp;
} 							    
//д�Ĵ���
//����ֵ:0,�ɹ�;1,ʧ��.
u8 STM32_SCCBWriteReg(u8 reg,u8 data)
{
	u8 res=0;
	STM32_SCCBStart(); 					//����SCCB����
	if(STM32_SCCBWriteByte(SCCB_ID))res=1;	//д����ID	  
	STM32_Delay_us(100);
  	if(STM32_SCCBWriteByte(reg))res=1;		//д�Ĵ�����ַ	  
	STM32_Delay_us(100);
  	if(STM32_SCCBWriteByte(data))res=1; 	//д����	 
  	STM32_SCCBStop();	  
  	return	res;
}		  					    
//���Ĵ���
//����ֵ:�����ļĴ���ֵ
u8 STM32_SCCBReadReg(u8 reg)
{
	u8 val=0;
	STM32_SCCBStart(); 				//����SCCB����
	STM32_SCCBWriteByte(SCCB_ID);		//д����ID	  
	STM32_Delay_us(100);	 
  STM32_SCCBWriteByte(reg);			//д�Ĵ�����ַ	  
	STM32_Delay_us(100);	  
	STM32_SCCBStop();   
	STM32_Delay_us(100);	   
	//���üĴ�����ַ�󣬲��Ƕ�
	STM32_SCCBStart();
	STM32_SCCBWriteByte(SCCB_ID|0X01);	//���Ͷ�����	  
	STM32_Delay_us(100);
  val=STM32_SCCBReadByte();		 	//��ȡ����
  STM32_SCCBNoAck();
  STM32_SCCBStop();
  return val;
}















