LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

$(shell cp $(LOCAL_PATH)/etc/openvoice_profile.json $(TARGET_OUT_ETC))

LOCAL_SRC_FILES := \
		src/IVoiceService.cpp \
		src/VoiceService.cpp \
		src/Engine.cpp \
		src/main.cpp

LOCAL_SHARED_LIBRARIES := \
		libhardware \
		libbinder \
		libbsiren \
		libutils \
		liblog \
		libspeech

LOCAL_STATIC_LIBRARIES += libjsonc_static 

LOCAL_C_INCLUDES += \
		$(LOCAL_PATH)/include \
		$(LOCAL_PATH)/hardware/include \
		$(LOCAL_PATH)/../rokid-blacksiren/include \
		$(LOCAL_PATH)/../rokid-blacksiren/thirdparty/libjsonc/include

ifeq ($(PLATFORM_SDK_VERSION), 22)
LOCAL_SHARED_LIBRARIES += libc++ libdl
LOCAL_C_INCLUDES += external/libcxx/include
else ifeq ($(PLATFORM_SDK_VERSION), 19)
LOCAL_C_INCLUDES += \
		external/stlport/stlport \
		bionic
LOCAL_STATIC_LIBRARIES += libstlport_static
endif

LOCAL_MODULE := openvoice_proc
LOCAL_ARM_MODE := arm
LOCAL_CPPFLAGS := -DCURRENT_ANDROID_PLATFORM_SDK_VERSION=$(PLATFORM_SDK_VERSION) -std=c++11 #-DUSB_AUDIO_DEVICE
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_TARGET_ARCH := arm

include $(BUILD_EXECUTABLE)

include $(call first-makefiles-under,$(LOCAL_PATH))
