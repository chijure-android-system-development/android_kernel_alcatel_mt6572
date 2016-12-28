


#include <stdio.h>
#include <unistd.h>
#include "Osw.h"
#include "SmsPlatDefs.h"

#define CWD_MAX_LEN		1024
#define CWD_MAX_FILENAME_LEN	256

char g_cwd[CWD_MAX_LEN];


UINT32 OSW_FS_Init( UINT32 arg )
{
	memset (g_cwd, 0, CWD_MAX_LEN);
	return 0;
}


UINT32 OSW_FS_SetCwd( const char* cwd )
{
	UINT32	cwd_len;

	cwd_len = strlen(cwd);
	if (cwd_len > CWD_MAX_LEN - 2)
		return 0x80000010;

	strcpy(g_cwd, cwd);
	
	/* force '/' at the end of the current working directory */
	if (g_cwd[cwd_len-1] != '/') {
		g_cwd[cwd_len] = '/';
		g_cwd[cwd_len+1] = 0;
	}
	
	return OSW_OK;	
}


OSW_FILEHANDLE OSW_FS_Open(	const char* filename,
					   	const char* attributes )
{
	UINT32	cwd_len;
	char	full_filename[CWD_MAX_LEN+CWD_MAX_FILENAME_LEN];

	cwd_len = strlen(g_cwd);
	
	strcpy(full_filename, g_cwd);

	strcpy(&full_filename[cwd_len], filename);
	
	return fopen(full_filename,attributes);
}


UINT32 OSW_FS_Close(OSW_FILEHANDLE hFile)
{
	return fclose(hFile);
}

UINT32 OSW_FS_Write(OSW_FILEHANDLE hFile,
					void* IN   pBuffer,
					UINT32	   buffLen)
{
	return fwrite(pBuffer,1,buffLen,hFile);
}

UINT32 OSW_FS_Read(	OSW_FILEHANDLE	hFile,
				   	void* OUT	pData,
				   	UINT32		dataLen)
{
	return fread(pData,1,dataLen,hFile);
}


UINT32 OSW_FS_Delete(const char* filename)
{	
	UINT32	cwd_len;
	char	full_filename[CWD_MAX_LEN+CWD_MAX_FILENAME_LEN];

	cwd_len = strlen(g_cwd);
	
	strcpy(full_filename, g_cwd);

	strcpy(&full_filename[cwd_len], filename);
	
	if (remove(full_filename) == 0)
	{
		return OSW_OK;
	}
	else
	{
		return OSW_ERROR;
	}
}

#ifdef SMS_OSW_FS_EXTENSIONS

UINT32 OSW_FS_Tell(	OSW_FILEHANDLE	hFile)
{
	return ftell(hFile); 
}



UINT32 OSW_FS_Printf(OSW_FILEHANDLE	hFile, const char *fmt, ...)
{

	UINT32 NumWritten; 
	va_list Args;

	va_start(Args, fmt);
	NumWritten = vfprintf(hFile, fmt, Args);
	va_end(Args);

	return NumWritten; 
}



BOOL OSW_FS_Flush(OSW_FILEHANDLE hFile)
{
	return fflush(hFile) == 0; 
}


BOOL OSW_FS_Seek(OSW_FILEHANDLE hFile, UINT32 offset, INT32 whence)
{
	const int WHENCE_VALS[] = {SEEK_SET, SEEK_CUR, SEEK_END};  

	return fseek(hFile, offset, WHENCE_VALS[whence]) == 0; 
}


BOOL OSW_FS_Putc(OSW_FILEHANDLE hFile, INT32 c)
{
	return fputc(c, hFile) != EOF; 
}


OSW_FILEHANDLE OSW_FS_Dopen(INT32 fd, const char *attributes)
{
	return (OSW_FILEHANDLE)fdopen(fd, attributes); 
}



void OSW_FS_ClearErr(OSW_FILEHANDLE hFile)
{
	clearerr(hFile); 
}

INT32 OSW_FS_Error(OSW_FILEHANDLE hFile)
{
	return ferror(hFile);
}

#endif
