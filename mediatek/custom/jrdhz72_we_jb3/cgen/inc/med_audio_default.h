/*********************************************************
History:
2013-0607 modify for YarisM_Proto_Audio Parameters_V0.5 by yi.zheng.hz
2013-0829 modify for PR514519, Panasonic audio parameter
*********************************************************/
#if defined(JRD_DIGICEL_AUDIO_PARAM)
#include "Digicel/med_audio_default.h"

#elif defined(JRD_PANASONIC_AUDIO_PARAM)
#include "Panasonic/med_audio_default.h"

#elif !defined(MTK_2IN1_SPK_SUPPORT)
#include "EU/med_audio_default.h"

#else
#include "US/med_audio_default.h"
#endif

