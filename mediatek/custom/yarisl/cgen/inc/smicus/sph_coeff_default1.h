#ifndef SPEECH_COEFF_DEFAULT1_H
#define SPEECH_COEFF_DEFAULT1_H

#ifndef FALSE
#define FALSE 0
#endif

//speech parameter depen on BT_CHIP cersion
#if defined(MTK_MT6611)

#define BT_COMP_FILTER (1 << 15)
#define BT_SYNC_DELAY  86

#elif defined(MTK_MT6612)

#define BT_COMP_FILTER (1 << 15)
#define BT_SYNC_DELAY  86

#elif defined(MTK_MT6616) || defined(MTK_MT6620) || defined(MTK_MT6622) || defined(MTK_MT6626) || defined(MTK_MT6628)

#define BT_COMP_FILTER (1 << 15)
#define BT_SYNC_DELAY  86

#else // MTK_MT6620

#define BT_COMP_FILTER (0 << 15)
#define BT_SYNC_DELAY  86

#endif

#ifdef MTK_DUAL_MIC_SUPPORT
#define SPEECH_MODE_PARA13 (371)
#define SPEECH_MODE_PARA14 (23)
#else
#define SPEECH_MODE_PARA13 (0)
#define SPEECH_MODE_PARA14 (0)
#endif

#define DEFAULT_SPEECH_NORMAL_MODE_PARA1 \
    96,   253, 16388,    31, 57351,   799,   400,    64,\
    80,  4325,   611,     0, 20488,     0,     0,  8192

#define DEFAULT_SPEECH_EARPHONE_MODE_PARA1 \
     0,   189, 10756,    31, 57351,    31,   400,    64,\
    80,  4325,   611,     0, 20488,     0,     0,     0

#define DEFAULT_SPEECH_BT_EARPHONE_MODE_PARA1 \
     0,   253, 10756,    31, 53255,    31,   400,     0,\
    80,  4325,   611,     0, 20488|BT_COMP_FILTER,     0,     0,BT_SYNC_DELAY

#define DEFAULT_SPEECH_LOUDSPK_MODE_PARA1 \
    96,   224,  5256,    31, 57351, 24607,   400,   132,\
    84,  4325,   611,     0, 20488,     0,     0,     0

#define DEFAULT_SPEECH_CARKIT_MODE_PARA1 \
    96,   224,  5256,    31, 57351, 24607,   400,   132,\
    84,  4325,   611,     0, 20488,     0,     0,     0

#define DEFAULT_SPEECH_BT_CORDLESS_MODE_PARA1 \
     0,     0,     0,     0,     0,     0,     0,     0,\
     0,     0,     0,     0,     0,     0,     0,     0

#define DEFAULT_SPEECH_AUX1_MODE_PARA1 \
     0,     0,     0,     0,     0,     0,     0,     0,\
     0,     0,     0,     0,     0,     0,     0,     0

#define DEFAULT_SPEECH_AUX2_MODE_PARA1 \
     0,     0,     0,     0,     0,     0,     0,     0,\
     0,     0,     0,     0,     0,     0,     0,     0

#define DEFAULT_SPEECH_COMMON_PARA1 \
     0, 55997, 31000, 10752, 32769,     0,     0,     0, \
     0,     0,     0,     0

#define DEFAULT_SPEECH_VOL_PARA1 \
     0,     0,     0,     0

#define DEFAULT_AUDIO_DEBUG_INFO1 \
     0,     0,     0,     0,     0,     0,     0,     0, \
     0,     0,     0,     0,     0,     0,     0,     0

#define DEFAULT_VM_SUPPORT FALSE

#define DEFAULT_AUTO_VM FALSE

#define DEFAULT_WB_SPEECH_NORMAL_MODE_PARA1 \
    96,   253, 16388,    31, 57607,   799,   400,    32,\
    80,  4325,   611,     0, 16392,     0,     0,  8192

#define DEFAULT_WB_SPEECH_EARPHONE_MODE_PARA1 \
     0,   189, 10756,    31, 57607,    31,   400,    64,\
    80,  4325,   611,     0, 16392,     0,     0,     0

#define DEFAULT_WB_SPEECH_BT_EARPHONE_MODE_PARA1 \
     0,   253, 10756,    31, 53511,    31,   400,     0,\
    80,  4325,   611,     0, 16392|BT_COMP_FILTER,     0,     0,BT_SYNC_DELAY

#define DEFAULT_WB_SPEECH_LOUDSPK_MODE_PARA1 \
    96,   224,  5256,    31, 57607, 24607,   400,   132,\
    84,  4325,   611,     0, 16392,     0,     0,     0

#define DEFAULT_WB_SPEECH_CARKIT_MODE_PARA1 \
 65271, 64827, 65517,   245, 65123,   276, 64913,   455,\
 64857,    72,   296,   244, 65362, 64771,   512,   424

#define DEFAULT_WB_SPEECH_BT_CORDLESS_MODE_PARA1 \
 65026, 65479, 65379, 65405, 65423, 65068, 65171, 65204,\
 65351, 65058, 65424, 65152, 65178, 65340,     9, 65285

#define DEFAULT_WB_SPEECH_AUX1_MODE_PARA1 \
  1858, 63701, 63760,  1230,  1345, 63287,   678, 64976,\
  5331, 64580, 65117, 56220, 26028, 26028, 56220, 65117

#define DEFAULT_WB_SPEECH_AUX2_MODE_PARA1 \
 64580,  5331, 64976,   678, 63287,  1345,  1230, 63760,\
 63701,  1858,   424,   512, 64771, 65362,   244,   296

#define MICBAIS  1900

/* The Bluetooth PCM digital volume */
/* default_bt_pcm_in_vol : uplink, only for enlarge volume,
                       0x100 : 0dB  gain
                       0x200 : 6dB  gain
                       0x300 : 9dB  gain
                       0x400 : 12dB gain
                       0x800 : 18dB gain
                       0xF00 : 24dB gain             */

#define DEFAULT_BT_PCM_IN_VOL1  0x100
/* default_bt_pcm_out_vol : downlink gain,
                       0x1000 : 0dB; maximum 0x7FFF  */
#define DEFAULT_BT_PCM_OUT_VOL1  0x1000

#endif
