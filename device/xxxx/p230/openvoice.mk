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
					libspeech_tts \
					librokid_tts_jni \
#					libspeech_asr \
					libspeech_nlp \
					librokid_speech_jni \
					rokid_tts \

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
