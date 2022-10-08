#include "MY_STM32F10X_Conf.h"
#include "delay.h"

/**************************************IIC����ͨ�ź���********************************************/
//IICIO��ʼ��
void STM32_IICInit(void)
{
	RCC->APB2ENR|=1<<4;//��ʹ������IO PORTCʱ��    	
	GPIOC->CRH=(GPIOC->CRH&(~(0xff<<(3*4))))+(0x33<<(3*4));	//GPIOC 11/12 ���ó�ͨ���������
 
	IIC_SCL=1;
	IIC_SDA=1;	
}
//IIC��ʼ�ź�
void STM32_IICStart(void)
{
	SDA_OUT();
	IIC_SCL=1;
	IIC_SDA=1;
	delay_us(2);//�ӵ�
	delay_us(2);
	IIC_SDA=0;
	IIC_SCL=0;//jia
	delay_us(2);
}
//IICֹͣ�ź�
void STM32_IICStop(void)
{
	SDA_OUT();
	IIC_SDA=0;
	IIC_SCL=0;
	delay_us(2);
	delay_us(2);//�ӵ�
	IIC_SCL=1;
	IIC_SDA=1;
	delay_us(2);
}
//IICӦ���ź�
void STM32_IICAck(void)
{
	IIC_SCL=0; //jia
	delay_us(2);//�ӵ�
	SDA_OUT();
	IIC_SDA=0;
	delay_us(2);
	//IIC_SDA=1;
	IIC_SCL=1;//jia
		delay_us(2);//�ӵ�
	IIC_SCL=0;//jia
}
//IIC��Ӧ���ź�
void STM32_IICNAck(void)	//����������һ����
{
	IIC_SCL=0; //jia
	delay_us(2);//�ӵ�
	SDA_OUT();
	IIC_SDA=1;		//��Ϊ1
	delay_us(2);
	//IIC_SDA=1;
	IIC_SCL=1;//jia
	delay_us(2);//�ӵ�
	IIC_SCL=0;//jia
}
//IIC����д�ֽ�
void STM32_IICWriteByte(u8 Byte)//�β�ΪҪ���͵�����
{
	u8 i;
	SDA_OUT();
	IIC_SCL=0;
	//delay_us(1);
	for(i=0;i<8;i++)
	{
		IIC_SDA=((Byte<<i)&0x80)>>7;
		delay_us(2); //���
		IIC_SCL=1;
		delay_us(2);
		IIC_SCL=0;
		delay_us(2);//�ӵ�
	}
}
//IIC�ȴ�Ӧ���źš�����ֵ��1��û�ȵ�Ӧ���źţ�0���ȵ�Ӧ���źš�
u8 STM32_IICWaitAck(void)
{
	u8 WaitTime=0;
	SDA_IN();
	IIC_SDA=1;delay_us(1);	//�ͷ�������   
	IIC_SCL=1;delay_us(1);	//��ʱ���ߣ��ȴ��ӻ���Ӧ���ź�	
	while(READ_SDA)
	{
		if(++WaitTime>=255)
		{
			STM32_IICStop();
			return 1;
		}
	}
	IIC_SCL=0;//ʱ�����0 	 �ӵ� 
	return 0;
}
//IIC���߶��ֽ�
u8 STM32_IICReadByte(u8 Ack)//�β�Ϊ�Ƿ�Ӧ��0����Ӧ��1��Ӧ�𡣷���ֵΪ�յ������ݡ�
{
	u8 i,Byte=0;
	SDA_IN();
	IIC_SCL=0;
	delay_us(2);
	for(i=0;i<8;i++)
	{
		IIC_SCL=1;
		delay_us(2);
		Byte<<=1;//jia
		if(READ_SDA)
			Byte+=1;
		//Byte<<=1;
		delay_us(1);	//���
		IIC_SCL=0;
		delay_us(2); //���
	}
	
	SDA_OUT();
	if(Ack)
		STM32_IICAck(); //����ack�ź�
	else
		STM32_IICNAck();	//����NoAck�ź�
	return Byte;		
}


/***************************************IIC�ϲ�ͨ�ź���*********************************************/
//�������ã�������IIC�ӻ��ļĴ���д��һ���ֽ�
//����˵����IICSlaveAddrΪIIC�ӻ���ַ��RegAddrΪ�Ĵ�����ַ��ByteΪҪд����ֽ�
//���Խ����MPU6050,CHECK OK
//������ڣ�15.09.28
void STM32_IICWriteOneByte(u8 IICSlaveAddr,u8 RegAddr,u8 Byte)
{
	STM32_IICStart();
	STM32_IICWriteByte((IICSlaveAddr<<1)&0xfe);//�豸��ַ��д
	STM32_IICWaitAck();
	STM32_IICWriteByte(RegAddr);//�Ĵ�����ַ
	STM32_IICWaitAck();
	STM32_IICWriteByte(Byte);//Ҫд������
	STM32_IICWaitAck();
	STM32_IICStop();
}
//�������ã�������IIC�ӻ��ļĴ���д�����ֽ�
//����˵����IICSlaveAddrΪIIC�ӻ���ַ��RegAddrΪ�Ĵ�����ַ��pByteWriteΪҪд��Ķ���ֽڵ��׵�ַ��CountΪҪд����ֽڸ���
//���Խ����MPU6050,CHECK OK
//������ڣ�15.09.28
void STM32_IICWriteMultiByte(u8 IICSlaveAddr,u8 RegAddr,u8* pByteWrite,u8 Count)
{
	u8 i;
	STM32_IICStart();
	STM32_IICWriteByte((IICSlaveAddr<<1)&0xfe);//�豸��ַ��д
	STM32_IICWaitAck();
	STM32_IICWriteByte(RegAddr);//�Ĵ�����ַ
	STM32_IICWaitAck();
	for(i=0;i<Count;i++)
	{
		STM32_IICWriteByte(*(pByteWrite+i));//Ҫд������
		STM32_IICWaitAck();
	}
	STM32_IICStop();
}
//�������ã����ڴ�IIC�ӻ��ļĴ�����ȡһ���ֽ�
//����˵����IICSlaveAddrΪIIC�ӻ���ַ��RegAddrΪ�Ĵ�����ַ������ֵΪ��ȡ���ֽ�
//���Խ����MPU6050,CHECK OK
//������ڣ�15.09.28
u8 STM32_IICReadOneByte(u8 IICSlaveAddr,u8 RegAddr)
{
	u8 Byte;
	STM32_IICStart();
	STM32_IICWriteByte((IICSlaveAddr<<1)&0xfe);//�豸��ַ��д
	STM32_IICWaitAck();
	STM32_IICWriteByte(RegAddr);//�Ĵ�����ַ
	STM32_IICWaitAck();
	STM32_IICStart();
	STM32_IICWriteByte((IICSlaveAddr<<1)|0x01);//�豸��ַ����
	STM32_IICWaitAck();
	Byte=STM32_IICReadByte(0);		//��Ӧ��
	STM32_IICStop();	
	return Byte;
}
//�������ã����ڴ�IIC�ӻ��ļĴ�����ȡ����ֽ�
//����˵����IICSlaveAddrΪIIC�ӻ���ַ��RegAddrΪ�Ĵ�����ַ��CountΪҪ��ȡ���ֽڵĸ�����
//						pByteReadΪ��ȡ�Ķ���ֽڵı���λ�õ��׵�ַ
//���Խ����MPU6050,CHECK OK
//������ڣ�15.09.28
void STM32_IICReadMultiByte(u8 IICSlaveAddr,u8 RegAddr,u8 Count,u8* pByteRead)
{
	u8 i;
	STM32_IICStart();
	STM32_IICWriteByte((IICSlaveAddr<<1)&0xfe);//�豸��ַ��д
	STM32_IICWaitAck();
	STM32_IICWriteByte(RegAddr);//�Ĵ�����ַ
	STM32_IICWaitAck();
	STM32_IICStart();
	STM32_IICWriteByte((IICSlaveAddr<<1)|0x01);//�豸��ַ����
	STM32_IICWaitAck();
	for(i=0;i<Count-1;i++)
		*(pByteRead+i)=STM32_IICReadByte(1);		//Ӧ��
	*(pByteRead+Count-1)=STM32_IICReadByte(0);//���һ���ֽڷ�Ӧ��
	STM32_IICStop();	
}
