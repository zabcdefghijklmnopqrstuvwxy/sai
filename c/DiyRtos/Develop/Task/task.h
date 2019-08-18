#ifndef		_TASK_H_
#define		_TASK_H_
#include<stdio.h>
#include "com.h"

typedef uint32_t		tTaskStack;


typedef struct _tTask{
		tTaskStack *stack;			//独立栈指针
		unsigned int unDelayTicks;  //时间戳
		unsigned int unPri;   		//优先级
}tTask;


extern tTask	*currentTask;
extern tTask	*nextTask;

void TASK_RunFirst(void);
void TASK_SWitch(void);

void Task_Sched(void);

tTask* TASK_HighestReady(void);


#endif

