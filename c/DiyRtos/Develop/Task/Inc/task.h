#ifndef		_TASK_H_
#define		_TASK_H_


#include<stdio.h>
#include "com.h"

typedef unsigned int tTaskStack;


typedef struct _tTask{
<<<<<<< HEAD:c/DiyRtos/Develop/Task/task.h
		tTaskStack *stack;			//独立栈指针
		unsigned int unDelayTicks;  //时间戳
		unsigned int unPri;   		//优先级
}tTask;
=======
		tTaskStack *stack;
}tTask,*p_tTask;
>>>>>>> 683dea7234d7244e39fe6a68ef4bdbecdd29db9e:c/DiyRtos/Develop/Task/Inc/task.h


extern tTask	*currentTask;
extern tTask	*nextTask;


void TASK_Init(tTask *task,void (*entry)(void*),void *param,tTaskStack *stack);
void TASK_RunFirst(void);
void TASK_SWitch(void);

void Task_Sched(void);

tTask* TASK_HighestReady(void);


#endif

