

#ifndef __OSW_MEMORY_H
#define __OSW_MEMORY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Osw.h"

void*  OSW_MemAlloc( UINT32 size );

void   OSW_MemFree( void* pMemToFree );


#ifdef __cplusplus
}
#endif

#endif
