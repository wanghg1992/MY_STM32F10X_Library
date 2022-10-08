#include "MY_STM32F10X_Conf.h"
#include "delay.h"

/**************************************IIC基础通信函数********************************************/
//IICIO初始化
void STM32_IICInit(void)
{
	RCC->APB2ENR|=1<<4;//先使能外设IO PORTC时钟    	
	GPIOC->CRH=(GPIOC->CRH&(~(0xff<<(3*4))))+(0x33<<(3*4));	//GPIOC 11/12 设置成通用推挽输出
 
	IIC_SCL=1;
	IIC_SDA=1;	
}
//IIC起始信号
void STM32_IICStart(void)
{
	SDA_OUT();
	IIC_SCL=1;
	IIC_SDA=1;
	delay_us(2);//加的
	delay_us(2);
	IIC_SDA=0;
	IIC_SCL=0;//jia
	delay_us(2);
}
//IIC停止信号
void STM32_IICStop(void)
{
	SDA_OUT();
	IIC_SDA=0;
	IIC_SCL=0;
	delay_us(2);
	delay_us(2);//加的
	IIC_SCL=1;
	IIC_SDA=1;
	delay_us(2);
}
//IIC应答信号
void STM32_IICAck(void)
{
	IIC_SCL=0; //jia
	delay_us(2);//加的
	SDA_OUT();
	IIC_SDA=0;
	delay_us(2);
	//IIC_SDA=1;
	IIC_SCL=1;//jia
		delay_us(2);//加的
	IIC_SCL=0;//jia
}
//IIC非应答信号
void STM32_IICNAck(void)	//这个和上面的一样吗？
{
	IIC_SCL=0; //jia
	delay_us(2);//加的
	SDA_OUT();
	IIC_SDA=1;		//改为1
	delay_us(2);
	//IIC_SDA=1;
	IIC_SCL=1;//jia
	delay_us(2);//加的
	IIC_SCL=0;//jia
}
//IIC总线写字节
void STM32_IICWriteByte(u8 Byte)//形参为要发送的数据
{
	u8 i;
	SDA_OUT();
	IIC_SCL=0;
	//delay_us(1);
	for(i=0;i<8;i++)
	{
		IIC_SDA=((Byte<<i)&0x80)>>7;
		delay_us(2); //添加
		IIC_SCL=1;
		delay_us(2);
		IIC_SCL=0;
		delay_us(2);//加的
	}
}
//IIC等待应答信号。返回值：1，没等到应答信号；0，等到应答信号。
u8 STM32_IICWaitAck(void)
{
	u8 WaitTime=0;
	SDA_IN();
	IIC_SDA=1;delay_us(1);	//释放数据线   
	IIC_SCL=1;delay_us(1);	//与时钟线，等待从机的应答信号	
	while(READ_SDA)
	{
		if(++WaitTime>=255)
		{
			STM32_IICStop();
			return 1;
		}
	}
	IIC_SCL=0;//时钟输出0 	 加的 
	return 0;
}
//IIC总线读字节
u8 STM32_IICReadByte(u8 Ack)//形参为是否应答：0，不应答；1，应答。返回值为收到的数据。
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
		delay_us(1);	//添加
		IIC_SCL=0;
		delay_us(2); //添加
	}
	
	SDA_OUT();
	if(Ack)
		STM32_IICAck(); //发送ack信号
	else
		STM32_IICNAck();	//发送NoAck信号
	return Byte;		
}


/***************************************IIC上层通信函数*********************************************/
//函数作用：用于向IIC从机的寄存器写入一个字节
//参数说明：IICSlaveAddr为IIC从机地址，RegAddr为寄存器地址，Byte为要写入的字节
//测试结果：MPU6050,CHECK OK
//完成日期：15.09.28
void STM32_IICWriteOneByte(u8 IICSlaveAddr,u8 RegAddr,u8 Byte)
{
	STM32_IICStart();
	STM32_IICWriteByte((IICSlaveAddr<<1)&0xfe);//设备地址，写
	STM32_IICWaitAck();
	STM32_IICWriteByte(RegAddr);//寄存器地址
	STM32_IICWaitAck();
	STM32_IICWriteByte(Byte);//要写的数据
	STM32_IICWaitAck();
	STM32_IICStop();
}
//函数作用：用于向IIC从机的寄存器写入多个字节
//参数说明：IICSlaveAddr为IIC从机地址，RegAddr为寄存器地址，pByteWrite为要写入的多个字节的首地址，Count为要写入的字节个数
//测试结果：MPU6050,CHECK OK
//完成日期：15.09.28
void STM32_IICWriteMultiByte(u8 IICSlaveAddr,u8 RegAddr,u8* pByteWrite,u8 Count)
{
	u8 i;
	STM32_IICStart();
	STM32_IICWriteByte((IICSlaveAddr<<1)&0xfe);//设备地址，写
	STM32_IICWaitAck();
	STM32_IICWriteByte(RegAddr);//寄存器地址
	STM32_IICWaitAck();
	for(i=0;i<Count;i++)
	{
		STM32_IICWriteByte(*(pByteWrite+i));//要写的数据
		STM32_IICWaitAck();
	}
	STM32_IICStop();
}
//函数作用：用于从IIC从机的寄存器读取一个字节
//参数说明：IICSlaveAddr为IIC从机地址，RegAddr为寄存器地址。返回值为读取的字节
//测试结果：MPU6050,CHECK OK
//完成日期：15.09.28
u8 STM32_IICReadOneByte(u8 IICSlaveAddr,u8 RegAddr)
{
	u8 Byte;
	STM32_IICStart();
	STM32_IICWriteByte((IICSlaveAddr<<1)&0xfe);//设备地址，写
	STM32_IICWaitAck();
	STM32_IICWriteByte(RegAddr);//寄存器地址
	STM32_IICWaitAck();
	STM32_IICStart();
	STM32_IICWriteByte((IICSlaveAddr<<1)|0x01);//设备地址，读
	STM32_IICWaitAck();
	Byte=STM32_IICReadByte(0);		//非应答
	STM32_IICStop();	
	return Byte;
}
//函数作用：用于从IIC从机的寄存器读取多个字节
//参数说明：IICSlaveAddr为IIC从机地址，RegAddr为寄存器地址，Count为要读取的字节的个数，
//						pByteRead为读取的多个字节的保存位置的首地址
//测试结果：MPU6050,CHECK OK
//完成日期：15.09.28
void STM32_IICReadMultiByte(u8 IICSlaveAddr,u8 RegAddr,u8 Count,u8* pByteRead)
{
	u8 i;
	STM32_IICStart();
	STM32_IICWriteByte((IICSlaveAddr<<1)&0xfe);//设备地址，写
	STM32_IICWaitAck();
	STM32_IICWriteByte(RegAddr);//寄存器地址
	STM32_IICWaitAck();
	STM32_IICStart();
	STM32_IICWriteByte((IICSlaveAddr<<1)|0x01);//设备地址，读
	STM32_IICWaitAck();
	for(i=0;i<Count-1;i++)
		*(pByteRead+i)=STM32_IICReadByte(1);		//应答
	*(pByteRead+Count-1)=STM32_IICReadByte(0);//最后一个字节非应答
	STM32_IICStop();	
}
