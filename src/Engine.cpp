#define LOG_TAG "Voice"
#define LOG_NDEBUG 0

#include <string.h>
#include <hardware/hardware.h>
#include "engine.h"

struct mic_array_device_t *mic_array_device = NULL;
siren_t _siren;

siren_input_if_t siren_input = {
	init_input,
	release_input,
	start_input,
	stop_input,
	read_input,
	on_err_input
};

siren_proc_callback_t siren_callback = {
	voice_event_callback
};

siren_state_changed_callback_t siren_state_change = {
	state_changed_callback
};

static inline int mic_array_device_open(const hw_module_t *module, struct mic_array_device_t **device){
	return module->methods->open(module, MIC_ARRAY_HARDWARE_MODULE_ID, (struct hw_device_t **)device);
}

bool _init_siren(VoiceService *service){
	//1. open mic driver.
	mic_array_module_t *module;
	if(hw_get_module(MIC_ARRAY_HARDWARE_MODULE_ID, (const struct hw_module_t **)&module) != 0){
		ALOGV("cannot find mic_array");
		return false;
	}
	if(mic_array_device_open(&module->common, &mic_array_device) != 0){
		ALOGE("open mic_array failed"); return false;
	}	
	ALOGI ("open mic array done");
	//2. init siren
	_siren = init_siren(service, NULL, &siren_input);	
	return true;
}

void _start_siren_process_stream(){
	start_siren_process_stream(_siren, &siren_callback);
}

void _stop_siren_process_stream(){
	stop_siren_process_stream(_siren);
}

void set_siren_state_change(int state){
	set_siren_state(_siren, state, &siren_state_change);
}

int init_input(void *token){
	return 0;
}

void release_input(void *token){
	mic_array_device->finish_stream(mic_array_device);
}

int start_input(void *token){
	return mic_array_device->start_stream(mic_array_device);
}

void stop_input(void *token){
	mic_array_device->stop_stream(mic_array_device);
	ALOGV("%s", __FUNCTION__);
}

int read_input(void *token, char *buff, int	frame_cnt){
	return mic_array_device->read_stream(mic_array_device, buff, frame_cnt);
}

int find_card(const char *snd){
	return mic_array_device->find_card(snd);
}

void on_err_input(void *token){
	ALOGV("%s", __FUNCTION__);
}

void state_changed_callback(void *token, int state){
	ALOGV("%s : %d", __FUNCTION__, state);
}

void voice_event_callback(void *token, voice_event_t *voice_event){

	VoiceService *service = (VoiceService*)token;
	if(service == NULL) return;
	pthread_mutex_lock(&service->event_mutex);
	//add to siren_queue
    voice_event_t *voice_message = (voice_event_t*)malloc(sizeof(voice_event_t));
    memcpy(voice_message, voice_event, sizeof(voice_event_t));
    void *buff = NULL;
    if(HAS_VOICE(voice_message->flag) || HAS_VT(voice_message->flag)){
        assert(voice_event->length > 0);
        buff = malloc(voice_event->length);
        memcpy(buff, voice_event->buff, voice_event->length);
        voice_message->buff = buff;
    }
	service->message_queue.push_back(voice_message);
	pthread_cond_signal(&service->event_cond);
	pthread_mutex_unlock(&service->event_mutex);
}
