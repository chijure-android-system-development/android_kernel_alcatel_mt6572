

#ifndef _APP_PANO_WARP_H
#define _APP_PANO_WARP_H

#include "MTKWarp.h"
#ifndef SIM_MAIN
#include "EGLUtils.h"
#include <EGL/egl.h>
#endif

class AppPanoWarp : public MTKWarp {
public:    
    static MTKWarp* getInstance();
    virtual void destroyInstance();
    
    AppPanoWarp();
    virtual ~AppPanoWarp();   
    // Process Control
    MRESULT WarpInit(MUINT32 *InitInData, MUINT32 *InitOutData);
    MRESULT WarpMain(void);                 // START
    MRESULT WarpReset(void);                    // RESET

    // Feature Control        
    MRESULT WarpFeatureCtrl(MUINT32 FeatureID, void* pParaIn, void* pParaOut);  
private:
#ifndef SIM_MAIN
    // for egl creation
    EGLDisplay eglDisplay;
    EGLConfig  eglConfig;
    EGLSurface eglSurface;
    EGLContext eglContext;
    EGLint     majorVersion;
    EGLint     minorVersion;
#endif

};

#endif

