

#ifndef __OSW_TASK_H
#define __OSW_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Osw.h"

typedef UINT32 (*TaskFunc)(UINT32);

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

OSW_TaskId OSW_TaskCreate (const char*	TaskName,
					  UINT32		TaskPriority,
					  UINT32		TaskStackSize,
					  TaskFunc		TaskFunction,
					  void*			TaskFunctionParams);


void  OSW_TaskCleanup   (OSW_TaskId TaskId);

void  OSW_TaskSleep  (UINT32 TaskSleepPeriod);

OSW_TaskId OSW_TaskGetCurrent(void);


#ifdef __cplusplus
}
#endif

#endif
