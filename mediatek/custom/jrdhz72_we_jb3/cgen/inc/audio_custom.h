/*********************************************************
History:
2013-0607 modify for YarisM_Proto_Audio Parameters_V0.5 by yi.zheng.hz
2013-0829 modify for PR514519, Panasonic audio parameter
*********************************************************/
#if defined(JRD_DIGICEL_AUDIO_PARAM)
#include "Digicel/audio_custom.h"

#elif defined(JRD_PANASONIC_AUDIO_PARAM)
#include "Panasonic/audio_custom.h"

#elif !defined(MTK_2IN1_SPK_SUPPORT)
#include "EU/audio_custom.h"

#else
#include "US/audio_custom.h"
#endif

