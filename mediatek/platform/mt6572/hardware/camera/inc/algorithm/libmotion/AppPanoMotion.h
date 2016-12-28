

#ifndef _APP_PANO_MOTION_H
#define _APP_PANO_MOTION_H

#include "MTKMotionType.h"
#include "MTKMotion.h"


class AppPanoMotion : public MTKMotion {
public:    
    static MTKMotion* getInstance(DrvMotionObject_e eobject);
    virtual void destroyInstance();
    
    AppPanoMotion();
    virtual ~AppPanoMotion();   
    // Process Control
    MRESULT MotionInit(void* InitInData, void* InitOutData);
    MRESULT MotionMain(void);					
    MRESULT MotionExit(void);					

	// Feature Control        
	MRESULT MotionFeatureCtrl(MUINT32 FeatureID, void* pParaIn, void* pParaOut);	
private:
   
    MOTION_STATE_ENUM	m_PanoMotionState;

    MTKMotionEnvInfo  m_PanoMotionEnvInfo;
    MTKMotionProcInfo m_PanoMotionProcInfo;


    MUINT32 m_MotionCurrProcNum;
    MUINT32 m_MotionCurrImageNum;
    MUINT32 m_MotionGetResultNum;
};

#endif

