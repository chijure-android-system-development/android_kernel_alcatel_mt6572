

#ifndef _APP_PANO_WARP_SW_H
#define _APP_PANO_WARP_SW_H

#include "MTKWarp.h"

class AppPanoWarpSw : public MTKWarp {
public:    
    static MTKWarp* getInstance();
    virtual void destroyInstance();
    
    AppPanoWarpSw();
    virtual ~AppPanoWarpSw();   
    // Process Control
    MRESULT WarpInit(MUINT32 *InitInData, MUINT32 *InitOutData);
    MRESULT WarpMain(void);                 // START
    MRESULT WarpReset(void);                    // RESET

    // Feature Control        
    MRESULT WarpFeatureCtrl(MUINT32 FeatureID, void* pParaIn, void* pParaOut);  
private:
   

};

#endif

