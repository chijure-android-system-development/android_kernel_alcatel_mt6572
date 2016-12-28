
#ifndef _MTK_CAMERA_INC_CAMSHOT_CALLBACKS_H_
#define _MTK_CAMERA_INC_CAMSHOT_CALLBACKS_H_


namespace NSCamShot {
////////////////////////////////////////////////////////////////////////////////


struct CamShotNotifyInfo
{
    /**
      * @var msgType 
      * The notify message type of the camshot 
      */ 
    MUINT32     msgType;    
    /**
      * @var ext1 
      * The extended parameter 1.
      */     
    MUINT32     ext1;      
    /**
      * @var ext2 
      * The extended parameter 2.
      */     
    MUINT32     ext2;       
    //
    CamShotNotifyInfo(
        MUINT32 const _msgType = 0, 
        MUINT32 const _ext1 = 0, 
        MUINT32 const _ext2 = 0
    )
        : msgType(_msgType)
        , ext1(_ext1)
        , ext2(_ext2)
    {}
};

typedef MBOOL   (*CamShotNotifyCallback_t)(MVOID* user, CamShotNotifyInfo const msg);


struct CamShotDataInfo
{
    /**
      * @var msgType 
      * The data message type of the camshot 
      */     
    MUINT32     msgType;    
    /**
      * @var ext1 
      * The extended parameter 1.
      */     
    MUINT32     ext1;       
    /**
      * @var ext2 
      * The extended parameter 2.
      */      
    MUINT32     ext2;       
    /**
      * @var puData 
      * Pointer to the callback data.
      */      
    MUINT8*     puData;    
    /**
      * @var u4Size 
      * Size of the callback data.
      */     
    MUINT32     u4Size;     
    //
    CamShotDataInfo(
        MUINT32 const _msgType = 0, 
        MUINT32 const _ext1 = 0, 
        MUINT32 const _ext2 = 0, 
        MUINT8* const _puData = NULL, 
        MUINT32 const _u4Size = 0
    )
        : msgType(_msgType)
        , ext1(_ext1)
        , ext2(_ext2)
        , puData(_puData)
        , u4Size(_u4Size)
    {
    }
};

typedef MBOOL   (*CamShotDataCallback_t)(MVOID* user, CamShotDataInfo const msg);


struct SImagerNotifyInfo
{
    /**
      * @var msgType 
      * The data message type of the SImager 
      */  
    MUINT32     msgType;  
    /**
      * @var ext1 
      * The extended parameter 1.
      */      
    MUINT32     ext1;     
    /**
      * @var ext2 
      * The extended parameter 2.
      */          
    MUINT32     ext2;    
    //
    SImagerNotifyInfo(
        MUINT32 const _msgType = 0, 
        MUINT32 const _ext1 = 0, 
        MUINT32 const _ext2 = 0
    )
        : msgType(_msgType)
        , ext1(_ext1)
        , ext2(_ext2)
    {}
};


typedef MBOOL   (*SImagerNotifyCallback_t)(MVOID* user, SImagerNotifyInfo const msg);

////////////////////////////////////////////////////////////////////////////////
};  //namespace NSCamShot
#endif  //  _MTK_CAMERA_INC_CAMSHOT_CALLBACKS_H_

