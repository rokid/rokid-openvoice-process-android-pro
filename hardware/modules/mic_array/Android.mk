LOCAL_PATH := $(call my-dir)

ifneq (, $(filter $(TARGET_DEVICE),msm8974))
include $(LOCAL_PATH)/qcom_msm89xx/build.mk
else
include $(LOCAL_PATH)/amlogic_s9xx/build.mk
endif


