
#define LOG_TAG "paramctrl_frameless"

#ifndef ENABLE_MY_LOG
    #define ENABLE_MY_LOG       (1)
#endif

#include <aaa_types.h>
#include <aaa_log.h>
#include <aaa_hal_base.h>
#include <camera_custom_nvram.h>
#include <awb_param.h>
#include <ae_param.h>
#include <af_param.h>
#include <flash_param.h>
#include <isp_tuning.h>
#include <camera_feature.h>
#include <isp_tuning_cam_info.h>
#include <isp_tuning_idx.h>
#include <isp_tuning_custom.h>
#include <isp_mgr.h>
#include <isp_mgr_helper.h>
#include <pca_mgr.h>
#include <lsc_mgr.h>
#include <dbg_isp_param.h>
#include "paramctrl_if.h"
#include "paramctrl.h"

using namespace android;
using namespace NSIspTuning;


MBOOL
Paramctrl::
applyToHw_Frameless_All()
{
    return  MTRUE
        //&&  ISP_MGR_SHADING_T::getInstance().apply()
        ;
}


MBOOL
Paramctrl::
prepareHw_Frameless_All()
{
    MBOOL fgRet = MTRUE;

    //  (1) prepare something and fill buffers.
    fgRet = MTRUE
        &&  prepare_Frameless_Shading()
            ;
    if  ( ! fgRet )
    {
        goto lbExit;
    }

lbExit:
    return  fgRet;
}


MBOOL
Paramctrl::
prepare_Frameless_Shading()
{
    static MBOOL MetaModeInit = MFALSE;
    MBOOL fgRet = MTRUE;

    MY_LOG("[%s] mode %d, profile %d", __FUNCTION__, getOperMode(), m_rIspCamInfo.eIspProfile);
//    printf("[%s] OpMode %d, profile %d\n", __FUNCTION__,
//            getOperMode(),
//            m_rIspCamInfo.eIspProfile);

    switch  (getOperMode())
    {
    case EOperMode_Normal:
    case EOperMode_PureRaw:
        MY_LOG("%s m_pLscMgr EOperMode_Normal", __FUNCTION__);
        m_pLscMgr->setIspProfile(m_rIspCamInfo.eIspProfile);
        m_pLscMgr->SetTBAToISP();
        m_pLscMgr->enableLsc(MTRUE);
        break;
    case EOperMode_EM:
    case EOperMode_Meta:
        MY_LOG("%s EOperMode_Meta", __FUNCTION__);
            m_pLscMgr->setMetaIspProfile(m_rIspCamInfo.eIspProfile, getSensorMode());
            m_pLscMgr->ConfigUpdate();
            m_pLscMgr->SetTBAToISP();
            m_pLscMgr->enableLsc(MTRUE);
        break;
    default:
        MY_LOG("[%s] Wrong OpMode %d", __FUNCTION__, getOperMode());
        return MTRUE;
    }

    // debug message
    m_rIspCamInfo.eIdx_Shading_CCT = (NSIspTuning::EIndex_Shading_CCT_T)m_pLscMgr->getCTIdx();
    m_IspNvramMgr.setIdx_LSC(m_pLscMgr->getRegIdx());
#if 0
#endif
    return  fgRet;
}

