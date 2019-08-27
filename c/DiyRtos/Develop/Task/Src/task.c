#include "task.h"
#include "stm32f4xx_hal.h"
//#include "core_cm3.h"

//#include "ARMCM3.h"
#include "CMSIS_ARMCC.H"

#define		NVIC_INT_CTRL		0xE000ED04
#define		NVIC_PENDSVSET		0x10000000
#define		NVIC_SYSPRI2		0xE000ED22
#define		NVIC_PENDSV_PRI		0x000000FF

#define		MEM32(addr)				*(volatile unsigned long*)(addr)
#define		MEM8(addr)				*(volatile unsigned char*)(addr)

tTask	*currentTask;
tTask	*nextTask;
extern p_tTask taskTable[2];

/**
* @brief PendSV中断处理
* @param[in] None
* @note 保存前一个任务的状态，恢复后一个任务的状态
* @retval None
*/
__asm void PendSV_Handler(void)
{    
	    IMPORT		currentTask
		IMPORT		nextTask

		MRS			r0,PSP 					
		CBZ			r0,PendSVHandler_nosave

		STMDB		r0!,{r4-r11}
		
		LDR			r1,=currentTask
		LDR			r1,[r1]
		STR			r0,[r1]

PendSVHandler_nosave
		
		LDR			r0,=currentTask
		LDR			r1,=nextTask
		LDR			r2,[r1]
		STR			r2,[r0]

		LDR			r0,[r2]
		LDMIA		r0!,{r4-r11}

		MSR			PSP,r0
		ORR			LR,LR,#0x04
		BX			LR
}

/**
* @brief 任务的切换
* @param[in] 
* @note None
* @retval None
*/
void TASK_Switch(void)
{
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}

/**
* @brief 任务调度
* @param[in] None
* @note 对任务进行切换处理
* @retval None
*/
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

/**
* @brief 触发PendSV中断
* @param[in] None
* @note None
* @retval None
*/
void TASK_TriggerPendSVC(void)
{
	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
} 

/**
* @brief 任务的初始化
* @param[in] 
* @note None
* @retval None
*/
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

/**
* @brief 运行第一个任务
* @param[in] 
* @note None
* @retval None
*/
void TASK_RunFirst(void)
{
    __set_PSP(0);
    TASK_TriggerPendSVC();
}



