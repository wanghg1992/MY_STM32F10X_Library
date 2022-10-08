#include "MY_STM32F10X_Conf.h"
#include "usart.h"

//�������ã�CAN1�ĳ�ʼ��
//����˵����CANBaudRate:�����ʣ�
//���Խ��������ģʽ�£���ģʽѡ���Ϊ���أ�CHECK OK������ģʽ�£���ģʽѡ���Ϊ������NOT CHECK
//������ڣ�16.01.23
//ע�⣺Ĭ��״̬������ӳ�䣬����������ģʽ��ʹ��CAN1�Ľ����жϣ������б�ʶ������
void STM32_CAN1Init(CANBaudRate_TypeDef CANBaudRate)
{
	GPIO_InitTypeDef GPIO_InitStructrue;
	CAN_InitTypeDef CAN_InitStructure;
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//ʱ��ʹ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);
	
	//��������
	GPIO_InitStructrue.GPIO_Pin=GPIO_Pin_11;					//PA11��������
	GPIO_InitStructrue.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_Init(GPIOA,&GPIO_InitStructrue);
	
	GPIO_InitStructrue.GPIO_Speed=GPIO_Speed_10MHz;
	GPIO_InitStructrue.GPIO_Pin=GPIO_Pin_12;					//PA12�����������
	GPIO_InitStructrue.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructrue);
	
	//�ж�����
	NVIC_InitStructure.NVIC_IRQChannel=USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//CAN����
	CAN_DeInit(CAN1);
	CAN_StructInit(&CAN_InitStructure);
	
	CAN_InitStructure.CAN_TTCM=DISABLE;			//�ر�ʱ�䴥��ģʽ
	CAN_InitStructure.CAN_ABOM=DISABLE;			//�ر��Զ����߹���
	CAN_InitStructure.CAN_AWUM=DISABLE;			//�ر��Զ�����ģʽ
	CAN_InitStructure.CAN_NART=DISABLE;			//ʹ�ܱ����Զ��ش�
	CAN_InitStructure.CAN_RFLM=DISABLE;			//FIFO���ʱ���ĸ���Դ�ļ�
	CAN_InitStructure.CAN_TXFP=DISABLE;			//ʧ���ڲ����ķ������ȼ������ķ������ȼ�ֻȡ����ID��
	CAN_InitStructure.CAN_Mode=CAN_Mode_Normal;			//CAN����������ģʽ���Ҳ���ʱ����Ҫ��
	//CAN_InitStructure.CAN_Mode=CAN_Mode_LoopBack;			//CAN�����ڻػ�ģʽ
	
	//���ò�����
//	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;	//��������Ϊ250KBPS
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
	
	CAN_Init(CAN1,&CAN_InitStructure);			//��ʼ��
	
	//�����˲�����
	CAN_FilterInitStructure.CAN_FilterNumber=0;													//ʹ��0�Ź�����
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;				//��ʶ������λģʽ
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;			//һ��32λ���������Һ������Ҫ��Ϊ16λ��
	CAN_FilterInitStructure.CAN_FilterIdHigh=0;													//��������ʶ����16λ������Ҫ��
	CAN_FilterInitStructure.CAN_FilterIdLow=0;													//��������ʶ����16λ������Ҫ��
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0;											//���������α�ʶ����16λ
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0;											//���������α�ʶ����16λ
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_FilterFIFO0;		//ָ���������Ϊ0��FIFO
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;								//ʹ�ܹ�����
	CAN_FilterInit(&CAN_FilterInitStructure);														//��ʼ��������
	
	//ʹ��CAN�����ж�
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);
}

//�������ã�CAN��������
//����˵����CANx��CANģ�飻StdID����׼��ʶ����pData��Ҫ�������ݵ��׵�ַ��Count��Ҫ�������ݵĳ���
//���Խ����CHECK OK
//������ڣ�16.01.24
//ע�⣺Ĭ��״̬�²�ʹ����չ��ʶ�������͵�Ϊ����֡������Զ��֡��
void STM32_CANSendData(CAN_TypeDef* CANx,u16 StdID,u8 *pData,u8 Count)		//��������֡��ʹ�ñ�׼��ʶ��
{
	u8 i=0,TransmitMailbox=0;
	CanTxMsg CanTxMessage;
	
	CanTxMessage.StdId=StdID&0x7ff;				//��׼��ʶ��
	CanTxMessage.ExtId=0;									//��չ��ʶ��,����
	CanTxMessage.IDE=CAN_ID_STD;					//��׼����չ��ʶ��ѡ��λ��ʹ�ñ�׼��ʶ��
	CanTxMessage.RTR=CAN_RTR_DATA;				//����֡��Զ��֡ѡ��λ��ʹ������֡
	CanTxMessage.DLC=Count;								//���ݳ�������
	for(i=0;i<Count;i++)
		CanTxMessage.Data[i]=pData[i];
	TransmitMailbox=CAN_Transmit(CANx,&CanTxMessage);			//���ͽ��յ�������
	i=0;
	while((CAN_TransmitStatus(CANx,TransmitMailbox)!=CANTXOK)&&(i!=0xff))		//�ȴ����ݿ�ʼ���䣬����������
	{
		i++;
	}
}

/**********************�жϴ�����***********************************/
void USB_LP_CAN1_RX0_IRQHandler(void)						//�����жϴ���CAN1��USB���ã�
{
	CanRxMsg CanRxMessage;
//	CanTxMsg CanTxMessage;
	
	CAN_Receive(CAN1,CAN_FIFO0,&CanRxMessage);		//������Ϣ

//	//�ӵ�ʲô��ʲô
//	CanTxMessage.StdId=CanRxMessage.StdId;				//��׼��ʶ��
//	CanTxMessage.ExtId=CanRxMessage.ExtId;				//��չ��ʶ��
//	CanTxMessage.IDE=CanRxMessage.IDE;						//��׼����չ��ʶ��ѡ��λ
//	CanTxMessage.RTR=CanRxMessage.RTR;						//����֡��Զ��֡ѡ��λ
//	CanTxMessage.DLC=CanRxMessage.DLC;						//���ݳ�������
//	CanTxMessage.Data[0]=CanRxMessage.Data[0];
//	CanTxMessage.Data[1]=CanRxMessage.Data[1];
//	CanTxMessage.Data[2]=CanRxMessage.Data[2];
//	CanTxMessage.Data[3]=CanRxMessage.Data[3];
//	CanTxMessage.Data[4]=CanRxMessage.Data[4];
//	CanTxMessage.Data[5]=CanRxMessage.Data[5];
//	CanTxMessage.Data[6]=CanRxMessage.Data[6];
//	CanTxMessage.Data[7]=CanRxMessage.Data[7];
//	CAN_Transmit(CAN1,&CanTxMessage);							//���ͽ��յ�������
	
	printf("ID:%x;\r\n",CanRxMessage.StdId);
	printf("Data:%x,%x,%x,%x,%x,%x,%x,%x;\r\n",CanRxMessage.Data[0],CanRxMessage.Data[1],CanRxMessage.Data[2],
		CanRxMessage.Data[3],CanRxMessage.Data[4],CanRxMessage.Data[5],CanRxMessage.Data[6],CanRxMessage.Data[7]);
}





