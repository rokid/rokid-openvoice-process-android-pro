#define LOG_TAG "RuntimeService"
#define LOG_NDEBUG 0

#include "include/RuntimeService.h"
#include "include/VoiceEngine.h"
#include <binder/IServiceManager.h>
#include <cutils/log.h>
#include <unistd.h>

using namespace android;
using namespace std;
using namespace siren;
using namespace rokid;
using namespace speech;

VoiceEngine *voice_engine;

bool RuntimeService::init(){
	voice_engine = new VoiceEngine();
//	if(!voice_engine->init(this)){
//		ALOGE("init siren failed.");
//		return false;
//	}
	pthread_create(&siren_thread, NULL, siren_thread_loop, this);
	pthread_join(siren_thread, NULL);
	return true;
}

void RuntimeService::set_siren_state(const int &state){
	current_status = state;
	voice_engine->set_siren_state_change(state);
	ALOGV("current_status   >>>   %d", current_status);
}

int RuntimeService::get_siren_state(){
	ALOGV("current_status   >>>   %d", current_status);
	return current_status;
}

RuntimeService::~RuntimeService(){
	free(voice_engine);
}

void* siren_thread_loop(void* arg){
	ALOGV("thread join !!");
	RuntimeService *runtime_service = (RuntimeService*)arg;
	int id = -1;
	for(;;){
		pthread_mutex_lock(&runtime_service->siren_mutex);
		if(runtime_service->voice_queue.empty()){
			pthread_cond_wait(&runtime_service->siren_cond, &runtime_service->siren_mutex);
		}
		const RuntimeService::VoiceMessage *voice_msg = runtime_service->voice_queue.front();
		RuntimeService::MyNlpCallback *callback = new RuntimeService::MyNlpCallback(runtime_service);
		ALOGV("event   >>>   %d", voice_msg->event);
		//send to speech
		switch(voice_msg->event){
			case SIREN_EVENT_WAKE_CMD:
				runtime_service->current_status = SIREN_STATE_AWAKE;
				break;
			case SIREN_EVENT_WAKE_NOCMD:
			case SIREN_EVENT_SLEEP:
				runtime_service->current_status = SIREN_STATE_SLEEP;
				break;
			case SIREN_EVENT_VAD_START:
			case SIREN_EVENT_WAKE_VAD_START:
				break;
			case SIREN_EVENT_VAD_DATA:
			case SIREN_EVENT_WAKE_VAD_DATA:
				break;
			case SIREN_EVENT_VAD_END:
			case SIREN_EVENT_WAKE_VAD_END:
				break;
			case SIREN_EVENT_VAD_CANCEL:
			case SIREN_EVENT_WAKE_CANCEL:
				break;
			case SIREN_EVENT_WAKE_PRE:
				break;
		}
		runtime_service->mNlpCallback.insert(pair<int, RuntimeService::MyNlpCallback*>(id, callback));
		runtime_service->voice_queue.pop_front();
		delete voice_msg;
		pthread_mutex_unlock(&runtime_service->siren_mutex);
	}

	ALOGV("thread quit!");
	return NULL;
}

void RuntimeService::MyNlpCallback::onNlp(int id, const char *nlp){
	ALOGI("%d", nlp);
	if(!runtime_service->mNlpCallback.empty()){
		//list<RuntimeService::MyNlpCallback>::iterator it = mNlpCallback.begin();
	}
	sp<IBinder> binder = defaultServiceManager()->getService(String16("runtime_java"));
	if(binder == NULL){
		ALOGI("java runtime is null , Waiting for it to initialize");
		return ;
	}
	Parcel data, reply;
	data.writeInterfaceToken(String16("rokid.os.IRuntimeService"));
	data.writeString16(String16(nlp));
	binder->transact(IBinder::FIRST_CALL_TRANSACTION + 0, data, &reply);
	reply.readExceptionCode();
}

void RuntimeService::MyNlpCallback::onError(int id, int err){
	ALOGE("callback  error :  %d,   id  :  %d", err, id);
}
