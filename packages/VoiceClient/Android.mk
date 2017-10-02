LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	$(call all-java-files-under, src) \
	src/com/rokid/openvoice/IVoiceCallback.aidl
#	$(call all-proto-files-under, protos) \

LOCAL_MODULE_TAGS := optional
LOCAL_CERTIFICATE := platform
LOCAL_PACKAGE_NAME := VoiceClient
LOCAL_JAVA_LIBRARIES := falcon_framework 
LOCAL_PRIVILEGED_MODULE := true
LOCAL_PROGUARD_ENABLED := disabled

include $(BUILD_PACKAGE)
