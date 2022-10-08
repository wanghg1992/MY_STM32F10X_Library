#include "MY_STM32F10X_Conf.h"
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序参考自网友guanfu_wang代码。
//ALIENTEK战舰STM32开发板
//SCCB 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/14
//版本：V1.0		    							    							  
//////////////////////////////////////////////////////////////////////////////////

//初始化SCCB接口
//CHECK OK
void STM32_SCCBInit(void)
{											   
	RCC->APB2ENR|=1<<8;		//先使能外设PORTG时钟	   
  	RCC->APB2ENR|=1<<5;		//先使能外设PORTD时钟
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

//SCCB起始信号
//当时钟为高的时候,数据线的高到低,为SCCB起始信号
//在激活状态下,SDA和SCL均为低电平
void STM32_SCCBStart(void)
{
    SCCB_SDA=1;     //数据线高电平	   
    SCCB_SCL=1;	    //在时钟线高的时候数据线由高至低
    STM32_Delay_us(50);  
    SCCB_SDA=0;
    STM32_Delay_us(50);	 
    SCCB_SCL=0;	    //数据线恢复低电平，单操作函数必要	  
}

//SCCB停止信号
//当时钟为高的时候,数据线的低到高,为SCCB停止信号
//空闲状况下,SDA,SCL均为高电平
void STM32_SCCBStop(void)
{
    SCCB_SDA=0;
    STM32_Delay_us(50);	 
    SCCB_SCL=1;	
    STM32_Delay_us(50); 
    SCCB_SDA=1;	
    STM32_Delay_us(50);
}  
//产生NA信号
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
//SCCB,写入一个字节
//返回值:0,成功;1,失败. 
u8 STM32_SCCBWriteByte(u8 dat)
{
	u8 j,res;	 
	for(j=0;j<8;j++) //循环8次发送数据
	{
		if(dat&0x80)SCCB_SDA=1;	
		else SCCB_SDA=0;
		dat<<=1;
		STM32_Delay_us(50);
		SCCB_SCL=1;	
		STM32_Delay_us(50);
		SCCB_SCL=0;		   
	}			 
	SCCB_SDA_IN();		//设置SDA为输入 
	STM32_Delay_us(50);
	SCCB_SCL=1;			//接收第九位,以判断是否发送成功
	STM32_Delay_us(50);
	if(SCCB_READ_SDA)res=1;  //SDA=1发送失败，返回1
	else res=0;         //SDA=0发送成功，返回0
	SCCB_SCL=0;		 
	SCCB_SDA_OUT();		//设置SDA为输出    
	return res;  
}	 
//SCCB 读取一个字节
//在SCL的上升沿,数据锁存
//返回值:读到的数据
u8 STM32_SCCBReadByte(void)
{
	u8 temp=0,j;    
	SCCB_SDA_IN();		//设置SDA为输入  
	for(j=8;j>0;j--) 	//循环8次接收数据
	{		     	  
		STM32_Delay_us(50);
		SCCB_SCL=1;
		temp=temp<<1;
		if(SCCB_READ_SDA)temp++;   
		STM32_Delay_us(50);
		SCCB_SCL=0;
	}	
	SCCB_SDA_OUT();		//设置SDA为输出    
	return temp;
} 							    
//写寄存器
//返回值:0,成功;1,失败.
u8 STM32_SCCBWriteReg(u8 reg,u8 data)
{
	u8 res=0;
	STM32_SCCBStart(); 					//启动SCCB传输
	if(STM32_SCCBWriteByte(SCCB_ID))res=1;	//写器件ID	  
	STM32_Delay_us(100);
  	if(STM32_SCCBWriteByte(reg))res=1;		//写寄存器地址	  
	STM32_Delay_us(100);
  	if(STM32_SCCBWriteByte(data))res=1; 	//写数据	 
  	STM32_SCCBStop();	  
  	return	res;
}		  					    
//读寄存器
//返回值:读到的寄存器值
u8 STM32_SCCBReadReg(u8 reg)
{
	u8 val=0;
	STM32_SCCBStart(); 				//启动SCCB传输
	STM32_SCCBWriteByte(SCCB_ID);		//写器件ID	  
	STM32_Delay_us(100);	 
  STM32_SCCBWriteByte(reg);			//写寄存器地址	  
	STM32_Delay_us(100);	  
	STM32_SCCBStop();   
	STM32_Delay_us(100);	   
	//设置寄存器地址后，才是读
	STM32_SCCBStart();
	STM32_SCCBWriteByte(SCCB_ID|0X01);	//发送读命令	  
	STM32_Delay_us(100);
  val=STM32_SCCBReadByte();		 	//读取数据
  STM32_SCCBNoAck();
  STM32_SCCBStop();
  return val;
}















