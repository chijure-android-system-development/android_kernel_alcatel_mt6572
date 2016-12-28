//#ifndef _DRV_LOG_H_   // Can't use Header Guard because we use undef-redefine to apply new DBG_LOG_TAG and DBG_LOG_LEVEL for each file.
//#define _DRV_LOG_H_






// Vent@20120615: Don't know why "__func__" and "__FUNCTION__" act like
//     "__PRETTY_FUNCTION__". Add following lines as a workaround to make
//     their behavior correct.
#undef	__func__
#define	__func__	__FUNCTION__

///////////////////////////////////////////////////////////////////////////
//                          Default Settings.                            //
///////////////////////////////////////////////////////////////////////////
#ifndef DBG_LOG_TAG     // Set default DBG_LOG_TAG.
    #define DBG_LOG_TAG     ""
#endif  // DBG_LOG_TAG

///////////////////////////////////////////////////////////////////////////
//                      Base Debug Log Functions                         //
///////////////////////////////////////////////////////////////////////////
#ifndef USING_MTK_LDVT   // Not using LDVT.
    #include <cutils/xlog.h>
    #define NEW_LINE_CHAR   ""      // XLOG?() already includes a new line char at the end of line, so don't have to add one.

    #define BASE_LOG_VRB(fmt, arg...)       XLOGV(DBG_LOG_TAG "[%s] " fmt NEW_LINE_CHAR, __func__, ##arg)	// <Verbose>: Show more detail debug information. E.g. Entry/exit of private function; contain of local variable in function or code block; return value of system function/API...
    #define BASE_LOG_DBG(fmt, arg...)       XLOGD(DBG_LOG_TAG "[%s] " fmt NEW_LINE_CHAR, __func__, ##arg)	// <Debug>: Show general debug information. E.g. Change of state machine; entry point or parameters of Public function or OS callback; Start/end of process thread...
    #define BASE_LOG_INF(fmt, arg...)       XLOGI(DBG_LOG_TAG "[%s] " fmt NEW_LINE_CHAR, __func__, ##arg)	// <Info>: Show general system information. Like OS version, start/end of Service...
    #define BASE_LOG_WRN(fmt, arg...)       XLOGW(DBG_LOG_TAG "[%s] WARNING: " fmt NEW_LINE_CHAR, __func__, ##arg)	// <Warning>: Some errors are encountered, but after exception handling, user won't notice there were errors happened.
    #define BASE_LOG_ERR(fmt, arg...)       XLOGE(DBG_LOG_TAG "[%s, %s, line%04d] ERROR: " fmt NEW_LINE_CHAR, __FILE__, __func__, __LINE__, ##arg)	// When MP, will only show log of this level. // <Fatal>: Serious error that cause program can not execute. <Error>: Some error that causes some part of the functionality can not operate normally.
    #define BASE_LOG_AST(cond, fmt, arg...)     \
        do {        \
            if (!(cond))        \
                XLOGE("[%s, %s, line%04d] ASSERTION FAILED! : " fmt NEW_LINE_CHAR, __FILE__, __func__, __LINE__, ##arg);        \
        } while (0)

#else   // Using LDVT.
    #include "uvvf.h"
    #define NEW_LINE_CHAR   "\n"

    #define BASE_LOG_VRB(fmt, arg...)        VV_MSG(DBG_LOG_TAG "[%s] " fmt NEW_LINE_CHAR, __func__, ##arg)	// <Verbose>: Show more detail debug information. E.g. Entry/exit of private function; contain of local variable in function or code block; return value of system function/API...
    #define BASE_LOG_DBG(fmt, arg...)        VV_MSG(DBG_LOG_TAG "[%s] " fmt NEW_LINE_CHAR, __func__, ##arg)	// <Debug>: Show general debug information. E.g. Change of state machine; entry point or parameters of Public function or OS callback; Start/end of process thread...
   	#define BASE_LOG_INF(fmt, arg...)        VV_MSG(DBG_LOG_TAG "[%s] " fmt NEW_LINE_CHAR, __func__, ##arg)	// <Info>: Show general system information. Like OS version, start/end of Service...
    #define BASE_LOG_WRN(fmt, arg...)        VV_MSG(DBG_LOG_TAG "[%s] WARNING: " fmt NEW_LINE_CHAR, __func__, ##arg)	// <Warning>: Some errors are encountered, but after exception handling, user won't notice there were errors happened.
  	#define BASE_LOG_ERR(fmt, arg...)        VV_ERRMSG(DBG_LOG_TAG "[%s, %s, line%04d] ERROR: " fmt NEW_LINE_CHAR, __FILE__, __func__, __LINE__, ##arg)	// When MP, will only show log of this level. // <Fatal>: Serious error that cause program can not execute. <Error>: Some error that causes some part of the functionality can not operate normally.
    #define BASE_LOG_AST(cond, fmt, arg...)     \
        do {        \
            if (!(cond))        \
                VV_ERRMSG("[%s, %s, line%04d] ASSERTION FAILED! : " fmt NEW_LINE_CHAR, __FILE__, __func__, __LINE__, ##arg);        \
        } while (0)

#endif  // USING_MTK_LDVT

///////////////////////////////////////////////////////////////////////////
//          Macros for dynamically changing debug log level              //
///////////////////////////////////////////////////////////////////////////
// [Set default debug log level here] (when DBG_LOG_LEVEL_SET() is not called.)
// Change the desired level (and following levels) to true.
// e.g. Change DEBUG and all level below DEBUG to true, then debug level is DEBUG.
#define DECLARE_DBG_LOG_VARIABLE(ModuleName)    \
    bool ModuleName ## _DbgLogEnable_VERBOSE   = false; \
    bool ModuleName ## _DbgLogEnable_DEBUG     = false; \
    bool ModuleName ## _DbgLogEnable_INFO      = true;  \
    bool ModuleName ## _DbgLogEnable_WARN      = true;  \
    bool ModuleName ## _DbgLogEnable_ERROR     = true;  \
    bool ModuleName ## _DbgLogEnable_ASSERT    = true;  \

#define EXTERN_DBG_LOG_VARIABLE(ModuleName)    \
    extern bool ModuleName ## _DbgLogEnable_VERBOSE;    \
    extern bool ModuleName ## _DbgLogEnable_DEBUG;      \
    extern bool ModuleName ## _DbgLogEnable_INFO;       \
    extern bool ModuleName ## _DbgLogEnable_WARN;       \
    extern bool ModuleName ## _DbgLogEnable_ERROR;      \
    extern bool ModuleName ## _DbgLogEnable_ASSERT;     \

// [Set default debug log level here] (when DBG_LOG_LEVEL_SET() is called.)
// Note: The default value in the property_get() does not controls the default
//       debug log level. It's the position of "default:" in the switch() controls
//       the default debug log level. E.g. put "default:" to case '5' will make
//       default debug level to WARN.
// PropertyStr: the Android property name you will use to control the debug level.
#define DBG_LOG_CONFIG(GroupName, ModuleName)                       \
    do {                                                            \
        char acDbgLogLevel[32] = {'\0'};                            \
        property_get("debuglog." #GroupName "." #ModuleName, acDbgLogLevel, "0");             \
        ModuleName ## _DbgLogEnable_VERBOSE   = false;              \
        ModuleName ## _DbgLogEnable_DEBUG     = false;              \
        ModuleName ## _DbgLogEnable_INFO      = false;              \
        ModuleName ## _DbgLogEnable_WARN      = false;              \
        ModuleName ## _DbgLogEnable_ERROR     = false;              \
        ModuleName ## _DbgLogEnable_ASSERT    = false;              \
        if (acDbgLogLevel[0] == '0')                                \
        {                                                           \
            property_get("debuglog." #GroupName, acDbgLogLevel, "9");   \
        }                                                           \
        switch (acDbgLogLevel[0])                                   \
        {                                                           \
            case '2':                                               \
                ModuleName ## _DbgLogEnable_VERBOSE   = true;       \
            case '3':                                               \
                ModuleName ## _DbgLogEnable_DEBUG     = true;       \
            default:                                                \
            case '4':                                               \
                ModuleName ## _DbgLogEnable_INFO      = true;       \
            case '5':                                               \
                ModuleName ## _DbgLogEnable_WARN      = true;       \
            case '6':                                               \
                ModuleName ## _DbgLogEnable_ERROR     = true;       \
            case '7':                                               \
                ModuleName ## _DbgLogEnable_ASSERT    = true;       \
            case '8':                                               \
                break;                                              \
        }                                                           \
    } while (0)

///////////////////////////////////////////////////////////////////////////
//                          Other Definition.                            //
///////////////////////////////////////////////////////////////////////////

//#if (S_DbgLogEnable_DEBUG)
//    #define DBG_ISP_REG_DUMP    // A flag to control whether ISP Reg DUMP will be performed or not.
//#endif


//#endif  // _DRV_LOG_H_

