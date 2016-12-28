

#include <time.h>
#include <stdlib.h>
#include "Osw.h"
#include "SmsPlatDefs.h"

typedef void *(*PthreadTaskFunc)(void*);

OSW_TaskId OSW_TaskCreate ( const char *TaskName,
					   UINT32 TaskPriority,
					   UINT32 TaskStackSize,
					   TaskFunc TaskFunction,
					   void *TaskFunctionParams )
{
	pthread_t *TaskHandle;
	pthread_attr_t attr;

	// allocate memory for task structure
	TaskHandle = (pthread_t*)malloc(sizeof(pthread_t));

	pthread_attr_init(&attr);
	pthread_attr_setstacksize (&attr, TaskStackSize);
	
	// create linux thread
	pthread_create ( TaskHandle, &attr,
					 (PthreadTaskFunc)TaskFunction,
					 TaskFunctionParams );
	if ( *TaskHandle == 0 )
	{
		// if failed - free task structure
		free(TaskHandle);
		return NULL;
	}

	return (void*)TaskHandle;
}

void OSW_TaskCleanup ( OSW_TaskId pTask )
{
	// android don't support pthread_cancel
	//// cancel thread
	//pthread_cancel ( *(pthread_t*)pTask );

	// free task structure
	free((pthread_t*)pTask);
}

void OSW_TaskSleep ( UINT32 TaskSleepPeriod )
{
	struct timespec ts, tr;
	
	// calculate wakeup time
	ts.tv_sec = TaskSleepPeriod / 1000;
	ts.tv_nsec = ( TaskSleepPeriod % 1000 ) * 1000000;
	// sleep
	nanosleep ( &ts, &tr );
}

/*!  unimplemented */
OSW_TaskId OSW_TaskGetCurrent ( void )
{
	return NULL;
}

