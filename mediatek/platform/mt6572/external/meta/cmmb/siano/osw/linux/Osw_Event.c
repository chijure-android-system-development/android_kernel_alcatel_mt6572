

#include "Osw.h"
#include <pthread.h>
#include <string.h>
#include <sys/time.h>

UINT32 OSW_EventCreate ( Event * pEvent)
{
	if (pEvent)
	{
		// init mutex for event
		pthread_mutex_init(&pEvent->mutex, NULL);
		// init condition variable
		pthread_cond_init(&pEvent->cond, NULL);
		pEvent->set = 0;
		return 0;
	}
	
	return 1;
}

UINT32 OSW_EventDelete ( Event * pEvent)
{
	if (pEvent)
	{
		// destroy mutex
		pthread_mutex_destroy(&pEvent->mutex);
		// destroy condition variable
		pthread_cond_destroy(&pEvent->cond);
		return 0;
	}

	return 1;
}

UINT32 OSW_EventSet ( Event * pEvent)
{
	int rc = 0;

	// critical section
	rc = pthread_mutex_lock ( &pEvent->mutex );
	if ( rc != 0 )
	{
		// exit critical section
		rc = pthread_mutex_unlock (  &pEvent->mutex );
		return rc | PTHREAD_MUTEX_ERROR;
	}
	// set "event set" flag
	pEvent->set = 1;
	// signal waiting threads
	rc = pthread_cond_signal ( &pEvent->cond );
	if ( rc != 0 )
	{
		pthread_mutex_unlock(&pEvent->mutex);
		return rc | PTHREAD_COND_VAR_ERROR;
	}
	// exit critical section
	rc = pthread_mutex_unlock (  &pEvent->mutex );
	if ( rc != 0 )
	{
		return rc | PTHREAD_MUTEX_ERROR;
	}

	return rc;
}

UINT32 OSW_EventClear ( Event * pEvent)
{
	int rc = 0;

	// critical section
	rc = pthread_mutex_lock ( &pEvent->mutex );
	if ( rc != 0 )
	{
		// exit critical section
		rc = pthread_mutex_unlock (  &pEvent->mutex );
		return rc | PTHREAD_MUTEX_ERROR;
	}

	// clear "even set" flag
	pEvent->set = 0;

	// exit critical section
	rc = pthread_mutex_unlock (  &pEvent->mutex );
	if ( rc != 0 )
	{
		return rc | PTHREAD_MUTEX_ERROR;
	}

	return 0;
}

UINT32 OSW_EventWait ( Event * pEvent, UINT32 Timeout )
{
	int rc = 0;
	UINT32 OswRc = OSW_OK;

	// critical section
	pthread_mutex_lock ( &pEvent->mutex );

	// check "event set" flag
	if ( pEvent->set )
	{
		// if already set reset and return immidiately
		pEvent->set = 0;
		pthread_mutex_unlock ( &pEvent->mutex );
		return 0;
	}
	// if wait forever
	if(OSW_WAIT_FOREVER == Timeout)
	{
		// wait forever
		pthread_cond_wait ( &pEvent->cond, &pEvent->mutex );
		pEvent->set = 0;
	}
	else
	{
		// prepare timer for timeout
		struct timeval now;
		struct timespec timeout;
		gettimeofday(&now, NULL);
		// calculate wakeup time
		timeout.tv_sec = now.tv_sec + (Timeout/1000);
		timeout.tv_nsec = (now.tv_usec*1000) + ((Timeout % 1000) * 1000000);
		if (timeout.tv_nsec >= 1000000000)
		{
			timeout.tv_nsec -= 1000000000;
			timeout.tv_sec++;			
		}

		// Wait for condition variable with timeout
		rc = pthread_cond_timedwait ( &pEvent->cond, &pEvent->mutex, &timeout);
		if(!rc)
		{
			pEvent->set = 0;		
		}
		else
		{
			OswRc = rc;
			if ( rc == ETIMEDOUT )
			{
				OswRc = OSW_TIMEOUT;
			}
		}
	}

	// exit critical section
	pthread_mutex_unlock (  &pEvent->mutex );
	return OswRc;
}

