/*Update for EU_HD version_Update Headset Parameters 2013-08-22 by yi.zheng.hz*/
#ifndef AUDIO_CUSTOM_H
#define AUDIO_CUSTOM_H

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
#define SPEECH_INPUT_FIR_COEFF \
     2906, -2805,  3029, -2307,  1969,\
     -710, -1099,  2225, -4451,  5953,\
    -7450,  7589,-10504, 11969,-14215,\
    12438,-15779, 10145,-15845, 10684,\
   -12131, 23197, 23197,-12131, 10684,\
   -15845, 10145,-15779, 12438,-14215,\
    11969,-10504,  7589, -7450,  5953,\
    -4451,  2225, -1099,  -710,  1969,\
    -2307,  3029, -2805,  2906,     0,\
                                      \
     -596,  1170,  -530,  1159,  -600,\
      639,  -902,   483, -1044,   620,\
    -1081,  1584,  -902,  2228, -1237,\
     2307, -2942,  1456, -5700,  1412,\
   -12088, 20675, 20675,-12088,  1412,\
    -5700,  1456, -2942,  2307, -1237,\
     2228,  -902,  1584, -1081,   620,\
    -1044,   483,  -902,   639,  -600,\
     1159,  -530,  1170,  -596,     0,\
                                      \
    -3623, -1117, -1005,   290,  -386,\
     1187, -1377,  -961, -4025, -1241,\
    -1527,   948,  -672,  4397,  -209,\
     1209, -3290,    22, -6089,  9277,\
    -3414, 23197, 23197, -3414,  9277,\
    -6089,    22, -3290,  1209,  -209,\
     4397,  -672,   948, -1527, -1241,\
    -4025,  -961, -1377,  1187,  -386,\
      290, -1005, -1117, -3623,     0,\
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
#define SPEECH_OUTPUT_FIR_COEFF \
     1063,   495,   494,    36,   358,\
     -499,  -418,  -725,  -746,  -652,\
    -1699,  -736, -2223,   762, -2813,\
     1520, -1947,  1570, -6312,  7191,\
    -4193, 23197, 23197, -4193,  7191,\
    -6312,  1570, -1947,  1520, -2813,\
      762, -2223,  -736, -1699,  -652,\
     -746,  -725,  -418,  -499,   358,\
       36,   494,   495,  1063,     0,\
                                      \
    -1083,   911,  -924,   654,  -928,\
      659,  -875,  1080,  -496,  1081,\
    -1236,  1688,  -719,  2953,  -396,\
     2295, -2042,  5086, -8004, -1572,\
    -7595, 23197, 23197, -7595, -1572,\
    -8004,  5086, -2042,  2295,  -396,\
     2953,  -719,  1688, -1236,  1081,\
     -496,  1080,  -875,   659,  -928,\
      654,  -924,   911, -1083,     0,\
                                      \
    -2449,  -969, -2316,  1795,   459,\
     6569,  2038,   863, -1502, -3169,\
    -6538,  2159, -2605,  3802,   206,\
    -1732,-14193, -5681,-15607, 17547,\
    -2966, 32767, 32767, -2966, 17547,\
   -15607, -5681,-14193, -1732,   206,\
     3802, -2605,  2159, -6538, -3169,\
    -1502,   863,  2038,  6569,   459,\
     1795, -2316,  -969, -2449,     0,\
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
#define   DEFAULT_BLUETOOTH_SYNC_TYPE    0
#define   DEFAULT_BLUETOOTH_SYNC_LENGTH    1
    /* 0: Input FIR coefficients for 2G/3G Normal mode */
    /* 1: Input FIR coefficients for 2G/3G/VoIP Headset mode */
    /* 2: Input FIR coefficients for 2G/3G Handfree mode */
    /* 3: Input FIR coefficients for 2G/3G/VoIP BT mode */
    /* 4: Input FIR coefficients for VoIP Normal mode */
    /* 5: Input FIR coefficients for VoIP Handfree mode */
#define WB_Speech_Input_FIR_Coeff \
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
       83,   102,    83,   147,   105,   161,   145,   102,   150,    47,\
      115,   -39,    26,  -126,   -58,  -150,  -147,  -127,  -168,   -58,\
     -136,    50,  -102,   275,   -75,   461,    28,   469,   195,   269,\
      296,   -38,   320,  -577,   296, -1281,   267, -2166,   340, -3249,\
      865, -4941,  2669, -9538, 20675, 20675, -9538,  2669, -4941,   865,\
    -3249,   340, -2166,   267, -1281,   296,  -577,   320,   -38,   296,\
      269,   195,   469,    28,   461,   -75,   275,  -102,    50,  -136,\
      -58,  -168,  -127,  -147,  -150,   -58,  -126,    26,   -39,   115,\
       47,   150,   102,   145,   161,   105,   147,    83,   102,    83,\
                                       \
    -1722, -1381, -1231, -1123,  -446,  -654,  -162,    77,   -78,  -103,\
      458,   246,    48,  -432,  -133,  -930,  -874, -1646, -1227, -1070,\
    -1722,   383,  -801,   311,    -1,  -402,  1045,  1879,   369,  1205,\
      961,  -213,  -563, -1192,   903, -2181, -1654, -2178,  2481, -3099,\
     6313,   168,  5357, -7206, 23043, 23043, -7206,  5357,   168,  6313,\
    -3099,  2481, -2178, -1654, -2181,   903, -1192,  -563,  -213,   961,\
     1205,   369,  1879,  1045,  -402,    -1,   311,  -801,   383, -1722,\
    -1070, -1227, -1646,  -874,  -930,  -133,  -432,    48,   246,   458,\
     -103,   -78,    77,  -162,  -654,  -446, -1123, -1231, -1381, -1722,\
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
#define WB_Speech_Output_FIR_Coeff \
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
     -291,   -62,  -136,     6,    95,  -106,   -55,   -93,   -89,  -110,\
       99,  -218,    21,   -20,   -29,   -59,   313,    49,   261,   213,\
     -266,   247,  -299,   446,  -156,   591,   389,   641,   537,  1013,\
      470,   -53,   889, -1059,  1026,  1265,   642, -2226,     2, -8645,\
     2441, -3510,  3504, -7040, 23197, 23197, -7040,  3504, -3510,  2441,\
    -8645,     2, -2226,   642,  1265,  1026, -1059,   889,   -53,   470,\
     1013,   537,   641,   389,   591,  -156,   446,  -299,   247,  -266,\
      213,   261,    49,   313,   -59,   -29,   -20,    21,  -218,    99,\
     -110,   -89,   -93,   -55,  -106,    95,     6,  -136,   -62,  -291,\
                                       \
     -822,  -384,  -562,  -809, -1004, -1125,    31,   361,    98,   355,\
     1328,  2097,  2610,  1886,   623,  -153,   733,  -695,  -617,  -815,\
    -2852, -2161, -2102,  -358,   323,   870,  -970,  -579,  2351,  1374,\
     -313,   394, -4635, -3854, -3241, -7467, -1513, -8502,   636,  -222,\
     5183,  7478,  6057,-14337, 32767, 32767,-14337,  6057,  7478,  5183,\
     -222,   636, -8502, -1513, -7467, -3241, -3854, -4635,   394,  -313,\
     1374,  2351,  -579,  -970,   870,   323,  -358, -2102, -2161, -2852,\
     -815,  -617,  -695,   733,  -153,   623,  1886,  2610,  2097,  1328,\
      355,    98,   361,    31, -1125, -1004,  -809,  -562,  -384,  -822,\
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
