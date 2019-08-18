#include "task.h"
#include "ARMCM3.h"

#define		NVIC_INT_CTRL		0xE000ED04
#define		NVIC_PENDSVSET		0x10000000
#define		NVIC_SYSPRI2		0xE000ED22
#define		NVIC_PENDSV_PRI		0x000000FF

#define		MEM32(addr)				*(volatile unsigned long*)(addr)
#define		MEM8(addr)				*(volatile unsigned char*)(addr)


bitmap_t taskPriMap;
tTask	*currentTask;
tTask	*nextTask;
tTask	*taskTable[32];

unsigned int unSchedLockCnt;

void Task_Sched(void)；

__asm void PendSV_Handler()
{
		IMPORT		currentTask
		IMPORT		nextTask

		MRS			r0,PSP 						;任务状态的保存
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

void SysTick_Handler(void)
{
	
}


void Task_Sched(void)
{
	tTask *temptask = NULL;
	unsigned int unStatus = 0;
	unStatus = TASK_EnterCritical();

	if(unSchedLockCnt > 0)
	{
		TASK_ExitCritical(unStatus);
		return ;
	}


	temptask = TASK_HighestReady();

	if(temptask != currentTask)
	{
		TASK_Switch();
	}
}

void TASK_SystemTickHandler(void)
{
	int i = 0;

	for(i = 0; i < 32;i++)
	{
		if(taskTable[i]->unDelayTicks > 0)
		{
			taskTable[i]->unDelayTicks--;	
		}

	}

	Task_Sched();
}
/**
 * @brief 系统时钟设置
 * @param[in] ms 定时器设置时钟时间 单位ms
 * @note None
 * @retval None
*/

void TASK_SetSysTickPeriod(unsigned int ms)
{
	SysTick->LOAD = ms * SystemCoreClock /1000 -1;  //初始化重载计数器寄存器值
	NVIC_SetPriority(SysTick_IRQn,(1 << __NVIC_PRIO_BITS) - 1);
	SysTick->VAL = 0;                               //递减寄存器值
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
}

/**
 * @brief PendSV触发
 * @param None
 * @note 触发PendSV中断，设置中断最低优先级，等其他中断完成后再处理任务切换的中断
 * retval None
 */
void TASK_TriggerPendSVC(void)
{
	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
} 


/**
 * @brief 任务初始化函数
 * @param task 任务独立栈，param 任务的参数，stack独立栈指针
 * @note 保存
 * @retval None
 */
void TASK_Init(tTask *task,void (*entry)(void*),void *param,unsigned int prio,tTaskStack *stack)
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
	task->unDelayTicks = 0;					//时间戳初始化为0
	task->unPri = prio;						//优先级赋值

	taskTable[prio] = task;
	COM_SetBitMap(&taskPriMap,prio);
}


tTask* TASK_HighestReady(void)
{
	unsigned int unHighPri = 0;
	unHighPri = COM_GetFirstBitMap(taskPriMap);
	return taskTable[unHighPri];
}


/**
 * @brief 关中断
 * @param None
 * @note 临界区保护进入
 * @return 返回中断状态
 */
unsigned int TASK_EnterCritical(void)
{
	unsigned int primask = __get_PRIMASK();
	__disable_irq();
	return primask;
}


/**
 * @brief 恢复中断状态
 * @param status 中断寄存器状态
 * @note 临界区保护离开
 * @return None
 */
void TASK_ExitCritical(unsigned int status)
{
	__set_PRIMASK(status);
}

void TASK_SchedLockInit(void)
{
	unSchedLockCnt = 0;
	COM_InitBitMap(&taskPriMap);
}


void TASK_SchedLockOff(void)
{
	unsigned int unStatus = 0;
	unStatus = TASK_EnterCritical();

	if(unSchedLockCnt > 0)
	{
		if(--unSchedLockCnt == 0)
		{
			Task_Sched();
		}

	}

	TASK_ExitCritical(unStatus);

}

void TASK_SchedLockOn(void)
{
	unsigned int unStatus = 0;
	unStatus = TASK_EnterCritical();

	if(unSchedLockCnt < 255)
    {
		unSchedLockCnt++;
    }


	TASK_ExitCritical(unStatus);
}



void TASK_RunFirst(void)
{
	__set_PSP(0);
	TASK_TriggerPendSVC();
}

void TASK_Switch(void)
{
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}

