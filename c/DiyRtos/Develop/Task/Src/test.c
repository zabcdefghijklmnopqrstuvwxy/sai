#include "task.h"

unsigned int taskFlag;
unsigned int task2Flag;


tTask tTask1;
tTask tTask2;
tTask tTaskIdle;

tTaskStack		task1Env[1024];
tTaskStack		task2Env[1024];
tTaskStack		idleTaskEnv[1024];

unsigned int unShareCount;


void Task_Delay(unsigned int count)
{
	while(--count > 0);
}

void task1Entry(void *param)
{
	bitmap_t bitmap;

	unsigned long value = *(unsigned long*)param;
	TaskSysTickPeriod(10);

	for(;;)
	{
		value++;
		unShareCount = value;

		task1Flag = 0;
		Task_Delay(100);
		task1Flag = 1;
		Task_Delay(100);
	}
}

void task2Entry(void *param)
{
	unsigned long value = *(unsigned long*)param;
	value++;
	for(;;)
	{
		unShareCount++;

		task2Flag = 0;
		Task_Delay(100);
		task2Flag = 1;
		Task_Delay(100);
	}
}

void idleTaskEntry(void *param)
{
	for(;;)
	{


	}

}


int main()
{
	TASK_Init(&tTask1,task1Entry,(void*)0x11111111,	0, &task1Env[1024]);
	TASK_Init(&tTask2,task2Entry,(void*)0x22222222,	1, &task2Env[1024]);
	TASK_Init(&tTaskIdle,idleTaskEntry,(void*)0x0,	31, &idleTaskEnv[1024]);

	TASK_SchedLockInit();

	taskTable[0] = &tTask1;
	taskTable[1] = &tTask2;

	nextTask = TASK_HighestReady();

	TASK_RunFirst();

	return 0;
}