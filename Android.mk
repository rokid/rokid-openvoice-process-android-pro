LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
		src/IRuntimeService.cpp \
		src/RuntimeService.cpp \
		src/voice.cpp \
		src/runtime_main.cpp

LOCAL_SHARED_LIBRARIES := \
		libhardware \
		libbinder \
		libbsiren \
		libutils \
		liblog \
		libspeech \
#		libspeech_asr \
		libspeech_nlp

LOCAL_STATIC_LIBRARIES += libjsonc_static 

OPENVOICE_DEP_DIR = robot/openvoice

LOCAL_C_INCLUDES += \
		$(LOCAL_PATH)/include \
		$(LOCAL_PATH)/hardware/include \
		$(OPENVOICE_DEP_DIR)/rokid-blacksiren/include \
		$(OPENVOICE_DEP_DIR)/rokid-blacksiren/thirdparty/libjsonc/include \
		$(OPENVOICE_DEP_DIR)/speech/include

ifeq ($(PLATFORM_SDK_VERSION), 22)
LOCAL_SHARED_LIBRARIES += libdl
LOCAL_STATIC_LIBRARIES += libc++
LOCAL_C_INCLUDES += external/libcxx/include
else ifeq ($(PLATFORM_SDK_VERSION), 19)
LOCAL_C_INCLUDES += \
		external/stlport/stlport \
		bionic
LOCAL_STATIC_LIBRARIES += libstlport_static
endif
#protostream_objectwriter.cc

LOCAL_MODULE := runtime
LOCAL_ARM_MODE := arm
LOCAL_CPPFLAGS := -DCURRENT_ANDROID_PLATFORM_SDK_VERSION=$(PLATFORM_SDK_VERSION) -std=c++11
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_TARGET_ARCH := arm

include $(BUILD_EXECUTABLE)

include $(call first-makefiles-under,$(LOCAL_PATH))
