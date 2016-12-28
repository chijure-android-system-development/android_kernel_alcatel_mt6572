

#ifndef __OSW_MAIN_INCLUDE_H
#define __OSW_MAIN_INCLUDE_H

#ifdef __cplusplus
extern "C" {
#endif

#define		OSW_OK						0
#define		OSW_TIMEOUT					1
#define		OSW_ERROR					2

#define		OSW_WAIT_FOREVER			0xFFFFFFFF

#define null 0

#include "SmsPlatDefs.h"
#include "Osw_Task.h"
#include "Osw_Mutex.h"
#include "Osw_Event.h"
#include "Osw_Memory.h"
#include "Osw_Time.h"
#include "Osw_FileSystem.h"

void OSW_OsInit(void);
void OSW_OsDeinit(void);

#ifdef __cplusplus
}
#endif

#endif // __OSW_MAIN_INCLUDE_H

