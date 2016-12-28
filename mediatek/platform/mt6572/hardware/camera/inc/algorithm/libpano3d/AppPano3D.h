

#ifndef _APP_PANO3D_H
#define _APP_PANO3D_H

#include "MTKPano3DType.h"
#include "MTKPano3D.h"

class AppPano3D : public MTKPano3D {
public:    
    static MTKPano3D* getInstance();
    virtual void destroyInstance();
    
    AppPano3D();
    virtual ~AppPano3D();   
    // Process Control
    MRESULT Pano3DInit(void *InitInData, void *InitOutData);
    MRESULT Pano3DMain(void);                   // START
    MRESULT Pano3DReset(void);                  // RESET

    // Feature Control        
    MRESULT Pano3DFeatureCtrl(MUINT32 FeatureID, void* pParaIn, void* pParaOut);    
private:
   

};

#endif

