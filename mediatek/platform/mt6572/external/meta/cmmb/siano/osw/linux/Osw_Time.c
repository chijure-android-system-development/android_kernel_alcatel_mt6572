

#include "Osw.h"
#include <sys/time.h>

UINT32 OSW_GetCurTimeStampInMSec ( void )
{
	struct timeval tv;
	gettimeofday ( &tv, NULL );
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

UINT32 OSW_GetMiliSecondsPassedSince ( UINT32 timeStamp )
{
	struct timeval tv;
	gettimeofday ( &tv, NULL );
	return tv.tv_sec * 1000 + tv.tv_usec / 1000 - timeStamp;
}
