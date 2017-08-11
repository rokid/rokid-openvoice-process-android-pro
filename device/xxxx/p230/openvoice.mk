#openvoice_process
PRODUCT_PACKAGES += \
					mic_array.$(TARGET_PRODUCT) \
					libopenvoice \
					openvoice_proc
#siren
PRODUCT_PACKAGES += \
					libbsiren \
					libr2audio \
					libr2ssp \
					libztvad \
					libr2vt

#speech
PRODUCT_PACKAGES += \
					libprotobuf-rokid-cpp-full \
					libpoco \
					libspeech \
#					librokid_speech_jni \
					rokid_speech \

#apps
PRODUCT_PACKAGES += \
					VoiceService \
					RKTtsService

PRODUCT_PACKAGES += \
					roots.pem \
					workdir_cn \
					blacksiren.json \
					openvoice_profile.json
