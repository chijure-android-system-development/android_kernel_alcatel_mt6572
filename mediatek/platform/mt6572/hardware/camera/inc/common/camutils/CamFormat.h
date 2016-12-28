
#ifndef _MTK_CAMERA_INC_COMMON_CAMUTILS_CAMFORMAT_H_
#define _MTK_CAMERA_INC_COMMON_CAMUTILS_CAMFORMAT_H_
//


namespace android {
namespace MtkCamUtils {
namespace FmtUtils {


uint_t
queryImageioFormat(
    char const* szPixelFormat
);


int
queryBitsPerPixel(
    char const* szPixelFormat
);


size_t
queryPlaneCount(
    char const* szPixelFormat
);


size_t
queryImgWidthStride(
    char const* szPixelFormat, 
    size_t imgWidth, 
    size_t planeIndex
);


size_t
queryImgHeightStride(
    char const* szPixelFormat, 
    size_t imgHeight, 
    size_t planeIndex
);


size_t
queryImgBufferSize(
    char const* szPixelFormat, 
    size_t imgWidth, 
    size_t imgHeight
);


};  // namespace FmtUtils
};  // namespace MtkCamUtils
};  // namespace android
#endif  //_MTK_CAMERA_INC_COMMON_CAMUTILS_CAMFORMAT_H_

