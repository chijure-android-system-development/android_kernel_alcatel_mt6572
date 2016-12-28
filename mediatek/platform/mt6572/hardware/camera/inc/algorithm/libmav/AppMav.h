

#ifndef _APP_MAV_H
#define _APP_MAV_H

#include "MTKMav.h"

class AppMav : public MTKMav {
public:    
    static MTKMav* getInstance(DrvMavObject_e eobject);
    virtual void destroyInstance();
    
    AppMav();
    virtual ~AppMav();   
    // Process Control
    MRESULT MavInit(void* InitInData, void* InitOutData);
    MRESULT MavMain(void);					// START
    MRESULT MavReset(void);					// RESET
    MRESULT MavMerge(MUINT32 *MavResult);	// MERGE        
    static void* MavThreadFunc(void *ParaIn);

	// Feature Control        
	MRESULT MavFeatureCtrl(MUINT32 FeatureID, void* pParaIn, void* pParaOut);	
private:
   

};

#endif

