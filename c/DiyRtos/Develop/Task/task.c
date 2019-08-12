#include "task.h"
#include "ARMCM3.h"

#define		NVIC_INT_CTRL		0xE000ED04
#define		NVIC_PENDSVSET		0x10000000
#define		NVIC_SYSPRI2		0xE000ED22
#define		NVIC_PENDSV_PRI		0x000000FF

#define		MEM32(addr)				*(volatile unsigned long*)(addr)
#define		MEM8(addr)				*(volatile unsigned char*)(addr)

tTask	*currentTask;
tTask	*nextTask;
tTask	*taskTable[2];


__asm void PendSV_Handler()
{
		IMPORT		currentTask
		IMPORT		nextTask

		MRS			R0,PSP 						;任务状态的保存
		CBZ			R0,PendSVHandler_nosave

		STMDB		R0!,{R4-R11}
		
		LDR			R1,=currentTask
		LDR			R1,{R1}
		STR			R0,{R1}

PendSVHandler_nosave
		
		LDR			R0,=currentTask
		LDR			R1,=nextTask
		LDR			R2,{R1}
		STR			R2,{R0}

		LDR			R0,{R2}
		LDMIA		R0!,{R4-R11}

		MSR			PSP,R0
		ORR			LR,LR,#0x04
		BX			LR
}



void Task_Sched(void)
{
		if(currentTask == taskTable[0])
		{
				nextTask = taskTable[1];
		}
		else
		{
				nextTask = taskTable[0];
		}

		TASK_Switch();
}


void TASK_TriggerPendSVC(void)
{
	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
} 

void TASK_Init(tTask *task,void (*entry)(void*),void *param,tTaskStack *stack)
{
		*(--stack) = (unsigned long)(1 << 24); //cpsr标志设置
		*(--stack) = (unsigned long)entry;		//保存任务入口地址
		*(--stack) = (unsigned long)0x14;		//保存lr寄存器值
		*(--stack) = (unsigned long)0x12;		//保存R12寄存器值
		*(--stack) = (unsigned long)0x3;		//保存R3寄存器值
		*(--stack) = (unsigned long)0x2;		//保存R2寄存器值
		*(--stack) = (unsigned long)0x1;		//保存R1寄存器值
		*(--stack) = (unsigned long)param;		//保存任务参数指针

		*(--stack) = (unsigned long)0x11;		//保存R11寄存器值
		*(--stack) = (unsigned long)0x10;		//保存R10寄存器值
		*(--stack) = (unsigned long)0x9;		//保存R9寄存器值
		*(--stack) = (unsigned long)0x8;		//保存R8寄存器值
		*(--stack) = (unsigned long)0x7;		//保存R7寄存器值
		*(--stack) = (unsigned long)0x6;		//保存R6寄存器值
		*(--stack) = (unsigned long)0x5;		//保存R5寄存器值
		*(--stack) = (unsigned long)0x4;		//保存R4寄存器值

		task->stack = stack;					//保存任务的独立栈
}

void TASK_RunFirst(void)
{
	_set_PSP(0);
	TASK_TriggerPendSVC();
}

void TASK_Switch(void)
{
		MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}

