

#ifndef _CAM_CAL_DRV_H_
#define _CAM_CAL_DRV_H_

//seaniln 121017 for 658x #include "camera_custom_nvram.h"
#include "camera_custom_cam_cal.h"
#define DRV_CAM_CAL_SUPPORT (0)

#include "camera_custom_nvram.h"
#ifndef USING_MTK_LDVT
    #define CAM_CAL_SUPPORT
#endif




class CamCalDrvBase {

public:
    /////////////////////////////////////////////////////////////////////////
    //
    // createInstance () -
    //! \brief create instance
    //
    /////////////////////////////////////////////////////////////////////////
    static CamCalDrvBase* createInstance();

    /////////////////////////////////////////////////////////////////////////
    //
    // destroyInstance () -
    //! \brief destroy instance
    //
    /////////////////////////////////////////////////////////////////////////
    virtual void destroyInstance() = 0;

    /////////////////////////////////////////////////////////////////////////
    //
    // readCamCal () -
    //! \brief
    //
    /////////////////////////////////////////////////////////////////////////
	virtual int GetCamCalCalData(unsigned long i4SensorDevId,
                          /*unsigned long u4SensorID,*/
                          CAMERA_CAM_CAL_TYPE_ENUM a_eCamCalDataType,
	                      void *a_pCamCalData) = 0;

protected:
    /////////////////////////////////////////////////////////////////////////
    //
    // ~CamCalDrvBase () -
    //! \brief descontrustor
    //
    /////////////////////////////////////////////////////////////////////////
    virtual ~CamCalDrvBase() {}

private:
	
};





#endif  

