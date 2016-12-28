

#ifndef _APP_AUTORAMA_H
#define _APP_AUTORAMA_H

#include "MTKAutoramaType.h"
#include "MTKAutorama.h"

class AppAutorama : public MTKAutorama {
public:    
    static MTKAutorama* getInstance();
    virtual void destroyInstance();
    
    AppAutorama();
    virtual ~AppAutorama();   
    // Process Control
    MRESULT AutoramaInit(void *InitInData, void *InitOutData);
    MRESULT AutoramaMain(void);					// START
    MRESULT AutoramaExit(void);					// EXIT

	// Feature Control        
	MRESULT AutoramaFeatureCtrl(MUINT32 FeatureID, void* pParaIn, void* pParaOut);	
private:
    MTKAUTORAMA_STATE_ENUM m_AutoramaState;
    MTKAUTORAMA_CTRL_ENUM m_CtrlEnum;
    MTKAutoramaEnvInfo m_AutoramaEnvInfo;

    MUINT32 m_AutoramaCurrProcNum;
    MUINT32 m_AutoramaCurrImageNum;
    MUINT32 m_ProcessCount;


};

#endif

