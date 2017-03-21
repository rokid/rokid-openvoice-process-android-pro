LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
ROKID_FRAMEWORKS_BASE_SUBDIRS := \
	$(addsuffix /java, \
		core \
		)

ROKID_FRAMEWORKS_BASE_JAVA_SRC_DIRS := \
	$(addprefix rokid-openvoice-sample-android/framework/base, $(ROKID_FRAMEWORKS_BASE_SUBDIRS))

LOCAL_SRC_FILES := $(call find-other-java-files, $(ROKID_FRAMEWORKS_BASE_SUBDIRS))

LOCAL_SRC_FILES += \
				   core/java/rokid/os/IRuntimeService.aidl

LOCAL_AIDL_INCLUDES += $(ROKID_FRAMEWORKS_BASE_JAVA_SRC_DIRS)

### aidl files
define get-aidl-files
	$(addsuffix .aidl, \
		$(addprefix $(1)/, $(2)) \
	)
endef

CORE_AIDL_FILES := $(call get-aidl-files, $(LOCAL_PATH))
LOCAL_SRC_FILES += $(CORE_AIDL_FILES)

### modified by lujnan begin. for build on android 6.0.
LOCAL_NO_STANDARD_LIBRARIES := true
LOCAL_JAVA_LIBRARIES := bouncycastle conscrypt
LOCAL_JAVA_LIBRARIES += core-libart
LOCAL_JAVA_LIBRARIES += core-junit ext framework
LOCAL_JAVA_LIBRARIES += org.apache.http.legacy.boot
### modified by lujnan end.

LOCAL_MODULE := rokid-framework-base
LOCAL_JAR_EXCLUDE_FILES := none

include $(BUILD_STATIC_JAVA_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := rokid_framework
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_NO_STANDARD_LIBRARIES := true
LOCAL_STATIC_JAVA_LIBRARIES := rokid-framework-base
LOCAL_DX_FLAGS := --core-library
LOCAL_DX_FLAGS += --multi-dex
LOCAL_JACK_FLAGS := --multi-dex native
LOCAL_JAR_PACKAGES := rokid\*
LOCAL_JAR_PACKAGES += com\*

#LOCAL_JAVA_RESOURCE_FILES += $(LOCAL_PATH)/preloaded-classes
include $(BUILD_JAVA_LIBRARY)

# create api stubs jar for java sources in directories robot/base and robot/services
#  Include subdirectory makefiles
ifeq (,$(ONE_SHOT_MAKEFILE))
include $(call first-makefiles-under,$(LOCAL_PATH))
endif


