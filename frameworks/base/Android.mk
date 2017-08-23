LOCAL_PATH := $(call my-dir)

SDK_VERSION_22 = $(shell if [ $(PLATFORM_SDK_VERSION) -ge 22 ]; then echo true; fi)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(call find-other-java-files,$(FRAMEWORKS_BASE_SUBDIRS))

LOCAL_SRC_FILES += \
				   core/java/rokid/tts/ITts.aidl \
				   core/java/rokid/tts/ITtsCallback.aidl

LOCAL_AIDL_INCLUDES += $(LOCAL_PATH)/core/java

#LOCAL_JACK_ENABLED = disabled
LOCAL_NO_STANDARD_LIBRARIES := true
LOCAL_JAVA_LIBRARIES := core-libart conscrypt core-junit bouncycastle ext framework

LOCAL_MODULE := falcon_framework

LOCAL_DX_FLAGS := --core-library
ifeq ($(SDK_VERSION_22), true)
LOCAL_DX_FLAGS += --multi-dex
endif
LOCAL_JACK_FLAGS := --multi-dex native

LOCAL_RMTYPEDEFS := true

include $(BUILD_JAVA_LIBRARY)

