#define LOG_TAG "RuntimeService"
#define LOG_NDEBUG 0

#include "include/RuntimeService.h"
#include "include/SirenService.h"

using namespace android;
using namespace std;
using namespace siren;

bool RuntimeService::init(){
	SirenService* siren_service = new SirenService();
	if(!siren_service->init(this)){
		ALOGE("init siren failed.");
		return false;
	}

	pthread_create(&siren_thread, NULL, siren_thread_loop, this);
	pthread_join(siren_thread, NULL);

	return true;
}

void* siren_thread_loop(void* arg){
	RuntimeService *runtime_service = (RuntimeService*)arg;
	for(;;){
		pthread_mutex_lock(runtime_service->siren_mutex);
		if(runtime_service->voice_queue.empty()){
			pthread_cond_wait(runtime_service->siren_cond, runtime_service->siren_mutex);
		}
		RuntimeService::VoiceMessage *voice_msg = runtime_service->voice_queue.front();
		//send to speech
		pthread_mutex_unlock(runtime_service->siren_mutex);
	}

	ALOGV("thread quit!");
	return NULL;
}
