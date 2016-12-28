
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
     -183,    31,  1140,   348,  -360,\
    -1232,  -918,  -243,  -167,   316,\
     -812,  -537, -2004,   366,  -663,\
     2107, -2194,   492, -5684,  2515,\
    -9442, 20675, 20675, -9442,  2515,\
    -5684,   492, -2194,  2107,  -663,\
      366, -2004,  -537,  -812,   316,\
     -167,  -243,  -918, -1232,  -360,\
      348,  1140,    31,  -183,     0,\
                                      \
       89,  -168,   263,  -205,   116,\
       -6,   213,   263,  -214,   -40,\
     -896,   178,  -600,  1550,  -317,\
     1871, -2060,  1426, -4164,  3966,\
    -7011, 23197, 23197, -7011,  3966,\
    -4164,  1426, -2060,  1871,  -317,\
     1550,  -600,   178,  -896,   -40,\
     -214,   263,   213,    -6,   116,\
     -205,   263,  -168,    89,     0,\
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
      186,  -320,   136,  -368,   -37,\
     -323,  -414,  -312,  -520,   -32,\
     -714,   -50, -1401,  1091, -2151,\
     1203, -1178,  1498, -5749,  6499,\
    -6838, 23197, 23197, -6838,  6499,\
    -5749,  1498, -1178,  1203, -2151,\
     1091, -1401,   -50,  -714,   -32,\
     -520,  -312,  -414,  -323,   -37,\
     -368,   136,  -320,   186,     0,\
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
     -343,  -192,   345,    78,  1135,\
      523,   709,  1133,   -29,   414,\
     -487,   331, -1330,  -531, -1701,\
     -564, -3058,  1352, -3430,  5550,\
    -6022, 23197, 23197, -6022,  5550,\
    -3430,  1352, -3058,  -564, -1701,\
     -531, -1330,   331,  -487,   414,\
      -29,  1133,   709,   523,  1135,\
       78,   345,  -192,  -343,     0,\
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
     -169,    30,   -48,   -11,    69,  -121,  -185,  -158,   -96,  -179,\
      -23,  -162,   -50,  -152,    -7,  -305,  -296,  -377,  -251,  -181,\
     -261,   422,  -308,  -296,  -730,    12,  -392,  -410,  -330,    86,\
     -619,  -460,  1099,  -339, -1068, -2298,  1723,  -416,   918, -3591,\
     2494, -2488,  4574, -8650, 23197, 23197, -8650,  4574, -2488,  2494,\
    -3591,   918,  -416,  1723, -2298, -1068,  -339,  1099,  -460,  -619,\
       86,  -330,  -410,  -392,    12,  -730,  -296,  -308,   422,  -261,\
     -181,  -251,  -377,  -296,  -305,    -7,  -152,   -50,  -162,   -23,\
     -179,   -96,  -158,  -185,  -121,    69,   -11,   -48,    30,  -169,\
                                       \
     -264,  -293,  -317,    -9,   143,   174,   416,   408,   221,  -179,\
     -189,  -522,  -502,  -508,  -303,  -289,  -102,  -128,   235,   189,\
     -239,   317,  -111,   -88, -1153,  -458,  -971,  -919,  -831,  -256,\
      243,   227,   916,  -352,   525,  -685,  -537, -3173,   582, -4427,\
     2658, -5835,  3225, -5039, 23197, 23197, -5039,  3225, -5835,  2658,\
    -4427,   582, -3173,  -537,  -685,   525,  -352,   916,   227,   243,\
     -256,  -831,  -919,  -971,  -458, -1153,   -88,  -111,   317,  -239,\
      189,   235,  -128,  -102,  -289,  -303,  -508,  -502,  -522,  -189,\
     -179,   221,   408,   416,   174,   143,    -9,  -317,  -293,  -264,\
                                       \
     -2073,  -983,  -997,  -1282,  -597,  -1325,  -936,  -760,  -1105,  -1375,\
     -1199,  -1450,  -1003,  -1145,  -851,  -1426,  -275,  -1272,  -812,  -806,\
     -1886,  432,  -1609,  -173,  -552,  -1079,  456,  109,  -871,    915,\
       239,   406,    682, -775,   630, -526,   1076, -1551,   2326, -3407,\
     4019, -1889,  3482, -4660, 23197, 23197, -4660,  3482, -1889,  4019,\
    -3407,   2326, -1551,   1076, -526,   630, -775,    682,   406,    239,\
       915,  -871,  109,  456,  -1079,  -552,  -173,  -1609,  432,  -1886,\
     -806,  -812,  -1272,  -275,  -1426,  -851,  -1145,  -1003,  -1450,  -1199,\
     -1375,  -1105,  -760,  -936,  -1325,  -597,  -1282,  -997,  -983,  -2073,\
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
      285,   134,    45,   212,   160,   199,   -45,  -263,    45,   224,\
       36,  -401,   -69,  -258,  -297,  -270,  -480,  -443,  -677,  -166,\
     -112,  -599,  -755,  -120,  -486,   364,  -243,    -6,  -309,  -318,\
      134,    59,  1116,  -574,  1698,  -477,  -324, -1934,  -425,   519,\
     4095, -5759,  6981, -9086, 23197, 23197, -9086,  6981, -5759,  4095,\
      519,  -425, -1934,  -324,  -477,  1698,  -574,  1116,    59,   134,\
     -318,  -309,    -6,  -243,   364,  -486,  -120,  -755,  -599,  -112,\
     -166,  -677,  -443,  -480,  -270,  -297,  -258,   -69,  -401,    36,\
      224,    45,  -263,   -45,   199,   160,   212,    45,   134,   285,\
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
    -1882, -1184, -1035,  -891,  -244,  -816,  -769,  -993,  -857,  -412,\
       19,  -361,   222,  -226, -1279, -1443,   770,  -841,  -517,   484,\
    -1094,   498,  -945,   137,   293,     4, -1175,  -768,   322,   503,\
      219,   789,  -188, -1153, -1588, -3627,  1703, -1577,  4239,  -724,\
     3097, -1199,  4606,-12203, 26028, 26028,-12203,  4606, -1199,  3097,\
     -724,  4239, -1577,  1703, -3627, -1588, -1153,  -188,   789,   219,\
      503,   322,  -768, -1175,     4,   293,   137,  -945,   498, -1094,\
      484,  -517,  -841,   770, -1443, -1279,  -226,   222,  -361,    19,\
     -412,  -857,  -993,  -769,  -816,  -244,  -891, -1035, -1184, -1882,\
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
