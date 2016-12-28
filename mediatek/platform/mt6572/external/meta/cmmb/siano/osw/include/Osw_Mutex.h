

#ifndef __OS_MUTEX_H
#define __OS_MUTEX_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Osw.h"

///////////////////////////////////////////////////////////////////////////////
UINT32 OSW_MutexCreate
	(
	MUTEX*					pMutex
	);


UINT32 OSW_MutexDelete
	(
	MUTEX*					pMutex
	);


UINT32 OSW_MutexGet
	(
	MUTEX*					pMutex
	);


UINT32 OSW_MutexPut
	(
	MUTEX*					pMutex
	);


#ifdef __cplusplus
}
#endif

#endif
