#define LOG_TAG "SirenService"
#define LOG_NDEBUG 0

#include <hardware/hardware.h>
#include "include/SirenService.h"

using namespace siren;
using namespace android;

struct mic_array_device_t *mic_array_device;

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

static inline int mic_array_device_open(const hw_module_t *module, struct mic_array_device_t **device){
	return module->methods->open(module, MIC_ARRAY_HARDWARE_MODULE_ID, (struct hw_device_t **)device);
}

bool SirenService::init(RuntimeService *runtime){
	this->runtime_service = runtime;

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
	//init_siren(runtime, NULL, &siren_input);	
	//3. set siren callback	
	//start_siren_process_stream(&siren_callback);
	return true;
}

int init_input(void *token){
	ALOGV("init input ..");
	return -1;
}

void release_input(void *token){
	ALOGV("release input ..");
}

void start_input(void *token){
	ALOGV("start input ..");
}

void stop_input(void *token){
	ALOGV("stop input ..");
}

int read_input(void *token, char *buff, int	len){
	ALOGV("read input ..");
	return -1;
}

void on_err_input(void *token){

}

void voice_event_callback(void *token, int length, siren_event_t event, 
		void* buff, int has_sl,
		int has_voice, double sl_degree,
		int has_voiceprint){

	ALOGV("voice_event_callback length %d, has_voice %d , event %d", length, has_voice, event);
	RuntimeService *runtime_service = (RuntimeService*)token;
	pthread_mutex_lock(runtime_service->siren_mutex);
	//add to siren_queue
	RuntimeService::VoiceMessage *voice_msg = new RuntimeService::VoiceMessage();

	runtime_service->voice_queue.push_front(voice_msg);
	pthread_cond_signal(runtime_service->siren_cond);
	pthread_mutex_unlock(runtime_service->siren_mutex);
}

