#ifndef AUDIO_CUSTOM1_H
#define AUDIO_CUSTOM1_H

/* define Gain For Normal */
/* Normal volume: TON, SPK, MIC, FMR, SPH, SID, MED */
/*
#define GAIN_NOR_TON_VOL        8     // reserved
#define GAIN_NOR_KEY_VOL       43    // TTY_CTM_Mic
#define GAIN_NOR_MIC_VOL       26    // IN_CALL BuiltIn Mic gain
// GAIN_NOR_FMR_VOL is used as idle mode record volume
#define GAIN_NOR_FMR_VOL        0     // Normal BuiltIn Mic gain
#define GAIN_NOR_SPH_VOL       20     // IN_CALL EARPIECE Volume
#define GAIN_NOR_SID_VOL      100  // IN_CALL EARPICE sidetone
#define GAIN_NOR_MED_VOL       25   // reserved
*/

#define GAIN_NOR_TON_VOL        8     // reserved
#define GAIN_NOR_KEY_VOL       43    // TTY_CTM_Mic
#define GAIN_NOR_MIC_VOL       26    // IN_CALL BuiltIn Mic gain
// GAIN_NOR_FMR_VOL is used as idle mode record volume
#define GAIN_NOR_FMR_VOL        0     // Normal BuiltIn Mic gain
#define GAIN_NOR_SPH_VOL       20     // IN_CALL EARPIECE Volume
#define GAIN_NOR_SID_VOL      100  // IN_CALL EARPICE sidetone
#define GAIN_NOR_MED_VOL       25   // reserved

/* define Gain For Headset */
/* Headset volume: TON, SPK, MIC, FMR, SPH, SID, MED */
/*
#define GAIN_HED_TON_VOL        8     // reserved
#define GAIN_HED_KEY_VOL       24    // reserved
#define GAIN_HED_MIC_VOL       20    // IN_CALL BuiltIn headset gain
#define GAIN_HED_FMR_VOL       24     // reserved
#define GAIN_HED_SPH_VOL       12     // IN_CALL Headset volume
#define GAIN_HED_SID_VOL      100  // IN_CALL Headset sidetone
#define GAIN_HED_MED_VOL       12   // Idle, headset Audio Buf Gain setting
*/

#define GAIN_HED_TON_VOL        8     // reserved
#define GAIN_HED_KEY_VOL       24    // reserved
#define GAIN_HED_MIC_VOL       20    // IN_CALL BuiltIn headset gain
#define GAIN_HED_FMR_VOL       24     // reserved
#define GAIN_HED_SPH_VOL       12     // IN_CALL Headset volume
#define GAIN_HED_SID_VOL      100  // IN_CALL Headset sidetone
#define GAIN_HED_MED_VOL       12   // Idle, headset Audio Buf Gain setting

/* define Gain For Handfree */
/* Handfree volume: TON, SPK, MIC, FMR, SPH, SID, MED */
/* GAIN_HND_TON_VOL is used as class-D Amp gain*/
/*
#define GAIN_HND_TON_VOL       15     // use for ringtone volume
#define GAIN_HND_KEY_VOL       24    // reserved
#define GAIN_HND_MIC_VOL       20    // IN_CALL LoudSpeak Mic Gain = BuiltIn Gain
#define GAIN_HND_FMR_VOL       24     // reserved
#define GAIN_HND_SPH_VOL        6     // IN_CALL LoudSpeak
#define GAIN_HND_SID_VOL      100  // IN_CALL LoudSpeak sidetone
#define GAIN_HND_MED_VOL       12   // Idle, loudSPK Audio Buf Gain setting
*/

#define GAIN_HND_TON_VOL       15     // use for ringtone volume
#define GAIN_HND_KEY_VOL       24    // reserved
#define GAIN_HND_MIC_VOL       20    // IN_CALL LoudSpeak Mic Gain = BuiltIn Gain
#define GAIN_HND_FMR_VOL       24     // reserved
#define GAIN_HND_SPH_VOL        6     // IN_CALL LoudSpeak
#define GAIN_HND_SID_VOL      100  // IN_CALL LoudSpeak sidetone
#define GAIN_HND_MED_VOL       12   // Idle, loudSPK Audio Buf Gain setting
    /* 0: Input FIR coefficients for 2G/3G Normal mode */
    /* 1: Input FIR coefficients for 2G/3G/VoIP Headset mode */
    /* 2: Input FIR coefficients for 2G/3G Handfree mode */
    /* 3: Input FIR coefficients for 2G/3G/VoIP BT mode */
    /* 4: Input FIR coefficients for VoIP Normal mode */
    /* 5: Input FIR coefficients for VoIP Handfree mode */
#define SPEECH_INPUT_FIR_COEFF1 \
    -1403,  2004, -1687,   734,  -358,\
    -1911,  1741, -3963,  3780, -4734,\
     3992, -4267,   546,   557, -4395,\
     6635,-11404,  9649,-14750, 13025,\
   -15730, 23197, 23197,-15730, 13025,\
   -14750,  9649,-11404,  6635, -4395,\
      557,   546, -4267,  3992, -4734,\
     3780, -3963,  1741, -1911,  -358,\
      734, -1687,  2004, -1403,     0,\
                                      \
    32767,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
                                      \
    32767,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
                                      \
    32767,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
                                      \
    32767,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
                                      \
    32767,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0
    /* 0: Output FIR coefficients for 2G/3G Normal mode */
    /* 1: Output FIR coefficients for 2G/3G/VoIP Headset mode */
    /* 2: Output FIR coefficients for 2G/3G Handfree mode */
    /* 3: Output FIR coefficients for 2G/3G/VoIP BT mode */
    /* 4: Output FIR coefficients for VoIP Normal mode */
    /* 5: Output FIR coefficients for VoIP Handfree mode */
#define SPEECH_OUTPUT_FIR_COEFF1 \
    32767,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
                                      \
    32767,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
                                      \
    32767,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
                                      \
    32767,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
                                      \
    32767,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
                                      \
    32767,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0
#define   DG_DL_Speech    0xe3d
#define   DG_Microphone    0x1400
#define   FM_Record_Vol    6     /* 0 is smallest. each step increase 1dB.
                            Be careful of distortion when increase too much.
                            Generally, it's not suggested to tune this parameter */
/*
* The Bluetooth DAI Hardware COnfiguration Parameter
*/
#define   DEFAULT_BLUETOOTH_SYNC_TYPE1    0
#define   DEFAULT_BLUETOOTH_SYNC_LENGTH1    1
    /* 0: Input FIR coefficients for 2G/3G Normal mode */
    /* 1: Input FIR coefficients for 2G/3G/VoIP Headset mode */
    /* 2: Input FIR coefficients for 2G/3G Handfree mode */
    /* 3: Input FIR coefficients for 2G/3G/VoIP BT mode */
    /* 4: Input FIR coefficients for VoIP Normal mode */
    /* 5: Input FIR coefficients for VoIP Handfree mode */
#define WB_Speech_Input_FIR_Coeff1 \
     -510,   -57,  -157,  -131,  -113,  -468,  -365,  -332,  -185,  -478,\
     -112,  -384,  -358,  -196,     9,  -251,  -265,  -709,   -19,   245,\
     -413,   276,  -623,   455,  -679,    72,   296,   244,  -174,  -765,\
      512,   424,  1858, -1835, -1776,  1230,  1345, -2249,   678,  -560,\
     5331,  -956,  -419, -9316, 26028, 26028, -9316,  -419,  -956,  5331,\
     -560,   678, -2249,  1345,  1230, -1776, -1835,  1858,   424,   512,\
     -765,  -174,   244,   296,    72,  -679,   455,  -623,   276,  -413,\
      245,   -19,  -709,  -265,  -251,     9,  -196,  -358,  -384,  -112,\
     -478,  -185,  -332,  -365,  -468,  -113,  -131,  -157,   -57,  -510,\
                                       \
    32767,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
                                       \
    32767,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
                                       \
    32767,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
                                       \
    32767,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
                                       \
    32767,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0
    /* 0: Output FIR coefficients for 2G/3G Normal mode */
    /* 1: Output FIR coefficients for 2G/3G/VoIP Headset mode */
    /* 2: Output FIR coefficients for 2G/3G Handfree mode */
    /* 3: Output FIR coefficients for 2G/3G/VoIP BT mode */
    /* 4: Output FIR coefficients for VoIP Normal mode */
    /* 5: Output FIR coefficients for VoIP Handfree mode */
#define WB_Speech_Output_FIR_Coeff1 \
      -39,  -135,  -200,  -136,   -92,  -197,   -41,    79,  -318,   -85,\
     -180,  -134,  -124,  -214,  -186,  -148,    40,  -238,    11,  -123,\
      -91,  -218,   297,  -254,  -122,  -115,    65,   267,  -306,   189,\
    -1039,  1887,  -529,  -309,   938,   208,    49, -2360,  1935, -2170,\
     5090, -4521,  7814,-10122, 23197, 23197,-10122,  7814, -4521,  5090,\
    -2170,  1935, -2360,    49,   208,   938,  -309,  -529,  1887, -1039,\
      189,  -306,   267,    65,  -115,  -122,  -254,   297,  -218,   -91,\
     -123,    11,  -238,    40,  -148,  -186,  -214,  -124,  -134,  -180,\
      -85,  -318,    79,   -41,  -197,   -92,  -136,  -200,  -135,   -39,\
                                       \
    32767,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
                                       \
    32767,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
                                       \
    32767,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
                                       \
    32767,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
                                       \
    32767,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0
#endif
