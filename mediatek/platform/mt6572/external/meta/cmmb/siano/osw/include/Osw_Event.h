

#ifndef __OSW_EVENT_H
#define __OSW_EVENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Osw.h"

UINT32 OSW_EventCreate
	(
	Event*		pEvent			// where to store the event 
    );


UINT32 OSW_EventDelete
	(
	Event*		pEvent
	);



UINT32 OSW_EventSet
	(
	Event*		pEvent
	);


UINT32 OSW_EventClear
	(
	Event*		pEvent
	);


UINT32 OSW_EventWait
	(
	Event*		pEvent,
	UINT32		timeout
	);

#ifdef __cplusplus
}
#endif

#endif


