
#define LOG_TAG "MtkCam/CamAdapter"
//
#include <inc/CamUtils.h>
using namespace android;
using namespace MtkCamUtils;
//
#include <inc/ImgBufProvidersManager.h>
//
#include <camera/MtkCamera.h>
//
#include <drv/sensor_hal.h>
//
#include <IParamsManager.h>
//
#include <ICamAdapter.h>
#include <inc/BaseCamAdapter.h>
#include <inc/MtkDefaultCamAdapter.h>
using namespace NSMtkDefaultCamAdapter;
//
#include <cutils/properties.h>
//
#define MY_LOGV(fmt, arg...)        CAM_LOGV("(%d)(%s)[%s] "fmt, ::gettid(), getName(), __FUNCTION__, ##arg)
#define MY_LOGD(fmt, arg...)        CAM_LOGD("(%d)(%s)[%s] "fmt, ::gettid(), getName(), __FUNCTION__, ##arg)
#define MY_LOGI(fmt, arg...)        CAM_LOGI("(%d)(%s)[%s] "fmt, ::gettid(), getName(), __FUNCTION__, ##arg)
#define MY_LOGW(fmt, arg...)        CAM_LOGW("(%d)(%s)[%s] "fmt, ::gettid(), getName(), __FUNCTION__, ##arg)
#define MY_LOGE(fmt, arg...)        CAM_LOGE("(%d)(%s)[%s] "fmt, ::gettid(), getName(), __FUNCTION__, ##arg)
//
#define MY_LOGV_IF(cond, arg...)    if (cond) { MY_LOGV(arg); }
#define MY_LOGD_IF(cond, arg...)    if (cond) { MY_LOGD(arg); }
#define MY_LOGI_IF(cond, arg...)    if (cond) { MY_LOGI(arg); }
#define MY_LOGW_IF(cond, arg...)    if (cond) { MY_LOGW(arg); }
#define MY_LOGE_IF(cond, arg...)    if (cond) { MY_LOGE(arg); }
namespace {




// Destroy Criteria 
// (1) stopSmoothZoom (should wait at least one frame has been callback)
// (2) reach targetZoom naturally
// (3) uninit by camadapter
// (4) resett for startSmoothZoom

class ZoomCallBack : public IPreviewCmdQueCallBack 
{

private:
    mutable Mutex       mLock;
    sp<CamMsgCbInfo>    mspCamMsgCbInfo; 
    sp<IParamsManager>  mspParamsMgr;    
    int32_t             mTargetIdx;
    int32_t             mIniIdx;
    int32_t             mPrevIdx;
    int32_t             mCBcnt;
    bool                mRequestStop;
    Condition           mCondReqStop;
    
private:
    void                reset() 
                        {
                            mspCamMsgCbInfo = 0;
                            mspParamsMgr = 0;
                            mTargetIdx = mIniIdx = mCBcnt = 0; 
                            mPrevIdx = -1;
                            mRequestStop = false;
                        }

// for CTS case, if stopSmoothZoom follows with startSmoothZoom quickly,
// must quarentee one callback at least has been sent to AP. 
public:
    bool                requestStop();
        
public:

    //  Notify Callback of Zoom
    //  Arguments:
    //
    //      _msgType: 
    //       
    //
    //      _ext1:

    virtual void        doNotifyCb (
                            int32_t _msgType, 
                            int32_t _ext1, 
                            int32_t _ext2,
                            int32_t _ext3
                        );
    
// class member
public:
    
    virtual             ~ZoomCallBack(){}
                        ZoomCallBack()
                            : mLock()                            
                            , mspCamMsgCbInfo(0)
                            , mspParamsMgr(0)
                            , mTargetIdx(0)
                            , mIniIdx(0)
                            , mPrevIdx(-1)
                            , mRequestStop(false)
                        {}
    
    virtual void        setZoomTarget(int32_t cIdx, int32_t tIdx)
                        {
                            mIniIdx = cIdx;
                            mTargetIdx = tIdx; 
                            mPrevIdx = -1;
                        }   

    virtual void        setUser(sp<CamMsgCbInfo> const &rpCamMsgCbInfo,
                                sp<IParamsManager> const &rpParamsMgr) 
                        { 
                            Mutex::Autolock lock(mLock);
                            
                            reset(); //destroy criteria (4)
                            mspCamMsgCbInfo = rpCamMsgCbInfo; 
                            mspParamsMgr = rpParamsMgr;
                        }       
    
    virtual void        destroyUser() // destroy criteria (3) 
                        {
                            Mutex::Autolock lock(mLock);
                            reset();
                        }
};


bool
ZoomCallBack::
requestStop()
{
    Mutex::Autolock lock(mLock);
    
    mRequestStop = true;
    nsecs_t nsTimeoutToWait = 33LL*1000LL*1000LL;//wait 33 msecs.    
    status_t status = mCondReqStop.waitRelative(mLock, nsTimeoutToWait);

    return status == OK;
}



void
ZoomCallBack::
doNotifyCb(
    int32_t _msgType, 
    int32_t _ext1, 
    int32_t _ext2,
    int32_t _ext3
)
{
    Mutex::Autolock lock(mLock);

    if( mspCamMsgCbInfo == 0)
    {
        CAM_LOGW("mspCamMsgCbInfo == 0");
        return;
    }
    
    if (mPrevIdx == _ext1)
    {
        CAM_LOGW("same as previous idx, drop!");
        return;
    }
    //
    bool retStop = false;
    if ( _msgType == IPreviewCmdQueCallBack::eID_NOTIFY_Zoom &&
         _ext1 != mIniIdx )
    {
        retStop = (mRequestStop)||(_ext1 == mTargetIdx);
        CAM_LOGD("smoothZoom cb(#%d): (%d, %d)", mCBcnt++, _ext1, retStop); 
        mspParamsMgr->set(CameraParameters::KEY_ZOOM, _ext1);        
        mspCamMsgCbInfo->mNotifyCb(CAMERA_MSG_ZOOM, _ext1,  retStop, mspCamMsgCbInfo->mCbCookie);       
        mPrevIdx = _ext1;
    }

    //
    if ( retStop ) // destroy criteria (1)&(2)
    {
        mCondReqStop.broadcast(); 
        reset();
    }    
}

static ZoomCallBack gZoomCallback;

}; // end of namespace


status_t
CamAdapter::startSmoothZoom(int32_t arg1)
{
    MY_LOGD("+ (%d)", arg1);

    int32_t curIdx = mpParamsMgr->getInt(CameraParameters::KEY_ZOOM);

    if( arg1 < 0 || arg1 > mpParamsMgr->getInt(CameraParameters::KEY_MAX_ZOOM) )
    {
        MY_LOGE("return fail: smooth zoom(%d)", arg1);
        return BAD_VALUE;
    }

    if( arg1 == curIdx )
    {
        MY_LOGE("smooth zoom(%d) equals to current", arg1);
        return OK; 
    }
    
    // (0) initial setting
    gZoomCallback.setUser(mpCamMsgCbInfo, mpParamsMgr);
    gZoomCallback.setZoomTarget(curIdx, arg1);
        
    // (1) once callback has been set to PreviewCmdQueThread,
    //     it contains this pointer forever, 
    mpPreviewCmdQueThread->setZoomCallback(&gZoomCallback);

    //(2)
    while( mpPreviewCmdQueThread->popZoom() != -1 );

    // (3)
    if (curIdx < arg1)
    {
        for (int32_t i = curIdx+1; i <= arg1; i++)
        {
            mpPreviewCmdQueThread->pushZoom((uint32_t)i);
        }
    }
    else if (curIdx > arg1)
    {
        for (int32_t i = curIdx-1; i >= arg1; i--)
        {
            mpPreviewCmdQueThread->pushZoom((uint32_t)i);
        }
    }
    
    MY_LOGD("-");
    
    return OK;
}


status_t
CamAdapter::stopSmoothZoom()
{
    MY_LOGD("+");

    //
    int popIdx = -1;
    Vector<int> vPopIdx;
    while( -1 != (popIdx = mpPreviewCmdQueThread->popZoom()) )
    {
        vPopIdx.push_back(popIdx);
    }


    // To guarentee:
    // (1) must send one callback if AP has ever sent startSmoothZoom;
    // (2) the last callback must indicate "stop";  
    while( ! vPopIdx.empty() && ! gZoomCallback.requestStop())
    {
        MY_LOGD("Push Idx(%d) for compensation", *(vPopIdx.end()-1));
        mpPreviewCmdQueThread->pushZoom(*(vPopIdx.end()-1));
        vPopIdx.erase(vPopIdx.end()-1);
    }
    
    MY_LOGD("-");    
    
    return OK;
}


void
CamAdapter::uninitSmoothZoom()
{
    gZoomCallback.destroyUser();
}


