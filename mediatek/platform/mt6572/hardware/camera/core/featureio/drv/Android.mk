LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

ifeq ($(BUILD_MTK_LDVT),true)
    LOCAL_CFLAGS += -DUSING_MTK_LDVT
endif

LOCAL_SRC_FILES:= \
  strobe/strobe_drv.cpp \
  strobe/flashlight_drv.cpp \
 
ifneq ($(BUILD_MTK_LDVT),true)
    LOCAL_SRC_FILES += \
  lens/mcu_drv.cpp \
  lens/lens_drv.cpp \
  lens/lens_sensor_drv.cpp
endif

LOCAL_C_INCLUDES:= \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/camera/inc \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/camera/inc/drv \
  $(TOP)/$(MTK_PATH_PLATFORM)/hardware/camera/core/featureio/drv/inc \
  $(TOP)/$(MTK_PATH_PLATFORM)/hardware/camera/core/drv/imem \
  $(TOP)/$(MTK_PATH_PLATFORM)/hardware/camera/inc/common \
  $(TOP)/$(MTK_PATH_PLATFORM)/hardware/camera/inc/drv \
  $(TOP)/$(MTK_PATH_PLATFORM)/hardware/camera/inc/featureio \
  $(TOP)/$(MTK_PATH_PLATFORM)/external/ldvt/include \
  $(TOP)/$(MTK_PATH_PLATFORM)/hardware/m4u \
  $(MTK_PATH_CUSTOM)/kernel/imgsensor/inc \
  $(MTK_PATH_SOURCE)/external/nvram/libnvram \
  $(MTK_PATH_CUSTOM)/hal/inc/aaa \
  $(MTK_PATH_CUSTOM)/hal/inc \
  $(MTK_PATH_CUSTOM)/hal/camera \
  $(MTK_PATH_CUSTOM)/kernel/flashlight/inc \
  $(MTK_PATH_CUSTOM)/kernel/lens/inc \

#endif

PLATFORM_VERSION_MAJOR := $(word 1,$(subst .,$(space),$(PLATFORM_VERSION)))
LOCAL_CFLAGS += -DPLATFORM_VERSION_MAJOR=$(PLATFORM_VERSION_MAJOR)

LOCAL_STATIC_LIBRARIES := \

LOCAL_WHOLE_STATIC_LIBRARIES := \
    
LOCAL_MODULE:= libfeatureiodrv

include $(BUILD_STATIC_LIBRARY)

include $(call all-makefiles-under,$(LOCAL_PATH))
