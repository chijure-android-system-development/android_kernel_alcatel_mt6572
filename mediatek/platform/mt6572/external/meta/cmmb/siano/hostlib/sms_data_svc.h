
/*************************************************************************/
/*                                                                       */
/* Copyright (C) 2005,2006 Siano Mobile Silicon Ltd. All rights reserved */
/*                                                                       */
/* PROPRIETARY RIGHTS of Siano Mobile Silicon are involved in the        */
/* subject matter of this material.  All manufacturing, reproduction,    */
/* use, and sales rights pertaining to this subject matter are governed  */
/* by the license agreement.  The recipient of this software implicitly  */
/* accepts the terms of the license.                                     */
/*                                                                       */
/*                                                                       */
/*************************************************************************/

/*************************************************************************/
/*                                                                       */
/* FILE NAME                                                             */
/*                                                                       */
/*					                                                     */
/*                                                                       */
/* COMPONENT                                                             */
/*                                                                       */
/*																		 */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*************************************************************************/
#ifndef _SMS_DATA_SVC_H__
#define _SMS_DATA_SVC_H__

#include "SmsPlatDefs.h"

#ifdef __cplusplus
extern "C" {
#endif





typedef struct SmsDataSvcFatUpdateInfo_S
{
	UINT32 FatMinorVersion;         // Minor version of the FAT taken from the FAT XML.
	UINT32 FatMajorVersion;			// Major version of the FAT taken from the FAT XML.
	UINT32 FatUpdateNum;			// Update number (0-31) of the FAT. 
	UINT32 NumFiles;				// Number of files in this update
	const char** pFilePathsArr;		// Array of strings containing the paths of the files
									// included in this update
	UINT32 Reserved[4];				// Reserved for future use
} SmsDataSvcFatUpdateInfo_ST;		 
									 
typedef void (*SmsDataSvcUpdateBeginCb)( void* ClientPtr, UINT32 SvcHdl, 
										UINT32 SfIndex, SmsDataSvcFatUpdateInfo_ST* pFatUpdateInfo );

typedef void (*SmsDataSvcUpdateFinishCb)( void* ClientPtr, UINT32 SvcHdl, 
										UINT32 SfIndex, SmsDataSvcFatUpdateInfo_ST* pFatUpdateInfo);

typedef struct SmsDataInitiateParams_S
{
	const char* pBaseDirPath;					// Base dir for writing the data service files
												// all paths of the delivered files will begin with 
												// this base dir.

	SmsDataSvcUpdateBeginCb pfnUpdateBeginCb;	// Update begin callback pointer - see #SmsDataSvcUpdateBeginCb
	SmsDataSvcUpdateFinishCb pfnUpdateFinishCb;	// Update finish callback pointer - see #SmsDataSvcUpdateFinishCb

	void* ClientPtr;							// A client pointer that will be passed to the begin
												// and finish callbacks
	UINT32 ReservedZero[6];
} SmsDataInitiateParams_ST;
	

SMSHOSTLIB_API SMSHOSTLIB_ERR_CODES_E SmsDataSvcInitiate(UINT32 SvcHdl,
							UINT32 SfIndex, 
							SmsDataInitiateParams_ST* pParams);


SMSHOSTLIB_API SMSHOSTLIB_ERR_CODES_E SmsDataSvcCleanup(UINT32 SvcHdl, UINT32 SfIndex);


SMSHOSTLIB_API SMSHOSTLIB_ERR_CODES_E SmsDataSvcSetBaseDir(UINT32 SvcHdl, UINT32 SfIndex, 
											   const char* pNewBaseDir);

SMSHOSTLIB_API SMSHOSTLIB_ERR_CODES_E SmsDataSvcProcessServiceFrame(UINT32 SvcHdl, 
							UINT32 SfIndex, 
							UINT8* pMpxFrame,
							UINT32 MpxFrameSize);


#ifdef __cplusplus
}
#endif

#endif	//_SMS_DATA_SVC_H__