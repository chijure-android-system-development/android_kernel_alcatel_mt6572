

#ifndef ANDROID_AUDIO_FTM_H
#define ANDROID_AUDIO_FTM_H


#include <stdint.h>
#include <sys/types.h>
#include <pthread.h>
#include "AudioDef.h"

#include "AudioVolumeFactory.h"
#include "AudioAnalogControlFactory.h"
#include "AudioDigitalControlFactory.h"

//#include "AudioMTKStreamOut.h"    //ship marked
//#include "AudioMTKStreamIn.h"
#include "AudioMTKStreamInManager.h"
#include "AudioDigitalType.h"
#include "AudioAnalogType.h"






namespace android
{

enum FMTX_Command {
    FREQ_NONE = 0,
    FREQ_1K_HZ,
    FREQ_2K_HZ,
    FREQ_3K_HZ,
    FREQ_4K_HZ,
    FREQ_5K_HZ,
    FREQ_6K_HZ,
    FREQ_7K_HZ,
    FREQ_8K_HZ,
    FREQ_9K_HZ,
    FREQ_10K_HZ,
    FREQ_11K_HZ,
    FREQ_12K_HZ,
    FREQ_13K_HZ,
    FREQ_14K_HZ,
    FREQ_15K_HZ
};

enum UL_SAMPLERATE_INDEX {
    UPLINK8K = 0,
    UPLINK16K,
    UPLINK32K,
    UPLINK48K,
    UPLINK_UNDEF
};


class AudioFtm
{
    public:
        static AudioFtm *getInstance();
        ~AudioFtm();

        bool AudFtm_Init();

        // for FM Tx and HDMI  factory mode & Meta mode
        bool WavGen_SW_SineWave(bool Enable, uint32 Freq, int type);
        bool WavGen_SWPattern(bool Enable, uint32 Freq, int type);
        static void *FmTx_thread_create(void *arg);
        void FmTx_thread_digital_out(void);
        void FmTx_thread_analog_out(void);
        static void *HDMI_thread_create(void *arg);
        void HDMI_thread_I2SOutput(void);
        void WavGen_AudioRead(char *pBuffer, unsigned int bytes);
        unsigned int WavGen_AudioWrite(void *buffer, unsigned int bytes);

        // for factory mode & Meta mode (Digital part)
        void Afe_Enable_SineWave(bool bEnable);

        // for factory mode & Meta mode (Analog part)
        void FTM_AnaLpk_on(void);
        void FTM_AnaLpk_off(void);

        void Audio_Set_Speaker_Vol(int level);
        void Audio_Set_Speaker_On(int Channel);
        void Audio_Set_Speaker_Off(int Channel);
        void Audio_Set_HeadPhone_On(int Channel);
        void Audio_Set_HeadPhone_Off(int Channel);
        void Audio_Set_Earpiece_On();
        void Audio_Set_Earpiece_Off();

        int RecieverTest(char receiver_test);
        int LouderSPKTest(char left_channel, char right_channel);
        int EarphoneTest(char bEnable);
        int Pmic_I2s_out(char echoflag);

        // FM loopbacvk test
        int FMLoopbackTest(char bEnable);
        int Audio_FM_I2S_Play(char bEnable);
        int Audio_MATV_I2S_Play(char bEnable);
        // speaker OC test
        int Audio_READ_SPK_OC_STA(void);
        int LouderSPKOCTest(char left_channel, char right_channel);

        void FTMI2SDacOutSet(uint32 SampleRate);

        void FTMPMICLoopbackTest(bool bEnable);
        void FTMPMICEarpieceLoopbackTest(bool bEnable);
        void FTMPMICDualModeLoopbackTest(bool bEnable);

        int RequestClock(void);
        int ReleaseClock(void);


        pthread_t m_WaveThread;
        bool mAudioSinWave_thread;
        unsigned int IdxAudioPattern;
        unsigned int SizeAudioPattern;
        unsigned char *g_i2VDL_DATA;
        char *mAudioBuffer;

    private:
        AudioMTKVolumeInterface *mAudioVolumeInstance;
        AudioAnalogControlInterface *mAudioAnalogInstance;
        AudioDigitalControlInterface *mAudioDigitalInstance;
        //AudioMTKStreamInManager *mStreamInManager;    //ship marked
        AudioResourceManagerInterface *mAudioResourceManager;
        AudioAnalogReg *mAudioAnalogReg;

        static AudioFtm *UniqueAudioFtmInstance;
        AudioFtm();
        AudioFtm(const AudioFtm &);             // intentionally undefined
        AudioFtm &operator=(const AudioFtm &);  // intentionally undefined

        int    mFd;
        int    mSamplingRate;
        int    mSineWaveStatus;
	//modify for dual mic cust by yi.zheng.hz begin
#if defined(JRD_HDVOICE_CUST)
	bool mbMtkDualMicSupport;
#endif
	//modify for dual mic cust by yi.zheng.hz end
};

}; // namespace android

#endif

