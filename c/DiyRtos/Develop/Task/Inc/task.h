#ifndef		_TASK_H_
#define		_TASK_H_


#include<stdio.h>

typedef unsigned int tTaskStack;


typedef struct _tTask{
		tTaskStack *stack;
}tTask,*p_tTask;


extern tTask	*currentTask;
extern tTask	*nextTask;


void TASK_Init(tTask *task,void (*entry)(void*),void *param,tTaskStack *stack);
void TASK_RunFirst(void);
void TASK_SWitch(void);

void Task_Sched(void);


#endif

