LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(shell cp $(LOCAL_PATH)/etc/openvoice_profile.json $(TARGET_OUT_ETC))

include $(call first-makefiles-under,$(LOCAL_PATH))
