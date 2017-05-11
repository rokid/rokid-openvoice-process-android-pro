LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(shell cp $(LOCAL_PATH)/openvoice_profile.json $(TARGET_OUT_ETC))
