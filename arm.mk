LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	src/IRKRuntimeService.cpp \
	src/RKRuntimeService.cpp \
	src/runtime_main.cpp

LOCAL_SHARED_LIBRARIES := \
		libbinder \
		liblog \
		libutils 

THIRD_INCLUDES += $(LOCAL_PATH)/include

LOCAL_MODULE := runtime
LOCAL_ARM_MODE := arm
LOCAL_CPPFLAGS := -DCURRENT_ANDROID_PLATFORM_SDK_VERSION=$(PLATFORM_SDK_VERSION)
LOCAL_MODULE_TARGET_ARCH := arm
include $(BUILD_EXECUTABLE)
