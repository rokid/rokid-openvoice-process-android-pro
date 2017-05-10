LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
		src/IRuntimeService.cpp \
		src/RuntimeService.cpp \
		src/voice_engine.cpp \
		src/runtime_main.cpp

LOCAL_SHARED_LIBRARIES := \
		libhardware \
		libbinder \
		libbsiren \
		libutils \
		liblog \
		libspeech_common \
		libspeech \
		libspeech_asr 

LOCAL_STATIC_LIBRARIES += libjsonc_static 

OPENVOICE_DIR_DEP = openvoice

LOCAL_C_INCLUDES += \
		$(LOCAL_PATH)/include \
		$(LOCAL_PATH)/hardware/include \
		$(OPENVOICE_DIR_DEP)/rokid-blacksiren-master/include \
		$(OPENVOICE_DIR_DEP)/rokid-blacksiren-master/thirdparty/libjsonc/include \
		$(OPENVOICE_DIR_DEP)/speech/include

ifeq ($(PLATFORM_SDK_VERSION), 18)
LOCAL_SHARED_LIBRARIES += libdl
LOCAL_STATIC_LIBRARIES += libc++
LOCAL_C_INCLUDES += external/libcxx/include
endif

LOCAL_MODULE := runtime
#LOCAL_CXX_STL := libc++
LOCAL_ARM_MODE := arm
LOCAL_CPPFLAGS := -DCURRENT_ANDROID_PLATFORM_SDK_VERSION=$(PLATFORM_SDK_VERSION) -std=c++11
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_TARGET_ARCH := arm

include $(BUILD_EXECUTABLE)

include $(call first-makefiles-under,$(LOCAL_PATH))
