LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := mic_array.c
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
LOCAL_MODULE := mic_array.$(TARGET_DEVICE)

LOCAL_MODULE_TAGS := optional
LOCAL_C_INCLUDES += hardware/libhardware \
	$(LOCAL_PATH)/../../include \
	external/tinyalsa/include

LOCAL_SHARED_LIBRARIES := liblog libcutils libtinyalsa

LOCAL_MODULE_TARGET_ARCH := arm

include $(BUILD_SHARED_LIBRARY)
