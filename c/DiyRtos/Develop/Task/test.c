#include "task.h"

unsigned int taskFlag;
unsigned int task2Flag;


tTask tTask1;
tTask tTask2;


tTaskStack		task1Env[1024];
tTaskStack		task2Env[1024];


void Task_Delay(unsigned int count)
{
	while(--count > 0);
}

void task1Entry(void *param)
{
	unsigned long value = *(unsigned long*)param;
	value++;
	for(;;)
	{
		task1Flag = 0;
		Task_Delay(100);
		task1Flag = 1;
		Task_Delay(100);

		Task_Sched();
	}
}

void task2Entry(void *param)
{
		unsigned long value = *(unsigned long*)param;
		value++;
		for(;;)
		{
			task2Flag = 0;
			Task_Delay(100);
			task2Flag = 1;
			Task_Delay(100);
		}


}

int main()
{
		TASK_Init(&tTask1,task1Entry,(void*)0x11111111,&task1Env[1024]);
		TASK_Init(&tTask2,task2Entry,(void*)0x22222222,&task2Env[1024]);

		taskTable[0] = &tTask1;
		taskTable[1] = &tTask2;

		nextTask = taskTable[0];

		TASK_RunFirst();

		return 0;
}