LOCAL_PATH := $(call my-dir)
ifneq (, $(filter $(TARGET_DEVICE),p230))
include $(LOCAL_PATH)/qcom_msm89xx/build.mk
endif

