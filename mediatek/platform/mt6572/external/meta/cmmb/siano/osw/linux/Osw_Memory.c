

#include "Osw.h"
#include <stdlib.h>
#include <string.h>


void *OSW_MemAlloc ( UINT32 size )
{
	return malloc ( size );
}

void OSW_MemFree ( void *pMemToFree )
{
	free ( pMemToFree );
}

