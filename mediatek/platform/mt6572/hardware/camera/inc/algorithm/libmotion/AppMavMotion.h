

#ifndef _APP_MAV_MOTION_H
#define _APP_MAV_MOTION_H

#include "MTKMotionType.h"
#include "MTKMotion.h"

class AppMavMotion : public MTKMotion {
public:    
    static MTKMotion* getInstance();
    virtual void destroyInstance();
    
    AppMavMotion();
    virtual ~AppMavMotion();   
    // Process Control
    MRESULT MotionInit(void *InitInData, void *InitOutData);
    MRESULT MotionMain(void);					// START
    MRESULT MotionExit(void);					// EXIT

	// Feature Control        
	MRESULT MotionFeatureCtrl(MUINT32 FeatureID, void* pParaIn, void* pParaOut);	
private:
   

};

#endif

