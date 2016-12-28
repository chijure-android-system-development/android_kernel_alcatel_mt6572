




#ifndef _CFG_AUDIO_JRD_CUST_H
#define _CFG_AUDIO_JRD_CUST_H

#include "CFG_Audio_Default_Cust.h"
#include "../cfgfileinc/CFG_AUDIO_File.h"
#include "../inc/sph_coeff_record_mode_default.h"
#include "../inc/sph_coeff_dmnr_default.h"
#include "../inc/audio_hd_record_custom.h"
#include "../inc/audio_acf_default.h"
#include "../inc/audio_hcf_default.h"
#include "../inc/audio_effect_default.h"
#include "../inc/audio_gain_table_default.h"
#include "../inc/audio_ver1_volume_custom_default.h"
#include "../inc/audio_hd_record_48k_custom.h"
#include "../inc/voice_recognition_custom.h"
#include "../inc/audio_audenh_control_option.h"


/*add for US_3in1 version_Update All Modes Uplink Volume PR500584 by yi.zheng.hz*/
#define VER1_AUD_VOLUME_MIC_LATAM \
    64,112,184,168,184,200,160,184,184,184,184,0,0,0,0,\
    255,192,184,188,184,208,172,184,184,184,184,0,0,0,0,\
    255,208,208,200,255,208,172,0,0,0,0,0,0,0,0,\
    255,208,208,164,255,208,172,0,0,0,0,0,0,0,0

/*add for US_3in1 version_Update All Modes Uplink Volume PR500584 by yi.zheng.hz*/
#define AUD_VOLUME_MIC_LATAM \
64, 112, 184, 168, 184, 200, 160,     \
255, 192, 184, 188, 184, 208, 172,     \
255, 208, 208, 200, 255, 208, 172

AUDIO_VER1_CUSTOM_VOLUME_STRUCT audio_ver1_custom_default_latam = {
    VER1_AUD_VOLUME_RING,
    VER1_AUD_VOLUME_SIP,
    VER1_AUD_VOLUME_MIC_LATAM,
    VER1_AUD_VOLUME_FM,
    VER1_AUD_VOLUME_SPH,
    VER1_AUD_VOLUME_SPH, // sph2 now use the same
    VER1_AUD_VOLUME_SID,
    VER1_AUD_VOLUME_MEDIA,
    VER1_AUD_VOLUME_MATV,
    VER1_AUD_NORMAL_VOLUME_DEFAULT,
    VER1_AUD_HEADSER_VOLUME_DEFAULT,
    VER1_AUD_SPEAKER_VOLUME_DEFAULT,
    VER1_AUD_HEADSETSPEAKER_VOLUME_DEFAULT,
    VER1_AUD_EXTAMP_VOLUME_DEFAULT,
    VER1_AUD_VOLUME_LEVEL_DEFAULT
};

AUDIO_VOLUME_CUSTOM_STRUCT audio_volume_custom_default_latam =
{
    AUD_VOLUME_RING,
    AUD_VOLUME_KEY,
    AUD_VOLUME_MIC_LATAM,
    AUD_VOLUME_FMR,
    AUD_VOLUME_SPH,
    AUD_VOLUME_SID,
    AUD_VOLUME_MEDIA,
    AUD_VOLUME_MATV
};

#endif


