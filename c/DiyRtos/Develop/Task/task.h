#ifndef		_TASK_H_
#define		_TASK_H_
#include<stdio.h>

typedef uint32_t		tTaskStack;


typedef struct _tTask{
		tTaskStack *stack;
}tTask;


extern tTask	*currentTask;
extern tTask	*nextTask;

void TASK_RunFirst(void);
void TASK_SWitch(void);

void Task_Sched(void);


#endif

