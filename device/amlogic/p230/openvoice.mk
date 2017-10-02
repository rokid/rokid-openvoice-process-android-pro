PRODUCT_PACKAGES += \
					falcon_framework

#hardware
PRODUCT_PACKAGES += \
					mic_array.$(TARGET_PRODUCT)

#siren
PRODUCT_PACKAGES += \
					libbsiren \
					libr2audio \
					libr2ssp \
					libztvad \
					libr2vt \
					libr2mvdrbf

#speech
PRODUCT_PACKAGES += \
					libprotobuf-rokid-cpp-full \
					libpoco \
					libspeech

#apps
PRODUCT_PACKAGES += \
					VoiceClient

#libraries
PRODUCT_PACKAGES += \
					libopenvoice \
					libtts \
					libropus

#C++ process
PRODUCT_PACKAGES += \
					openvoice_proc \
					tts_process
#config file
PRODUCT_PACKAGES += \
					workdir_cn
