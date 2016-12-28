

#ifndef _APP_ASD_H
#define _APP_ASD_H

#include "MTKAsd.h"

class AppAsd : public MTKAsd {
public:    
    static MTKAsd* getInstance();
    virtual void destroyInstance();
    
    AppAsd();
    virtual ~AppAsd();   
    // Process Control
    MRESULT AsdInit(void* InitInData, void* InitOutData);	// Env/Cb setting
    MRESULT AsdMain(ASD_PROC_ENUM ProcId, void* pParaIn);	// START
            
	// Feature Control        
	MRESULT AsdFeatureCtrl(MUINT32 FeatureID, void* pParaIn, void* pParaOut);

private:
   

};

#endif

