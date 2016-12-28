
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
/*      AppDriver.c                                                      */
/*                                                                       */
/* COMPONENT                                                             */
/*                                                                       */
/*      Application driver unified interface   	                         */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*************************************************************************/

#ifndef _APP_DRIVER_INT_H
#define _APP_DRIVER_INT_H

#include "SmsHostLibTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

#define	ADR_CTRL_HANDLE		0

///////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////
typedef void ( *SmsLiteAdr_pfnFuncCb )( UINT32 handle_num, UINT8* p_buffer, UINT32 buff_size );


///////////////////////////////////////////////////////////////////////////////
SMSHOSTLIB_ERR_CODES_E SmsLiteAdrInit( SMSHOSTLIB_DEVICE_MODES_E DeviceMode, 
									  SmsLiteAdr_pfnFuncCb pfnControlCb, 
									  SmsLiteAdr_pfnFuncCb pfnDataCb );

///////////////////////////////////////////////////////////////////////////////
SMSHOSTLIB_ERR_CODES_E SmsLiteAdrTerminate( void );

///////////////////////////////////////////////////////////////////////////////
SMSHOSTLIB_ERR_CODES_E SmsLiteAdrWriteMsg( SmsMsgData_ST* p_msg );

///////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif // _APP_DRIVER_INT_H
