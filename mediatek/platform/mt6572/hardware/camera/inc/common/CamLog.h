
#ifndef _MTK_CAMERA_INC_COMMON_CAMLOG_H_
#define _MTK_CAMERA_INC_COMMON_CAMLOG_H_


#if 1
//
#include <cutils/xlog.h>
#define CAM_LOGV(fmt, arg...)       XLOGV(fmt"\r\n", ##arg)
#define CAM_LOGD(fmt, arg...)       XLOGD(fmt"\r\n", ##arg)
#define CAM_LOGI(fmt, arg...)       XLOGI(fmt"\r\n", ##arg)
#define CAM_LOGW(fmt, arg...)       XLOGW(fmt"\r\n", ##arg)
#define CAM_LOGE(fmt, arg...)       XLOGE(fmt" (%s){#%d:%s}""\r\n", ##arg, __FUNCTION__, __LINE__, __FILE__)
//
#else
//
#include <cutils/log.h>
#define CAM_LOGV(fmt, arg...)       LOGV(fmt"\r\n", ##arg)
#define CAM_LOGD(fmt, arg...)       LOGD(fmt"\r\n", ##arg)
#define CAM_LOGI(fmt, arg...)       LOGI(fmt"\r\n", ##arg)
#define CAM_LOGW(fmt, arg...)       LOGW(fmt"\r\n", ##arg)
#define CAM_LOGE(fmt, arg...)       LOGE(fmt" (%s){#%d:%s}""\r\n", ##arg, __FUNCTION__, __LINE__, __FILE__)
//
#endif
//
//  ASSERT
#define CAM_LOGA(...) \
    do { \
        CAM_LOGE("[Assert] "__VA_ARGS__); \
        while(1) { ::usleep(500 * 1000); } \
    } while (0)
//
//
//  FATAL
#define CAM_LOGF(...) \
    do { \
        CAM_LOGE("[Fatal] "__VA_ARGS__); \
        LOG_ALWAYS_FATAL_IF(1, "(%s){#%d:%s}""\r\n", __FUNCTION__, __LINE__, __FILE__); \
    } while (0)


#define CAM_LOGV_IF(cond, ...)      do { if ( (cond) ) { CAM_LOGV(__VA_ARGS__); } }while(0)
#define CAM_LOGD_IF(cond, ...)      do { if ( (cond) ) { CAM_LOGD(__VA_ARGS__); } }while(0)
#define CAM_LOGI_IF(cond, ...)      do { if ( (cond) ) { CAM_LOGI(__VA_ARGS__); } }while(0)
#define CAM_LOGW_IF(cond, ...)      do { if ( (cond) ) { CAM_LOGW(__VA_ARGS__); } }while(0)
#define CAM_LOGE_IF(cond, ...)      do { if ( (cond) ) { CAM_LOGE(__VA_ARGS__); } }while(0)
#define CAM_LOGA_IF(cond, ...)      do { if ( (cond) ) { CAM_LOGA(__VA_ARGS__); } }while(0)
#define CAM_LOGF_IF(cond, ...)      do { if ( (cond) ) { CAM_LOGF(__VA_ARGS__); } }while(0)


#endif  //_MTK_CAMERA_INC_COMMON_CAMLOG_H_

