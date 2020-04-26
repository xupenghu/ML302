/*******************************************************************************
  
 *******************************************************************************/
#include "cmmqtt_platform.h"
typedef void * TaskHandle_t;


void cmmqtt_deltask(CMMQTT_Task *task)
{
	if(task->pvCreatedTask != NULL)
	{
		osThreadTerminate(task->pvCreatedTask);
	}
}

void cmmqtt_suspendtask(CMMQTT_Task *task)
{
	if(task->pvCreatedTask != NULL)
		osThreadSuspend(task->pvCreatedTask);
}

void cmmqtt_resumetask(CMMQTT_Task *task)
{
	if(task->pvCreatedTask != NULL)
		osThreadResume(task->pvCreatedTask);
}


