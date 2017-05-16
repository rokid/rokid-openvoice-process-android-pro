#define LOG_TAG "VoiceEngine"
#define LOG_NDEBUG 0

#include <string.h>
#include <hardware/hardware.h>
#include "voice.h"

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

bool _init(RuntimeService *runtime){
	//1. open mic driver.
	mic_array_module_t *module;
	if(hw_get_module(MIC_ARRAY_HARDWARE_MODULE_ID, (const struct hw_module_t **)&module) != 0){
		ALOGV("cannot find mic_array");
		return false;
	}
	if(mic_array_device_open(&module->common, &mic_array_device) != 0){
		ALOGE("open mic_array failed");
		return false;
	}	
	ALOGI ("open mic array done");
	//2. init siren
	_siren = init_siren(runtime, NULL, &siren_input);	
	//3. set siren callback	
	start_siren_process_stream(_siren, &siren_callback);
	//set_siren_state_change(SIREN_STATE_SLEEP);
	return true;
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

void start_input(void *token){
	mic_array_device->start_stream(mic_array_device);
//	ALOGV("start input ..");
}

void stop_input(void *token){
	mic_array_device->stop_stream(mic_array_device);
	ALOGV("%s", __FUNCTION__);
}

int read_input(void *token, char *buff, int	frame_cnt){
	//ALOGV("read input ..");
	return mic_array_device->read_stream(mic_array_device, buff, frame_cnt);
}

void on_err_input(void *token){
	ALOGV("%s", __FUNCTION__);
}

void state_changed_callback(void *token, int state){
	ALOGV("%s : %d", __FUNCTION__, state);
}

void voice_event_callback(void *token, int length, siren_event_t event, 
		void* buff, int has_sl,
		int has_voice, double sl_degree, 
		double energy, double threshold,
		int has_voiceprint){

	ALOGV("voice_event_callback    >>>  has_voice : %d, len : %d", has_voice, length);
	RuntimeService *runtime = (RuntimeService*)token;
	if(runtime == NULL) return;
	pthread_mutex_lock(&runtime->event_mutex);
	//add to siren_queue
	RuntimeService::VoiceMessage *message = new RuntimeService::VoiceMessage();
	char *_cache = NULL;
	if(has_voice > 0){
		assert(length >= 0);
		_cache = new char[length];
		memcpy(_cache, buff, length);	
		message->buff = _cache;
	}
	message->length = length;
	message->event = event;
	message->has_voice = has_voice;
	message->has_sl = has_sl;
	message->sl_degree = sl_degree;
	message->sl_degree = sl_degree;
	message->energy = energy;
	message->threshold = threshold;
	message->has_voiceprint = has_voiceprint;
	runtime->message_queue.push_back(message);

	pthread_cond_signal(&runtime->event_cond);
	pthread_mutex_unlock(&runtime->event_mutex);
}

