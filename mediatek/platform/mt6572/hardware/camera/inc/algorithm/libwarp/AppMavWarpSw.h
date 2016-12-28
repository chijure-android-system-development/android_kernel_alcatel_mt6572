

#ifndef _APP_MAV_WARP_SW_H
#define _APP_MAV_WARP_SW_H

#include "MTKWarp.h"

class AppMavWarpSw : public MTKWarp {
public:    
    static MTKWarp* getInstance();
    virtual void destroyInstance();
    
    AppMavWarpSw();
    virtual ~AppMavWarpSw();   
    // Process Control
    MRESULT WarpInit(MUINT32 *InitInData, MUINT32 *InitOutData);
    MRESULT WarpMain(void);					// START
    MRESULT WarpReset(void);					// RESET

	// Feature Control        
	MRESULT WarpFeatureCtrl(MUINT32 FeatureID, void* pParaIn, void* pParaOut);	
private:
   

};

#endif

