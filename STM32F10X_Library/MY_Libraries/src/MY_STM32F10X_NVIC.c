#include "MY_STM32F10X_Conf.h"
#include "math.h"

//函数作用：设置向量表偏移地址
//参数说明：NVIC_VectTab:基址，NVIC_Offset:偏移量
//测试结果：NOT CHECK
//完成日期：14.02.27
void STM32_NVICSetVectorTable(u32 NVIC_VectTab, u32 NVIC_Offset)
{
	//检查参数合法性
	assert_param(IS_NVIC_VECTTAB(NVIC_VectTab));
	assert_param(IS_NVIC_OFFSET(NVIC_Offset));
	SCB->VTOR = NVIC_VectTab|(NVIC_Offset & (u32)0x1fffff80);	//设置NVIC的向量表偏移寄存器
}

//函数作用：设置中断分组
//参数说明：NVIC_Group:中断分组
//测试结果：CHECK OK
//完成日期：14.02.22
//更新日期：15.10.19
void STM32_NVICPriorityGroupConfig(u8 NVIC_Group)													
{
	if(NVIC_Group<=4)		//参数检查
		SCB->AIRCR=((SCB->AIRCR)&0x0000f8ff)+0x05fa0000+((7-NVIC_Group)<<8);				
}

//函数作用：设置中断分组、优先级设置及中断使能
//参数说明：NVIC_Channel为中断通道（中断编号），NVIC_Group为中断分组，NVIC_PreemptionPriority为抢占优先级，\
			NVIC_SubPriority为响应优先级；
//测试结果：CHECK OK
//完成日期：14.02.22
//更新日期：15.10.19
void STM32_NVICInit(u8 NVIC_Channel,u8 NVIC_Group,u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority)	
{
	u8 Priority=0;
	STM32_NVICPriorityGroupConfig(NVIC_Group);																//中断分组	
	if((NVIC_PreemptionPriority<ldexp(1,NVIC_GROUP))&&(NVIC_SubPriority<ldexp(1,4-NVIC_GROUP)))		//参数检查
	{
		Priority=(NVIC_PreemptionPriority<<(4-NVIC_Group))+NVIC_SubPriority;		//中断优先级设置
		NVIC->IP[NVIC_Channel/4]=(NVIC->IP[NVIC_Channel/4]&(~(0xf<<((NVIC_Channel%4)*8+4))))+(Priority<<((NVIC_Channel%4)*8+4));
	}
	NVIC->ISER[NVIC_Channel/32]|=1<<(NVIC_Channel%32);												//中断使能
}
