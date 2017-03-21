LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := rokid_services.core
LOCAL_SRC_FILES += \
		$(call all-java-files-under,java) 

LOCAL_JAVA_LIBRARIES := rokid_framework


include $(BUILD_STATIC_JAVA_LIBRARY)
