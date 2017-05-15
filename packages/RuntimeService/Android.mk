LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
		$(call all-java-files-under, src) \
		src/com/rokid/openvoice/IRuntimeService.aidl

#LOCAL_JAVA_LIBRARIES := \
	framework 

LOCAL_MODULE_TAGS := optional
LOCAL_CERTIFICATE := platform
LOCAL_PACKAGE_NAME := RuntimeService
LOCAL_PRIVILEGED_MODULE := true
LOCAL_PROGUARD_ENABLED := disabled

include $(BUILD_PACKAGE)
