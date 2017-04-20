#native runtime
PRODUCT_PACKAGES += \
					mic_array.p230 \
					runtime 

#siren
PRODUCT_PACKAGES += \
					libbsiren \
					libr2ssp \
					libztvad \
					libztcodec2 \
					libr2audio \
					libr2vt \
					libjsonc 


#speech
PRODUCT_PACKAGES += \
					libprotobuf-rokid-cpp-lite \
					libprotobuf-rokid-cpp-full \
					libgpr \
					libgrpc \
					libgrpc++ \
					libspeech_common \
					libspeech \
					libspeech_tts \
					librokid_tts_jni \
#					libspeech_asr \
					libspeech_nlp \
					librokid_speech_jni \
					rokid_tts \
					RKTtsService \

#java runtime
PRODUCT_PACKAGES += \
					rokid_framework \
					rokid_services \
					runtime_cmd

PRODUCT_PACKAGES += \
					roots.pem \
					tts_sdk.json \
					speech_profile.json
