#include "MY_STM32F10X_Conf.h"
#include "usart.h"

//函数作用：CAN1的初始化
//参数说明：CANBaudRate:波特率；
//测试结果：环回模式下（把模式选择改为环回）CHECK OK，正常模式下（把模式选择改为正常）NOT CHECK
//完成日期：16.01.23
//注意：默认状态下无重映射，工作在正常模式，使能CAN1的接收中断，不进行标识符过滤
void STM32_CAN1Init(CANBaudRate_TypeDef CANBaudRate)
{
	GPIO_InitTypeDef GPIO_InitStructrue;
	CAN_InitTypeDef CAN_InitStructure;
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//时钟使能
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);
	
	//引脚配置
	GPIO_InitStructrue.GPIO_Pin=GPIO_Pin_11;					//PA11上拉输入
	GPIO_InitStructrue.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_Init(GPIOA,&GPIO_InitStructrue);
	
	GPIO_InitStructrue.GPIO_Speed=GPIO_Speed_10MHz;
	GPIO_InitStructrue.GPIO_Pin=GPIO_Pin_12;					//PA12复用推挽输出
	GPIO_InitStructrue.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructrue);
	
	//中断配置
	NVIC_InitStructure.NVIC_IRQChannel=USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//CAN配置
	CAN_DeInit(CAN1);
	CAN_StructInit(&CAN_InitStructure);
	
	CAN_InitStructure.CAN_TTCM=DISABLE;			//关闭时间触发模式
	CAN_InitStructure.CAN_ABOM=DISABLE;			//关闭自动离线管理
	CAN_InitStructure.CAN_AWUM=DISABLE;			//关闭自动唤醒模式
	CAN_InitStructure.CAN_NART=DISABLE;			//使能报文自动重传
	CAN_InitStructure.CAN_RFLM=DISABLE;			//FIFO溢出时报文覆盖源文件
	CAN_InitStructure.CAN_TXFP=DISABLE;			//失能内部报文发送优先级，报文发送优先级只取决于ID号
	CAN_InitStructure.CAN_Mode=CAN_Mode_Normal;			//CAN工作在正常模式，我测试时可能要改
	//CAN_InitStructure.CAN_Mode=CAN_Mode_LoopBack;			//CAN工作在回环模式
	
	//设置波特率
//	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;	//波特率设为250KBPS
//	CAN_InitStructure.CAN_BS1=CAN_BS1_3tq;
//	CAN_InitStructure.CAN_BS2=CAN_BS2_2tq;
//	CAN_InitStructure.CAN_Prescaler=24;
	switch(CANBaudRate)
	{
		case CANBaudRate_5K:CAN_InitStructure.CAN_SJW=CAN_SJW_2tq;CAN_InitStructure.CAN_BS1=CAN_BS1_6tq;
									CAN_InitStructure.CAN_BS2=CAN_BS2_4tq;CAN_InitStructure.CAN_Prescaler=600;break;
		case CANBaudRate_10K:CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;CAN_InitStructure.CAN_BS1=CAN_BS1_3tq;
									CAN_InitStructure.CAN_BS2=CAN_BS2_2tq;CAN_InitStructure.CAN_Prescaler=600;break;
		case CANBaudRate_20K:CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;CAN_InitStructure.CAN_BS1=CAN_BS1_3tq;
									CAN_InitStructure.CAN_BS2=CAN_BS2_2tq;CAN_InitStructure.CAN_Prescaler=300;break;
		case CANBaudRate_25K:CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;CAN_InitStructure.CAN_BS1=CAN_BS1_3tq;
									CAN_InitStructure.CAN_BS2=CAN_BS2_2tq;CAN_InitStructure.CAN_Prescaler=240;break;
		case CANBaudRate_40K:CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;CAN_InitStructure.CAN_BS1=CAN_BS1_3tq;
									CAN_InitStructure.CAN_BS2=CAN_BS2_2tq;CAN_InitStructure.CAN_Prescaler=150;break;
		case CANBaudRate_50K:CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;CAN_InitStructure.CAN_BS1=CAN_BS1_3tq;
									CAN_InitStructure.CAN_BS2=CAN_BS2_2tq;CAN_InitStructure.CAN_Prescaler=120;break;
		case CANBaudRate_62_5K:CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;CAN_InitStructure.CAN_BS1=CAN_BS1_3tq;
									CAN_InitStructure.CAN_BS2=CAN_BS2_2tq;CAN_InitStructure.CAN_Prescaler=96;break;
		case CANBaudRate_80K:CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;CAN_InitStructure.CAN_BS1=CAN_BS1_3tq;
									CAN_InitStructure.CAN_BS2=CAN_BS2_2tq;CAN_InitStructure.CAN_Prescaler=75;break;
		case CANBaudRate_100K:CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;CAN_InitStructure.CAN_BS1=CAN_BS1_3tq;
									CAN_InitStructure.CAN_BS2=CAN_BS2_2tq;CAN_InitStructure.CAN_Prescaler=60;break;
		case CANBaudRate_125K:CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;CAN_InitStructure.CAN_BS1=CAN_BS1_3tq;
									CAN_InitStructure.CAN_BS2=CAN_BS2_2tq;CAN_InitStructure.CAN_Prescaler=48;break;
		case CANBaudRate_200K:CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;CAN_InitStructure.CAN_BS1=CAN_BS1_3tq;
									CAN_InitStructure.CAN_BS2=CAN_BS2_2tq;CAN_InitStructure.CAN_Prescaler=30;break;
		case CANBaudRate_250K:CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;CAN_InitStructure.CAN_BS1=CAN_BS1_3tq;
									CAN_InitStructure.CAN_BS2=CAN_BS2_2tq;CAN_InitStructure.CAN_Prescaler=24;break;
		case CANBaudRate_400K:CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;CAN_InitStructure.CAN_BS1=CAN_BS1_5tq;
									CAN_InitStructure.CAN_BS2=CAN_BS2_3tq;CAN_InitStructure.CAN_Prescaler=10;break;
		case CANBaudRate_500K:CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;CAN_InitStructure.CAN_BS1=CAN_BS1_3tq;
									CAN_InitStructure.CAN_BS2=CAN_BS2_2tq;CAN_InitStructure.CAN_Prescaler=12;break;
		case CANBaudRate_800K:CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;CAN_InitStructure.CAN_BS1=CAN_BS1_5tq;
									CAN_InitStructure.CAN_BS2=CAN_BS2_3tq;CAN_InitStructure.CAN_Prescaler=5;break;
		case CANBaudRate_1M:CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;CAN_InitStructure.CAN_BS1=CAN_BS1_3tq;
									CAN_InitStructure.CAN_BS2=CAN_BS2_2tq;CAN_InitStructure.CAN_Prescaler=6;break;
		default:break;
	}
	
	CAN_Init(CAN1,&CAN_InitStructure);			//初始化
	
	//屏蔽滤波设置
	CAN_FilterInitStructure.CAN_FilterNumber=0;													//使用0号过滤器
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;				//标识符屏蔽位模式
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;			//一个32位过滤器，我后面可能要改为16位的
	CAN_FilterInitStructure.CAN_FilterIdHigh=0;													//过滤器标识符高16位，可能要改
	CAN_FilterInitStructure.CAN_FilterIdLow=0;													//过滤器标识符低16位，可能要改
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0;											//过滤器屏蔽标识符高16位
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0;											//过滤器屏蔽标识符低16位
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_FilterFIFO0;		//指向过滤器的为0号FIFO
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;								//使能过滤器
	CAN_FilterInit(&CAN_FilterInitStructure);														//初始化过滤器
	
	//使能CAN接收中断
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);
}

//函数作用：CAN发送数据
//参数说明：CANx：CAN模块；StdID：标准标识符；pData：要发送数据的首地址；Count：要发送数据的长度
//测试结果：CHECK OK
//完成日期：16.01.24
//注意：默认状态下不使用扩展标识符，发送的为数据帧（不用远程帧）
void STM32_CANSendData(CAN_TypeDef* CANx,u16 StdID,u8 *pData,u8 Count)		//发送数据帧，使用标准标识符
{
	u8 i=0,TransmitMailbox=0;
	CanTxMsg CanTxMessage;
	
	CanTxMessage.StdId=StdID&0x7ff;				//标准标识符
	CanTxMessage.ExtId=0;									//扩展标识符,不用
	CanTxMessage.IDE=CAN_ID_STD;					//标准或扩展标识符选择位：使用标准标识符
	CanTxMessage.RTR=CAN_RTR_DATA;				//数据帧或远程帧选择位：使用数据帧
	CanTxMessage.DLC=Count;								//数据长度设置
	for(i=0;i<Count;i++)
		CanTxMessage.Data[i]=pData[i];
	TransmitMailbox=CAN_Transmit(CANx,&CanTxMessage);			//发送接收到的数据
	i=0;
	while((CAN_TransmitStatus(CANx,TransmitMailbox)!=CANTXOK)&&(i!=0xff))		//等待数据开始传输，我这样理解的
	{
		i++;
	}
}

/**********************中断处理函数***********************************/
void USB_LP_CAN1_RX0_IRQHandler(void)						//接收中断处理（CAN1与USB共用）
{
	CanRxMsg CanRxMessage;
//	CanTxMsg CanTxMessage;
	
	CAN_Receive(CAN1,CAN_FIFO0,&CanRxMessage);		//接收消息

//	//接到什么发什么
//	CanTxMessage.StdId=CanRxMessage.StdId;				//标准标识符
//	CanTxMessage.ExtId=CanRxMessage.ExtId;				//扩展标识符
//	CanTxMessage.IDE=CanRxMessage.IDE;						//标准或扩展标识符选择位
//	CanTxMessage.RTR=CanRxMessage.RTR;						//数据帧或远程帧选择位
//	CanTxMessage.DLC=CanRxMessage.DLC;						//数据长度设置
//	CanTxMessage.Data[0]=CanRxMessage.Data[0];
//	CanTxMessage.Data[1]=CanRxMessage.Data[1];
//	CanTxMessage.Data[2]=CanRxMessage.Data[2];
//	CanTxMessage.Data[3]=CanRxMessage.Data[3];
//	CanTxMessage.Data[4]=CanRxMessage.Data[4];
//	CanTxMessage.Data[5]=CanRxMessage.Data[5];
//	CanTxMessage.Data[6]=CanRxMessage.Data[6];
//	CanTxMessage.Data[7]=CanRxMessage.Data[7];
//	CAN_Transmit(CAN1,&CanTxMessage);							//发送接收到的数据
	
	printf("ID:%x;\r\n",CanRxMessage.StdId);
	printf("Data:%x,%x,%x,%x,%x,%x,%x,%x;\r\n",CanRxMessage.Data[0],CanRxMessage.Data[1],CanRxMessage.Data[2],
		CanRxMessage.Data[3],CanRxMessage.Data[4],CanRxMessage.Data[5],CanRxMessage.Data[6],CanRxMessage.Data[7]);
}





