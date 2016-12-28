LOCAL_PATH:= $(call my-dir)

ifeq ($(MTK_PLATFORM),MT6589)
###############################################################################
# SEC KERNEL MODULE
###############################################################################
include $(CLEAR_VARS)
LOCAL_MODULE := sec.ko
LOCAL_MODULE_CLASS := MODULE
LOCAL_MODULE_PATH := $(TARGET_OUT)/lib/modules
LOCAL_SRC_FILES := sec.ko
include $(BUILD_PREBUILT)

###############################################################################
# SEC RECOVERY MODULE
###############################################################################
include $(CLEAR_VARS)
LOCAL_MODULE := sbup.ko
LOCAL_MODULE_CLASS := MODULE
LOCAL_MODULE_PATH := $(TARGET_OUT)/lib/modules
LOCAL_SRC_FILES := sbup.ko
include $(BUILD_PREBUILT)
endif