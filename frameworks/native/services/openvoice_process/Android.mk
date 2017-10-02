LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := libopenvoice
LOCAL_MODULE_CLASS := SHARED_LIBRARIES

ifeq ($(PLATFORM_SDK_VERSION), 19)
MY_PROTOC_OUT_DIR := $(call local-intermediates-dir)/proto
else
MY_PROTOC_OUT_DIR := $(call local-generated-sources-dir)/proto
endif

#		$(call all-proto-files-under, proto)
LOCAL_SRC_FILES := \
		src/IVoiceService.cpp \
		src/callback/IVoiceCallback.cpp

LOCAL_SHARED_LIBRARIES := \
		libbinder \
		libutils

LOCAL_C_INCLUDES := \
		$(LOCAL_PATH)/include \
		$(MY_PROTOC_OUT_DIR)/$(LOCAL_PATH)/proto \
#		external/protobuf/src

LOCAL_EXPORT_C_INCLUDE_DIRS := \
		$(LOCAL_PATH)/include \
		$(MY_PROTOC_OUT_DIR)/$(LOCAL_PATH)/proto

ifeq ($(PLATFORM_SDK_VERSION), 23)
#LOCAL_SHARED_LIBRARIES += libprotobuf-cpp-full
else ifeq ($(PLATFORM_SDK_VERSION), 22)
LOCAL_C_INCLUDES += external/libcxx/include
LOCAL_SHARED_LIBRARIES += libc++ libdl
else ifeq ($(PLATFORM_SDK_VERSION), 19)
LOCAL_STATIC_LIBRARIES += \
		libprotobuf-cpp-2.3.0-full \
		libstlport_static
LOCAL_C_INCLUDES += \
		external/stlport/stlport \
		bionic
endif

LOCAL_CPPFLAGS := -DPLATFORM_SDK_VERSION=$(PLATFORM_SDK_VERSION)
include $(BUILD_SHARED_LIBRARY)

#########################################################################
#
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
		src/VoiceService.cpp \
		src/CallbackProxy.cpp \
		src/siren_control.cpp \
		src/main.cpp

LOCAL_SHARED_LIBRARIES := \
		libhardware \
		libbinder \
		libutils \
		liblog \
		libopenvoice \
		libbsiren \
		libspeech

LOCAL_C_INCLUDES += \
		$(LOCAL_PATH)/include \
		$(LOCAL_PATH)/../../../../hardware/include \
		$(LOCAL_PATH)/../../../../../rokid-blacksiren/thirdparty/libjsonc/include

ifeq ($(PLATFORM_SDK_VERSION), 23)
#LOCAL_SHARED_LIBRARIES += libprotobuf-cpp-full
else ifeq ($(PLATFORM_SDK_VERSION), 22)
LOCAL_SHARED_LIBRARIES += libc++ libdl
LOCAL_C_INCLUDES += external/libcxx/include
else ifeq ($(PLATFORM_SDK_VERSION), 19)
MY_LOCAL_STATIC_LIBRARIES := prebuilts/ndk/current/sources/cxx-stl/gnu-libstdc++/libs/$(TARGET_CPU_ABI)/libgnustl_static.a
LOCAL_LDFLAGS += $(MY_LOCAL_STATIC_LIBRARIES) -ldl
LOCAL_STATIC_LIBRARIES += libprotobuf-cpp-2.3.0-full
LOCAL_C_INCLUDES += \
		prebuilts/ndk/current/sources/cxx-stl/gnu-libstdc++/libs/$(TARGET_CPU_ABI)/include \
		prebuilts/ndk/current/sources/cxx-stl/gnu-libstdc++/include \
		prebuilts/ndk/current/platforms/android-14/arch-arm/usr/include \
		external/protobuf/src
endif

LOCAL_MODULE := openvoice_proc
LOCAL_ARM_MODE := arm
LOCAL_CPPFLAGS := -DCURRENT_ANDROID_PLATFORM_SDK_VERSION=$(PLATFORM_SDK_VERSION) -std=c++11 #-DUSB_AUDIO_DEVICE
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_TARGET_ARCH := arm

include $(BUILD_EXECUTABLE)
