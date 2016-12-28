
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

#ifndef _SMS_HOST_LIB_LITE_CMMB_H_
#define _SMS_HOST_LIB_LITE_CMMB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "SmsHostLibTypes.h" 

#ifndef SMS_LITE_CB_DEFINED
#define SMS_LITE_CB_DEFINED

//! Callback function prototype for control response messages
typedef void (*SmsHostLiteCbFunc)(	SMSHOSTLIB_MSG_TYPE_RES_E	MsgType,		//!< Response type	
	SMSHOSTLIB_ERR_CODES_E		ErrCode,		//!< Response success code
	UINT8* 						pPayload,		//!< Response payload
	UINT32						PayloadLen );	//!< Response payload length

//! Callback function prototype for service data
typedef void ( *SmsHostLiteDataCbFunc)(UINT32	ServiceHandle, 
	UINT8*	pBuffer, 
	UINT32	BufferSize );

#endif
	
//! Structure of the init parameters passed to #SmsLiteCmmbLibInit
typedef struct SMSHOSTLIBLITE_CMMB_INITLIB_PARAMS_S
{
	UINT32						Size;				//!< Put sizeof(#SMSHOSTLIBLITE_CMMB_INITLIB_PARAMS_ST)into this field
	SmsHostLiteCbFunc			pCtrlCallback;		//!< Callback for control responses
	SmsHostLiteDataCbFunc		pDataCallback;		//!< Callback for asynchronous data reading
	UINT32						Crystal;			//!< The crystal frequency used in the chip. 12MHz is the default - use 0 to leave unchanged.
} SMSHOSTLIBLITE_CMMB_INITLIB_PARAMS_ST;



/*************************************************************************/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API SmsLiteCmmbLibInit( SMSHOSTLIBLITE_CMMB_INITLIB_PARAMS_ST* pInitLibParams );

/*************************************************************************/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API SmsLiteCmmbLibTerminate( void );


/*************************************************************************/
void SMSHOSTLIB_API SmsLiteGetVersion_Req( void );

/*************************************************************************/
void SMSHOSTLIB_API SmsLiteCmmbGetStatistics_Req( void ); 

/*************************************************************************/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API SmsLiteSetDbgLogMask(UINT32 newDbgLogMask);

/*************************************************************************/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API SmsLiteGetDbgLogMask(UINT32* pCurDbgLogMask);

/*************************************************************************/
void SMSHOSTLIB_API SmsLiteCmmbTune_Req( UINT32 Frequency, SMSHOSTLIB_FREQ_BANDWIDTH_ET Bandwidth );

/*************************************************************************/
void SMSHOSTLIB_API SmsLiteCmmbStartTs0_Req( void );

/*************************************************************************/
void SMSHOSTLIB_API SmsLiteCmmbStopTs0_Req( void );

/*************************************************************************/
void SMSHOSTLIB_API SmsLiteCmmbStartService_Req( UINT32 ServiceId );

/*************************************************************************/
void SMSHOSTLIB_API SmsLiteCmmbStopService_Req( UINT32 ServiceHandle ); 

/*************************************************************************/
void SMSHOSTLIB_API SmsLiteCmmbSetPeriodicStatistics_Req( BOOL IsEnabled );

/*************************************************************************/
void SMSHOSTLIB_API SmsLiteCmmbSetAutoOuputTs0_Req( BOOL IsEnabled );

/*************************************************************************/
void SMSHOSTLIB_API SmsLiteCmmbSetCaControlWords_Req( UINT32 SvcHdl, UINT32 SfIdx, SMSHOSTLIB_CA_CW_PAIR_ST *pControlWords);



/*************************************************************************/
void SMSHOSTLIB_API SmsLiteCmmbSetCaSaltKeys_Req( UINT32 SvcHdl, 
												 UINT32 SfIdx, 
												 UINT8 pVidSalt[SMSHOSTLIB_CMMB_CA_SALT_SIZE], 
												 UINT8 pAudSalt[SMSHOSTLIB_CMMB_CA_SALT_SIZE],
												 UINT8 pDataSalt[SMSHOSTLIB_CMMB_CA_SALT_SIZE] );

/*************************************************************************/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API SmsLiteCmmbUAMInit (void);


/*************************************************************************/
SMSHOSTLIB_ERR_CODES_E SMSHOSTLIB_API SmsLiteCmmbUAMSend (UINT8* pBuff, UINT32 size);


/*************************************************************************/
void SMSHOSTLIB_API SmsLiteCmmbMbbmsProtoSend_Req(UINT8* pBuff, UINT32 Size);


/*************************************************************************/
void SMSHOSTLIB_API SmsLiteCmmbSmdSn_Req( void ); 


/*************************************************************************/
void SMSHOSTLIB_API
	SmsLiteCmmbSetEadtMockup( UINT32 EmmServiceId, UINT32 ReservedZero1, UINT32 ReservedZero2 );



#ifdef __cplusplus
}
#endif

#endif //_SMS_HOST_LIB_LITE_CMMB_H_
