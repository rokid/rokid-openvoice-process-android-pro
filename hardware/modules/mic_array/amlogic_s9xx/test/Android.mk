# Copyright 2013 The Android Open Source Project
LOCAL_PATH:= $(call my-dir)

src_files = \
		mic_array_test.c

include $(CLEAR_VARS)
LOCAL_SRC_FILES:= $(src_files)
LOCAL_MODULE := mic_array_test_amlogic
LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES := \
	libandroid_runtime \
	libbinder \
	libcutils \
	liblog \
	libhardware \
	libutils \

LOCAL_C_INCLUDES += \
	robot/include \

ARCH_ARM=yes
HAVE_ARMV6=yes
HAVE_ARMV6T2=yes
HAVE_ARMVFP=yes
LOCAL_ARM_MODE := arm

EXTRA_CFLAGS="-march=armv7-a -mfloat-abi=softfp -mfpu=neon"
EXTRA_LDFLAGS="-Wl,--fix-cortex-a8 "
ABI="armeabi-v7a"

LOCAL_MODULE_TARGET_ARCH := arm

include $(BUILD_EXECUTABLE)

