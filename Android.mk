LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
		src/IRuntimeService.cpp \
		src/RuntimeService.cpp \
		src/voice_engine.cpp \
		src/runtime_main.cpp

LOCAL_SHARED_LIBRARIES := \
		libbinder \
		libhardware \
		libbsiren \
		libjsonc \
		libutils \
		libgpr \
		liblog \
		libgrpc \
		libgrpc++ \
		libprotobuf-rokid-cpp-lite \
		libprotobuf-rokid-cpp-full \
		libspeech_common \
		libspeech \

OPENVOICE_DIR_DEP = robot/openvoice

LOCAL_C_INCLUDES += \
		$(LOCAL_PATH)/include \
		$(LOCAL_PATH)/hardware/include \
		$(OPENVOICE_DIR_DEP)/rokid-blacksiren/include \
		$(OPENVOICE_DIR_DEP)/rokid-blacksiren/thirdparty/libjsonc/include \
		$(OPENVOICE_DIR_DEP)/rokid-openvoice-sdk/include 

IGNORED_WARNINGS := -Wno-sign-compare -Wno-unused-parameter -Wno-sign-promo -Wno-error=return-type -Wno-error=non-virtual-dtor
LOCAL_MODULE := runtime
LOCAL_ARM_MODE := arm
LOCAL_CPPFLAGS := -DCURRENT_ANDROID_PLATFORM_SDK_VERSION=$(PLATFORM_SDK_VERSION) $(IGNORED_WARNINGS) -DGOOGLE_PROTOBUF_NO_RTTI
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_TARGET_ARCH := arm

include $(BUILD_EXECUTABLE)
