

#ifndef __OSW_FS_H
#define __OSW_FS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Osw.h"

#ifndef _DOXYGEN_EXCLUDE

#define OSW_FS_SEEK_SET 0
#define OSW_FS_SEEK_CUR 1
#define OSW_FS_SEEK_END 2

#endif //_DOXYGEN_EXCLUDE

UINT32 OSW_FS_Init( UINT32 arg );


UINT32 OSW_FS_SetCwd( const char* cwd );


OSW_FILEHANDLE OSW_FS_Open(	const char* filename,
					   	const char* attributes );


UINT32 OSW_FS_Close(OSW_FILEHANDLE hFile);


UINT32 OSW_FS_Write(OSW_FILEHANDLE hFile,
					void*      pBuffer,
					UINT32	   buffLen);

UINT32 OSW_FS_Read(	OSW_FILEHANDLE	hFile,
				   	void*   	pData,
				   	UINT32		dataLen);


UINT32 OSW_FS_Delete(const char* filename);




//File system abstraction extensions declarations. 
//Platform dependent implementation of these functions is optional. 
//When SMS_OSW_FS_EXTENSIONS compilation flag is set, platform dependent code must define these functions. 
//When SMS_OSW_FS_EXTENSIONS compilation flag is not set, hostlib will be linked with stubs.
//Implementing the extensions is required for the imported zlib code to work 
//(currently used only by CMMB ESG parser). 


UINT32 OSW_FS_Tell(	OSW_FILEHANDLE	hFile);


UINT32 OSW_FS_Printf(OSW_FILEHANDLE	hFile, const char *fmt, ...);


BOOL OSW_FS_Flush(OSW_FILEHANDLE hFile);

BOOL OSW_FS_Seek(OSW_FILEHANDLE hFile, UINT32 offset, INT32 whence);

BOOL OSW_FS_Putc(OSW_FILEHANDLE hFile, INT32 c);

OSW_FILEHANDLE OSW_FS_Dopen(INT32 fd, const char *attributes); 



void OSW_FS_ClearErr(OSW_FILEHANDLE hFile);


INT32 OSW_FS_Error(OSW_FILEHANDLE hFile);

#ifdef __cplusplus
}
#endif
#endif

