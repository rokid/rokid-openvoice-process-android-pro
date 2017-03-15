#define LOG_TAG "RKRuntimeService"
#define LOG_NDEBUG 0


#include "include/RKRuntimeService.h"

using namespace android;
using namespace std;

bool RKRuntimeService::init(){
	//1. init siren
	
	//2. set siren callback
	siren_callback = (siren_proc_callback_t*)malloc(sizeof(siren_proc_callback_t));
	siren_callback->voice_event_callback = voice_event_callback;
	start_siren_process_stream(siren_callback);
	//3. set siren state
	pthread_create(&siren_thread, NULL, siren_thread_loop, this);
	pthread_join(siren_thread, NULL);
	return true;
}

void voice_event_callback(void* token, int length, siren_event_t event, 
		void* buff, int has_sl,
		int has_voice, double sl_degree,
		int has_voiceprint){
	ALOGV("voice_event_callback length %d, has_voice %d , event %d", length, has_voice, event);
	RKRuntimeService *runtime_service = (RKRuntimeService*)token;
	pthread_mutex_lock(runtime_service->siren_mutex);
	//add to siren_msgs
	RKRuntimeService::VoiceMessage *voice_msg = new RKRuntimeService::VoiceMessage();
	runtime_service->siren_msgs.push_front(voice_msg);
	pthread_cond_signal(runtime_service->siren_cond);
	pthread_mutex_unlock(runtime_service->siren_mutex);
}

void* siren_thread_loop(void* arg){
	RKRuntimeService *runtime_service = (RKRuntimeService*)arg;
	for(;;){
		pthread_mutex_lock(runtime_service->siren_mutex);
		if(runtime_service->siren_msgs.empty()){
			pthread_cond_wait(runtime_service->siren_cond, runtime_service->siren_mutex);
		}
		RKRuntimeService::VoiceMessage *voice_msg = runtime_service->siren_msgs.front();
		//send to speech
		pthread_mutex_unlock(runtime_service->siren_mutex);
	}
	ALOGV("thread quit!");
	return NULL;
}
