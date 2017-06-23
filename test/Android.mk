LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_SRC_FILES := \
		test.cpp

LOCAL_SHARED_LIBRARIES := \
		libutils \
		libbinder

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

LOCAL_MODULE := openvoice_test
LOCAL_ARM_MODE := arm
LOCAL_CPPFLAGS := -DCURRENT_ANDROID_PLATFORM_SDK_VERSION=$(PLATFORM_SDK_VERSION) -std=c++11
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_TARGET_ARCH := arm

include $(BUILD_EXECUTABLE)