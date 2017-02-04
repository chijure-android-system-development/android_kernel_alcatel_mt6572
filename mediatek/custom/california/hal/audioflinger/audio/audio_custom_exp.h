
#ifndef AUDIO_CUSTOM_EXP_H
#define AUDIO_CUSTOM_EXP_H

#include "Audio_Customization_Common.h"

#undef ENABLE_AUDIO_DRC_SPEAKER //legen: no need to open DRC,as it is ext amp

#define DEVICE_MAX_VOLUME       (9)
#define DEVICE_VOICE_MAX_VOLUME (9)
#define DEVICE_AMP_MAX_VOLUME   (15)
#define DEVICE_MIN_VOLUME       (-5)
#define DEVICE_VOICE_MIN_VOLUME (-5)
#define DEVICE_AMP_MIN_VOLUME   (6)
#define DEVICE_VOLUME_RANGE     (64)
#define DEVICE_VOLUME_STEP      (256)

//#define USING_CLASSD_AMP                // define using which flag
//#define USING_CLASSAB_AMP

#define BOARD_ID_AUDIO_EXT_PA		"PCBA_5"

#define USING_CLASSD_AMP   //please donn't remove this, if choose the internal pa, class d amplifier will be needed

#endif

