
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
		src/IRuntimeService.cpp \
		src/RuntimeService.cpp \
		src/VoiceEngine.cpp \
		src/runtime_main.cpp

LOCAL_SHARED_LIBRARIES := \
		libbinder \
		liblog \
		libhardware \
		libbsiren \
		libjsonc \
		libutils \
		libgpr \
		libgrpc \
		libgrpc++ \
		libprotobuf-rokid-cpp-lite \
		libprotobuf-rokid-cpp-full \
		libspeech_common \
		libspeech \
		libspeech_nlp \
		libspeech_tts \
		libspeech_asr

SPEECH_DIR_DEP = speech/external
SPEECH_DIR = speech/src

LOCAL_MODULE := libspeech_common
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
PROTOC_OUT_DIR := $(call local-intermediates-dir)

LOCAL_MODULE :=
LOCAL_MODULE_CLASS :=
$(warning $(PROTOC_OUT_DIR))

LOCAL_C_INCLUDES += \
		$(LOCAL_PATH)/include \
		$(LOCAL_PATH)/hardware/include \
		rokid-blacksiren/thirdparty/libjsonc/include \
		speech/include 
#		$(SPEECH_DIR)/nlp \
		$(SPEECH_DIR)/asr \
		$(SPEECH_DIR)/common \
		$(SPEECH_DIR_DEP)/protobuf/src \
		$(SPEECH_DIR_DEP)/grpc/include \
		$(PROTOC_OUT_DIR)/gen \

IGNORED_WARNINGS := -Wno-sign-compare -Wno-unused-parameter -Wno-sign-promo -Wno-error=return-type -Wno-error=non-virtual-dtor
#LOCAL_RTTI_FLAG := -frtti
LOCAL_MODULE := runtime
LOCAL_ARM_MODE := arm
LOCAL_CPPFLAGS := -DCURRENT_ANDROID_PLATFORM_SDK_VERSION=$(PLATFORM_SDK_VERSION) $(IGNORED_WARNINGS) -DGOOGLE_PROTOBUF_NO_RTTI
#LOCAL_CFLAGS := -w
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_TARGET_ARCH := arm

include $(BUILD_EXECUTABLE)

include $(call first-makefiles-under,$(LOCAL_PATH))
