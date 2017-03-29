#define LOG_TAG "RuntimeService"
#define LOG_NDEBUG 0

#include "include/RuntimeService.h"
#include "include/VoiceEngine.h"
#include <binder/IServiceManager.h>
#include <stdio.h>
#include "json.h"

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

void RuntimeService::add_binder(sp<IBinder> binder){
	_binder = binder;	
	ALOGV("add_binder success %x", _binder.get());
}

RuntimeService::~RuntimeService(){
	free(voice_engine);
}

void* siren_thread_loop(void* arg){

	ALOGV("thread join !!");
	RuntimeService *runtime_service = (RuntimeService*)arg;
	int id = -1;

	bool err = false;;
	//FILE *fd = fopen("/data/voice.pcm", "w");
	runtime_service->_speech = new_speech();
	if (!runtime_service->_speech->prepare()) {
		ALOGE("=========prepare failed===============");
		return NULL;
	}
	runtime_service->_speech->config("codec", "opu");
	pthread_create(&runtime_service->speech_thread, NULL, speech_thread_loop, runtime_service);
	for(;;){
		pthread_mutex_lock(&runtime_service->siren_mutex);
		while(runtime_service->voice_queue.empty()){
			pthread_cond_wait(&runtime_service->siren_cond, &runtime_service->siren_mutex);
		}

		const RuntimeService::VoiceMessage *voice_msg = runtime_service->voice_queue.front();
		ALOGV("event : -------------------------%d----", voice_msg->event);
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
				id = runtime_service->_speech->start_voice();
				ALOGV("voice event   >>   start   id   :  <<%d>>     err : <<%d>>", id,  err);
				break;
			case SIREN_EVENT_VAD_DATA:
			case SIREN_EVENT_WAKE_VAD_DATA:
				if (id > 0 && voice_msg->has_voice > 0) {
					runtime_service->_speech->put_voice(id, (uint8_t *)voice_msg->buff, voice_msg->length);
					//fwrite(voice_msg->buff, voice_msg->length, 1, fd);
				}
				break;
			case SIREN_EVENT_VAD_END:
			case SIREN_EVENT_WAKE_VAD_END:
				ALOGV("voice event : end   id    >>>   %d ",id);
				if(id > 0) {
					runtime_service->_speech->end_voice(id);
					//fclose(fd);
				}
				break;
			case SIREN_EVENT_VAD_CANCEL:
			case SIREN_EVENT_WAKE_CANCEL:
				if(id > 0)
					runtime_service->_speech->cancel(id);
				ALOGI("voice event : cancel   id    >>>    %d", id);
				break;
			case SIREN_EVENT_WAKE_PRE:
				ALOGV("vicee event  >>>   prepare");
				break;
		}
		runtime_service->voice_queue.pop_front();
		delete voice_msg;
		pthread_mutex_unlock(&runtime_service->siren_mutex);
	}
	runtime_service->_speech->release();
	delete runtime_service->_speech;
	ALOGV("thread quit!");
	return NULL;
}

void* speech_thread_loop(void* arg){
	RuntimeService *runtime_service = (RuntimeService*)arg;
	json_object *_json_obj = NULL;
	//sp<IBinder> binder = defaultServiceManager()->getService(String16("runtime_java"));
	for(;;){
		SpeechResult sr;
		int32_t flag = runtime_service->_speech->poll(sr);
		if (flag < 0)
			break;

		ALOGV("result : asr  >>  %s    %d", sr.asr.c_str(), flag);
		ALOGV("result : nlp  >>  %s", sr.nlp.c_str());
		ALOGV("result : action >>  %s", sr.action.c_str());

		if(flag == 0 && sr.nlp != ""){
			_json_obj = json_tokener_parse("{}");
			json_object_object_add(_json_obj, "nlp", json_object_new_string(sr.nlp.c_str()));
			//_json_obj = json_tokener_parse(sr.nlp.c_str());
			json_object_object_add(_json_obj, "asr", json_object_new_string(sr.asr.c_str()));
			json_object_object_add(_json_obj, "action", json_object_new_string(sr.action.c_str()));
			ALOGV("-------------------------------------------------------------------------");
			ALOGV("%s", json_object_to_json_string(_json_obj));
			ALOGV("-------------------------------------------------------------------------");
			if(runtime_service->_binder != NULL){
				Parcel data, reply;
				data.writeInterfaceToken(String16("rokid.os.IRuntimeService"));
				data.writeString16(String16(json_object_to_json_string(_json_obj)));
				runtime_service->_binder->transact(IBinder::FIRST_CALL_TRANSACTION + 0, data, &reply);
				reply.readExceptionCode();
			}else{
				ALOGI("java runtime is null , Waiting for it to initialize");
			}
			delete _json_obj;
			_json_obj = NULL;
		}
	}
	return NULL;
}

//void RuntimeService::MyAsrCallback::onData(int id, const char *text){
//	ALOGI("native    >>>   %d", text);
//	if(!runtime_service->mAsrCallback.empty()){
//		map<int, RuntimeService::MyAsrCallback*>::iterator it = runtime_service->mAsrCallback.find(id);
//		runtime_service->mAsrCallback.erase(id);
//		//TODO delete the callback;
//		RuntimeService::MyAsrCallback* callback = it->second;
//		delete callback;
//	}
//}
//
//void RuntimeService::MyAsrCallback::onComplete(int id){
//	ALOGV("callback onComplete   >>>    %d    >>>   this    >>>    %x", id, this);
//	runtime_service->mAsrCallback.insert(pair<int, RuntimeService::MyAsrCallback*>(id, this));
//}
