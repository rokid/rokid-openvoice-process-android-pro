#native runtime
PRODUCT_PACKAGES += \
					mic_array.$(TARGET_PRODUCT) \
					runtime 
#siren
PRODUCT_PACKAGES += \
					libbsiren \
					libr2ssp \
					libztvad \
					libztcodec2 \
					libr2audio \
					libr2vt

#speech
PRODUCT_PACKAGES += \
					libprotobuf-rokid-cpp-full \
					libpoco \
					libspeech_common \
					libspeech \
#					libspeech_tts \
					libspeech_asr \
					libspeech_nlp \
					librokid_tts_jni \
					librokid_asr_jni \
					librokid_speech_jni \
					rokid_tts \
					rokid_asr \
					rokid_speech \
					librokid_tts_util

#apps
PRODUCT_PACKAGES += \
					RuntimeService \
					RKTtsService 

PRODUCT_PACKAGES += \
					roots.pem \
					workdir_cn \
					workdir_en \
					blacksiren.json \
					openvoice_profile.json
