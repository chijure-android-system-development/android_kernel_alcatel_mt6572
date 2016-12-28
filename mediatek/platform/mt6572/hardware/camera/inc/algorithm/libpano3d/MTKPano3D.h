

#ifndef _MTK_PANO3D_H
#define _MTK_PANO3D_H

#include "MTKPano3DType.h"
#include "MTKPano3DErrCode.h"
typedef enum DRVPano3DObject_s {
    DRV_PANO3D_OBJ_NONE = 0,
    DRV_PANO3D_OBJ_SW,
    DRV_PANO3D_OBJ_GPU,    
    DRV_PANO3D_OBJ_UNKNOWN = 0xFF,
} DrvPano3DObject_e;

#define PANO3D_IM_WIDTH             (800)
#define PANO3D_IM_HEIGHT            (600)
#define PANO3D_CLIP_WIDTH           (1920)
#define PANO3D_CLIP_HEIGHT          (1080)
#define PANO3D_MAX_IMG_NUM          (25)
#define PHOTO_OPT_SWITCH            (1)
#define PANO3D_BUFFER_UNSET         (0xFFFF)

typedef enum
{
    PANO3D_STATE_STANDBY=0, // After Create Obj or Reset
    PANO3D_STATE_INIT,          // After Called Pano3DInit
    PANO3D_STATE_PROC,          // After Called Pano3DMain
    PANO3D_STATE_PROC_READY,    // After Finish Pano3DMain  
} PANO3D_STATE_ENUM;

// PANO3DInit, 
// Input    : Pano3DTuningInfo
// Output   : NONE
struct Pano3DTuningInfo
{
    MUINT16                 SmoothThre;
};

typedef enum
{
    PANO3D_IMAGE_YV12=0,          // input image format is YV12
    PANO3D_IMAGE_NV21,            // input image format is NV21
    PANO3D_IMAGE_RGB565,          // input image format is RGB565
    PANO3D_IMAGE_MAX              // maximum image format enum
}   PANO3D_IMAGE_FORMAT;

typedef enum
{
    PANO3D_SMALL_OVLP_WIDTH=0,    // error of small overlap width
    PANO3D_LARGE_SEAM_CHANGE,     // error of large seam change
    PANO3D_ERR_NO
} PANO3D_ERR_ENUM;

typedef enum
{
    PANO3D_FEATURE_BEGIN,
    PANO3D_FEATURE_SET_ENV_INFO,
    PANO3D_FEATURE_GET_RESULT,
    PANO3D_FEATURE_GET_LOG,
    PANO3D_FEATURE_GET_WORKBUF_SIZE,
    PANO3D_FEATURE_SET_WORKBUF_ADDR,
    PANO3D_FEATURE_MAX
}   PANO3D_FEATURE_ENUM;

// PANO3D_FEATURE_SET_ENV_INFO, 
// Input    : Pano3DImageInfo
// Output   : NONE
struct Pano3DImageInfo
{
    MUINT32                 ImgAddr;
    MUINT16                 ImgWidth;
    MUINT16                 ImgHeight;
    MUINT32                 ImgNum;
    PANO3D_IMAGE_FORMAT     ImgFmt;
    MUINT32                 WorkingBuffAddr;    
    MFLOAT                  InvHmtx[PANO3D_MAX_IMG_NUM][9];

    // for 3D Panorama
    // Added by CM Cheng, 2011-06-09
    MINT32                  ClipY;                                  // image offset Y - the same for all images
    MUINT16                 ClipHeight;                             // image cropped height - the same for all images
    MINT16                  GridX[PANO3D_MAX_IMG_NUM];              // vertical offset in panorama space
    MINT16                  MinX[PANO3D_MAX_IMG_NUM];               // x_start in panorama space
    MINT16                  MaxX[PANO3D_MAX_IMG_NUM];               // x_end in panorama space

};

// PANO3D_FEATURE_GET_RESULT
// Input    : NONE
// Output   : Pano3DResultInfo
struct Pano3DResultInfo
{
    MUINT16                 PanoWidth;
    MUINT16                 PanoHeight;
    MUINT32                 LeftPanoImageAddr;
    MUINT32                 RightPanoImageAddr;
    MRESULT                 RetCode;
    MUINT8                  ErrPattern;         // Returned error/Warning bit pattern
                                                // bit 0: small overlap width (set if error)
                                                // bit 1: large seam change (set if error)

    // 3D picture cropping info
    MUINT32                 ClipX;              // horizontal offset of ROI
    MUINT32                 ClipY;              // vertical offset of ROI
    MUINT32                 ClipWidth;          // width of ROI
    MUINT32                 ClipHeight;         // height of ROI

    // Optimal Seam - debug purpose
    MINT32                 OptimalSeamLeft[PANO3D_MAX_IMG_NUM][PANO3D_IM_HEIGHT];
};

class MTKPano3D {
public:
    static MTKPano3D* createInstance(DrvPano3DObject_e eobject);
    virtual void   destroyInstance() = 0;
       
    virtual ~MTKPano3D();
    // Process Control
    virtual MRESULT Pano3DInit(void *InitInData, void *InitOutData);    // Env/Cb setting
    virtual MRESULT Pano3DMain(void);                   // START
    virtual MRESULT Pano3DReset(void);                  // RESET
            
    // Feature Control        
    virtual MRESULT Pano3DFeatureCtrl(MUINT32 FeatureID, void* pParaIn, void* pParaOut);
private:
    
};

class AppPano3DTmp : public MTKPano3D {
public:
    //
    static MTKPano3D* getInstance();
    virtual void destroyInstance();
    //
    AppPano3DTmp() {}; 
    virtual ~AppPano3DTmp() {};
};

#endif

