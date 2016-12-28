

#ifndef _APP_MAV_WARP_H
#define _APP_MAV_WARP_H

#include "MTKWarp.h"

#ifndef SIM_MAIN
#include "EGLUtils.h"
#include <EGL/egl.h>
#endif
    
class AppMavWarp : public MTKWarp {
public:    
    static MTKWarp* getInstance();
    virtual void destroyInstance();
    
    AppMavWarp();
    virtual ~AppMavWarp();   
    // Process Control
    MRESULT WarpInit(MUINT32 *InitInData, MUINT32 *InitOutData);
    MRESULT WarpMain(void);					// START
    MRESULT WarpReset(void);					// RESET

	// Feature Control        
	MRESULT WarpFeatureCtrl(MUINT32 FeatureID, void* pParaIn, void* pParaOut);	
private:
   
#ifndef SIM_MAIN
    EGLDisplay eglDisplay;
    EGLConfig  eglConfig;
    EGLSurface eglSurface;
    EGLContext eglContext;
    EGLint     majorVersion;
    EGLint     minorVersion;
#endif
};

#endif

