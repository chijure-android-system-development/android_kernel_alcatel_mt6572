

#include "Osw.h"
#include "SmsPlatDefs.h"


UINT32 OSW_MutexCreate (MUTEX * mutex )
{
	pthread_mutexattr_t attr;
	SMS_ASSERT ( mutex );

	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype((pthread_mutexattr_t *)&attr, (int)PTHREAD_MUTEX_RECURSIVE_NP);

	// init mutex
	int rc = pthread_mutex_init ( mutex, &attr );
	if ( rc != 0 )
	{
		return PTHREAD_MUTEX_ERROR | rc;
	}

	return rc;
}

UINT32 OSW_MutexDelete ( MUTEX * mutex )
{

	SMS_ASSERT ( mutex );
	// destroy mutex
	int rc = pthread_mutex_destroy ( mutex );
	if ( rc != 0 )
	{
		return PTHREAD_MUTEX_ERROR | rc;
	}

	return rc;
}

UINT32 OSW_MutexPut ( MUTEX * mutex )
{
	SMS_ASSERT ( mutex );

	// unlock mutex
	int rc = pthread_mutex_unlock ( mutex );
	if ( rc != 0 )
	{
		return PTHREAD_MUTEX_ERROR | rc;
	}
	return rc;
}

UINT32 OSW_MutexGet ( MUTEX * mutex )
{
	SMS_ASSERT ( mutex );

	// get mutex
	int rc = pthread_mutex_lock ( mutex );
	if ( rc != 0 )
	{
		return PTHREAD_MUTEX_ERROR | rc;
	}
	return rc;
}
