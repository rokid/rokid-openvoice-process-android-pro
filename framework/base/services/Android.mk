LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := rokid_services

LOCAL_SRC_FILES :=$(call all-java-files-under,java)

services := \
	core

LOCAL_JAVA_LIBRARIES := rokid_framework
LOCAL_STATIC_JAVA_LIBRARIES := $(addprefix rokid_services.,$(services))
include $(BUILD_JAVA_LIBRARY)

include $(CLEAR_VARS)

ifeq (,$(ONE_SHOT_MAKEFILE))
include $(call all-makefiles-under,$(LOCAL_PATH))
else
include $(patsubst %,$(LOCAL_PATH)/%/Android.mk,$(services))
endif

