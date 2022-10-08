#include "MY_STM32F10X_Conf.h"
#include "math.h"

//�������ã�����������ƫ�Ƶ�ַ
//����˵����NVIC_VectTab:��ַ��NVIC_Offset:ƫ����
//���Խ����NOT CHECK
//������ڣ�14.02.27
void STM32_NVICSetVectorTable(u32 NVIC_VectTab, u32 NVIC_Offset)
{
	//�������Ϸ���
	assert_param(IS_NVIC_VECTTAB(NVIC_VectTab));
	assert_param(IS_NVIC_OFFSET(NVIC_Offset));
	SCB->VTOR = NVIC_VectTab|(NVIC_Offset & (u32)0x1fffff80);	//����NVIC��������ƫ�ƼĴ���
}

//�������ã������жϷ���
//����˵����NVIC_Group:�жϷ���
//���Խ����CHECK OK
//������ڣ�14.02.22
//�������ڣ�15.10.19
void STM32_NVICPriorityGroupConfig(u8 NVIC_Group)													
{
	if(NVIC_Group<=4)		//�������
		SCB->AIRCR=((SCB->AIRCR)&0x0000f8ff)+0x05fa0000+((7-NVIC_Group)<<8);				
}

//�������ã������жϷ��顢���ȼ����ü��ж�ʹ��
//����˵����NVIC_ChannelΪ�ж�ͨ�����жϱ�ţ���NVIC_GroupΪ�жϷ��飬NVIC_PreemptionPriorityΪ��ռ���ȼ���\
			NVIC_SubPriorityΪ��Ӧ���ȼ���
//���Խ����CHECK OK
//������ڣ�14.02.22
//�������ڣ�15.10.19
void STM32_NVICInit(u8 NVIC_Channel,u8 NVIC_Group,u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority)	
{
	u8 Priority=0;
	STM32_NVICPriorityGroupConfig(NVIC_Group);																//�жϷ���	
	if((NVIC_PreemptionPriority<ldexp(1,NVIC_GROUP))&&(NVIC_SubPriority<ldexp(1,4-NVIC_GROUP)))		//�������
	{
		Priority=(NVIC_PreemptionPriority<<(4-NVIC_Group))+NVIC_SubPriority;		//�ж����ȼ�����
		NVIC->IP[NVIC_Channel/4]=(NVIC->IP[NVIC_Channel/4]&(~(0xf<<((NVIC_Channel%4)*8+4))))+(Priority<<((NVIC_Channel%4)*8+4));
	}
	NVIC->ISER[NVIC_Channel/32]|=1<<(NVIC_Channel%32);												//�ж�ʹ��
}
