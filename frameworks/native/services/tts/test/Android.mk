LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := test.cpp
LOCAL_SHARED_LIBRARIES := \
	libutils \
	libbinder \
	libtts

ifeq ($(PLATFORM_SDK_VERSION), 22)
LOCAL_SHARED_LIBRARIES += libc++ libdl
LOCAL_C_INCLUDES += external/libcxx/include
endif

LOCAL_MODULE := tts_test
LOCAL_ARM_MODE := arm
LOCAL_CPPFLAGS := -DCURRENT_ANDROID_PLATFORM_SDK_VERSION=$(PLATFORM_SDK_VERSION) -std=c++11
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_TARGET_ARCH := arm

include $(BUILD_EXECUTABLE)
