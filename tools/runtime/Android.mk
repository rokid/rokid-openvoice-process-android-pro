# Copyright 2007 The Android Open Source Project
#
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_JAVA_LIBRARIES := framework

LOCAL_SRC_FILES := $(call all-subdir-java-files)
LOCAL_MODULE := runtime
include $(BUILD_JAVA_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := runtime
LOCAL_SRC_FILES := runtime
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_MODULE_TAGS := optional
include $(BUILD_PREBUILT)
