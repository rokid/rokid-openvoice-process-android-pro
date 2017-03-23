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
	RuntimeService::MyAsrCallback *callback = NULL;
	Asr *asr = NULL;
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
				runtime_service->current_status = SIREN_STATE_AWAKE;
				id = -1;
				asr = NULL;
				callback = NULL;
				asr = new Asr();
				callback = new RuntimeService::MyAsrCallback(runtime_service, asr);
				asr->prepare();
				id = asr->start(callback);
				ALOGV("voice event : start   id   >>>   %d   callback   >>>   %x", id, callback);
				break;
			case SIREN_EVENT_VAD_DATA:
			case SIREN_EVENT_WAKE_VAD_DATA:
				asr->voice(id, (unsigned char *)voice_msg->buff, voice_msg->length);
				break;
			case SIREN_EVENT_VAD_END:
			case SIREN_EVENT_WAKE_VAD_END:
				ALOGV("voice event : end   id    >>>   %d   callback  >>   %x",id,  callback);
				if(asr && id > 0)
					asr->end(id);
				break;
			case SIREN_EVENT_VAD_CANCEL:
			case SIREN_EVENT_WAKE_CANCEL:
				if(asr && id > 0)
					asr->cancel(id);
				ALOGI("voice event : cancel   id    >>>    %d    callback   >>   %x", id,  callback);
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

void RuntimeService::MyAsrCallback::onData(int id, const char *text){
	ALOGI("native    >>>   %d", text);

	sp<IBinder> binder = defaultServiceManager()->getService(String16("runtime_java"));
	if(binder != NULL){
		Parcel data, reply;
		data.writeInterfaceToken(String16("rokid.os.IRuntimeService"));
		data.writeString16(String16(text));
		binder->transact(IBinder::FIRST_CALL_TRANSACTION + 0, data, &reply);
		reply.readExceptionCode();
	}else{
		ALOGI("java runtime is null , Waiting for it to initialize");
	}
	if(!runtime_service->mAsrCallback.empty()){
		map<int, RuntimeService::MyAsrCallback*>::iterator it = runtime_service->mAsrCallback.find(id);
		runtime_service->mAsrCallback.erase(id);
		//TODO delete the callback;
		RuntimeService::MyAsrCallback* callback = it->second;
		delete callback;
	}
}

void RuntimeService::MyAsrCallback::onComplete(int id){
	ALOGV("callback onComplete   >>>    %d    >>>   this    >>>    %x", id, this);
	runtime_service->mAsrCallback.insert(pair<int, RuntimeService::MyAsrCallback*>(id, this));
}

void RuntimeService::MyAsrCallback::onStart(int id){
	ALOGI("callback onStart    >>>   %d    this   >>>    %x", id, this);	
}

void RuntimeService::MyAsrCallback::onStop(int id){
	ALOGE("callback onStop    >>>   %d    this   >>>   %x", id, this);
	delete this;
}

void RuntimeService::MyAsrCallback::onError(int id, int err){
	ALOGE("callback  error :  %d,   id  :  %d", err, id);
	delete this;
}
