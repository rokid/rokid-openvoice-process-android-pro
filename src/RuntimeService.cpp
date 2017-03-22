#define LOG_TAG "RuntimeService"
#define LOG_NDEBUG 0

#include "include/RuntimeService.h"
#include "include/VoiceEngine.h"
#include <binder/IServiceManager.h>

using namespace android;
using namespace std;
using namespace siren;
using namespace rokid;
using namespace speech;

VoiceEngine *voice_engine;

bool RuntimeService::init(){
	voice_engine = new VoiceEngine();
	if(!voice_engine->init(this)){
		ALOGE("init siren failed.");
		return false;
	}
	set_siren_state(SIREN_STATE_SLEEP);
	pthread_create(&siren_thread, NULL, siren_thread_loop, this);
//	pthread_join(siren_thread, NULL);
	return true;
}

void RuntimeService::set_siren_state(const int &state){
	current_status = state;
	voice_engine->set_siren_state_change(state);
	ALOGV("current_status     >>   %d,   %d", current_status , state);
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
	RuntimeService::MyNlpCallback *callback = NULL;
	Nlp *nlp = NULL;
	int id = -1;
	for(;;){
		pthread_mutex_lock(&runtime_service->siren_mutex);
		if(runtime_service->voice_queue.empty()){
			pthread_cond_wait(&runtime_service->siren_cond, &runtime_service->siren_mutex);
		}
		const RuntimeService::VoiceMessage *voice_msg = runtime_service->voice_queue.front();
		switch(voice_msg->event){
			case SIREN_EVENT_WAKE_CMD:
				ALOGV("voice event   >>>   wake_cmd");
				runtime_service->current_status = SIREN_STATE_AWAKE;
				break;
			case SIREN_EVENT_WAKE_NOCMD:
			case SIREN_EVENT_SLEEP:
				ALOGV("voice event   >>>   wake_nocmd or sleep");
				runtime_service->current_status = SIREN_STATE_SLEEP;
				break;
			case SIREN_EVENT_VAD_START:
			case SIREN_EVENT_WAKE_VAD_START:
				ALOGV("voice event   >>>   start");
				runtime_service->current_status = SIREN_STATE_AWAKE;
				nlp = new Nlp();
				nlp->prepare();
				break;
			case SIREN_EVENT_VAD_DATA:
			case SIREN_EVENT_WAKE_VAD_DATA:
				callback = NULL;
				callback = new RuntimeService::MyNlpCallback(runtime_service, nlp);
				callback->event = RuntimeService::VOICE_STATE_DATA;
				id = nlp->request((char *)voice_msg->buff, callback);
				runtime_service->mNlpCallback.insert(pair<int, 
						RuntimeService::MyNlpCallback*>(id, callback));
				break;
			case SIREN_EVENT_VAD_END:
			case SIREN_EVENT_WAKE_VAD_END:
				ALOGV("voice event   >>>   end        %x", callback);
				if(callback != NULL){
					//Modify the last callback of the state
					callback->event = RuntimeService::VOICE_STATE_END;
					callback = NULL;
					nlp = NULL;
				}
				break;
			case SIREN_EVENT_VAD_CANCEL:
			case SIREN_EVENT_WAKE_CANCEL:
				ALOGI("voice event  >>>   cancel    %x", callback);
				if(callback != NULL){
					//Modify the last callback of the state
					callback->event = RuntimeService::VOICE_STATE_CANCEL;
					callback = NULL;
					nlp = NULL;
				}
				break;
			case SIREN_EVENT_WAKE_PRE:
				ALOGV("vicee event  >>>   prepare");
				break;
		}
		runtime_service->voice_queue.pop_front();
		delete voice_msg;
		pthread_mutex_unlock(&runtime_service->siren_mutex);
	}
	ALOGV("thread quit!");
	return NULL;
}

void RuntimeService::MyNlpCallback::onNlp(int id, const char *nlp){
	ALOGI("native    >>>   %d", nlp);
	if(!runtime_service->mNlpCallback.empty()){
		map<int, RuntimeService::MyNlpCallback*>::iterator it = runtime_service->mNlpCallback.find(id);
		runtime_service->mNlpCallback.erase(it);
		//TODO delete the callback;
		RuntimeService::MyNlpCallback* callback = it->second;
		delete callback;
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
