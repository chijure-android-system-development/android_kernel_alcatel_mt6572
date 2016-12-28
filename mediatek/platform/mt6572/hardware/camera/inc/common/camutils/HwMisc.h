
#ifndef _MTK_CAMERA_INC_COMMON_CAMUTILS_HWMISC_H_
#define _MTK_CAMERA_INC_COMMON_CAMUTILS_HWMISC_H_
//

#include <common/CamTypes.h>
#include <common/hw/hwstddef.h>



namespace android {
namespace MtkCamUtils {


NSCamHW::Rect
calCrop(
    NSCamHW::Rect const &rSrc, 
    NSCamHW::Rect const &rDst, 
    uint32_t ratio
); 


};  // namespace MtkCamUtils
};  // namespace android
#endif  //_MTK_CAMERA_INC_COMMON_CAMUTILS_HWMISC_H_

