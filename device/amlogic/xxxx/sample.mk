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
					libgpr \
					libgrpc \
					libgrpc++ \
					libprotobuf-rokid-cpp-lite \
					libprotobuf-rokid-cpp-full \
					libspeech_asr \
					libspeech_common \
					libspeech_nlp \
					libspeech_tts

#java runtime
PRODUCT_PACKAGES += \
					rokid_framework \
					rokid_services \
					runtime_cmd
