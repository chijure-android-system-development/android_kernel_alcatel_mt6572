LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_PREBUILT_LIBS := \
	lib/libh264dec_xa.ca7.so \
	lib/libh264dec_xb.ca7.so \
	lib/libh264dec_customize.so \
	lib/libmp4dec_sa.ca7.so \
	lib/libmp4dec_sb.ca7.so \
	lib/libmp4dec_customize.so \
	lib/libvp8dec_xa.ca7.so \
	lib/libmp4enc_xa.ca7.so \
	lib/libmp4enc_xb.ca7.so \
	lib/libh264enc_sa.ca7.so \
	lib/libh264enc_sb.ca7.so \
	lib/libvcodec_oal.so
	
ifeq ($(strip $(MTK_ASF_PLAYBACK_SUPPORT)), yes)
LOCAL_PREBUILT_LIBS += \
	lib/libvc1dec_sa.ca7.so
endif

ifeq ($(strip $(MTK_RMVB_PLAYBACK_SUPPORT)), yes)
	LOCAL_PREBUILT_LIBS += \
	librv9dec_xa.ca9.so \
	librv9dec_customize.so    
endif	

include $(BUILD_MULTI_PREBUILT)