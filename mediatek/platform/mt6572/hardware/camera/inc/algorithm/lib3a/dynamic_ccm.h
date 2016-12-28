

#ifndef _DYNAMIC_CCM_H
#define _DYNAMIC_CCM_H

enum EIndex_CCM
{
    eIDX_CCM_D65  = 0,
    eIDX_CCM_TL84,
    eIDX_CCM_CWF,
    eIDX_CCM_A,
    eIDX_CCM_NUM
};

// CCM
typedef struct
{
    MINT32 M11;
    MINT32 M12;
    MINT32 M13;
    MINT32 M21;
    MINT32 M22;
    MINT32 M23;
    MINT32 M31;
    MINT32 M32;
    MINT32 M33;
} ISP_CCM_T;

MVOID MultiCCM(AWB_GAIN_T& rD65,
               AWB_GAIN_T& rTL84,
               AWB_GAIN_T& rCWF,
               AWB_GAIN_T& rA,
               AWB_GAIN_T const& rCurrent, 
               ISP_NVRAM_CCM_POLY22_STRUCT& rCCMPoly22,
               ISP_NVRAM_CCM_T (&rCCMInput)[eIDX_CCM_NUM], 
               ISP_NVRAM_CCM_T& rCCMOutput);


#endif