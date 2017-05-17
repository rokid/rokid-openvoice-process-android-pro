#define LOG_TAG "RuntimeService"
#define LOG_NDEBUG 0

#include <stdio.h>
#include <utils/String8.h>
#include <binder/IServiceManager.h>

#include "RuntimeService.h"
#include "voice.h"
#include "json.h"

using namespace android;
using namespace std;
using namespace rokid;
using namespace speech;

RuntimeService::RuntimeService(){
	pthread_mutex_init(&event_mutex, NULL);
	pthread_mutex_init(&speech_mutex, NULL);
	pthread_cond_init(&event_cond, NULL);
}

bool RuntimeService::init() {
    if(!_init(this)) {
        ALOGE("init siren failed.");
        return false;
    }
    if(_speech == NULL)_speech = new_speech();
    pthread_create(&event_thread, NULL, onEvent, this);
    return true;
}

void RuntimeService::start_siren(bool flag) {
	if(flag){
		start_siren_process_stream();
	}else{
		stop_siren_stream();
	}
}

void RuntimeService::set_siren_state(const int &state) {
    set_siren_state_change(state);
    ALOGV("current_status     >>   %d", state);
}

void RuntimeService::network_state_change(bool connected) {
    ALOGV("network_state_change      isconnect  <<%d>>", connected);
    //pthread_mutex_lock(&speech_mutex);
    if(pthread_mutex_trylock(&speech_mutex) == EBUSY) {
        ALOGE("blocking 。。。");
        return;
    }
    if(_speech == NULL)_speech = new_speech();
    if(connected && !prepared) {
        this->config();
        if(_speech->prepare()) {
            prepared = true;
			pthread_create(&response_thread, NULL, onResponse, this);
			pthread_detach(response_thread);
			start_siren_process_stream();
        }
    } else if(!connected && prepared) {
		stop_siren_stream();
        _speech->release();
        prepared = false;
    }
    pthread_mutex_unlock(&speech_mutex);
}

void RuntimeService::send_siren_event(int event, double sl_degree, int has_sl){
	if(remote != NULL){
		Parcel data, reply;
		data.writeInterfaceToken(String16("com.rokid.openvoice.IRuntimeService"));
		data.writeInt32(event);
		data.writeDouble(sl_degree);
		data.writeInt32(has_sl);
		remote->transact(IBinder::FIRST_CALL_TRANSACTION + 1, data, &reply);
		reply.readExceptionCode();
	}else{
		ALOGI("Java runtime is null , Waiting for it to initialize");
	}
}

void RuntimeService::update_stack(String16 curr_appid, String16 prev_appid){
	if(_speech != NULL && prepared){
		if(curr_appid.size() > 0){
			String8 curr_appid8(curr_appid);
			ALOGE("curr_appid  %s", curr_appid8.string());
			_speech->config("stack", curr_appid8.string());
		}else{
			_speech->config("stack", "");
		}
	}
}

void RuntimeService::add_binder(sp<IBinder> binder){
	remote = binder;
}

void RuntimeService::config() {
    json_object *json_obj = json_object_from_file(SPEECH_CONFIG_FILE);

    if(json_obj == NULL) {
        ALOGE("%s cannot find", SPEECH_CONFIG_FILE);
		return;
    }
    json_object *server_address = NULL;
    json_object *ssl_roots_pem = NULL;
    json_object *auth_key = NULL;
    json_object *device_type = NULL;
    json_object *device_id = NULL;
    json_object *secret = NULL;
    json_object *api_version = NULL;
    json_object *codec = NULL;

    if(TRUE == json_object_object_get_ex(json_obj, "server_address", &server_address)) {
        _speech->config("server_address", json_object_get_string(server_address));
        ALOGE("%s", json_object_get_string(server_address));
    }
    if(TRUE == json_object_object_get_ex(json_obj, "ssl_roots_pem", &ssl_roots_pem)) {
        _speech->config("ssl_roots_pem", json_object_get_string(ssl_roots_pem));
        ALOGE("%s", json_object_get_string(ssl_roots_pem));
    }
    if(TRUE == json_object_object_get_ex(json_obj, "key", &auth_key)) {
        _speech->config("key", json_object_get_string(auth_key));
        ALOGE("%s", json_object_get_string(auth_key));
    }
    if(TRUE == json_object_object_get_ex(json_obj, "device_type_id", &device_type)) {
        _speech->config("device_type_id", json_object_get_string(device_type));
        ALOGE("%s", json_object_get_string(device_type));
    }
    if(TRUE == json_object_object_get_ex(json_obj, "device_id", &device_id)) {
        _speech->config("device_id", json_object_get_string(device_id));
        ALOGE("%s", json_object_get_string(device_id));
    }
    if(TRUE == json_object_object_get_ex(json_obj, "api_version", &api_version)) {
        _speech->config("api_version", json_object_get_string(api_version));
        ALOGE("%s", json_object_get_string(api_version));
    }
    if(TRUE == json_object_object_get_ex(json_obj, "secret", &secret)) {
        _speech->config("secret", json_object_get_string(secret));
        ALOGE("%s", json_object_get_string(secret));
    }
    if(TRUE == json_object_object_get_ex(json_obj, "codec", &codec)) {
        _speech->config("codec", json_object_get_string(codec));
        ALOGE("%s", json_object_get_string(codec));
    }
    _speech->config("vt", "若琪");
    json_object_put(json_obj);
}

void* onEvent(void* arg) {
    RuntimeService *runtime = (RuntimeService*)arg;
    int id = -1;
//	runtime->config();
//	if (!runtime->_speech->prepare()) {
//		return NULL;
//	}
//	runtime->prepared = true;
//	pthread_create(&runtime->response_thread, NULL, onResponse, runtime);
    for(;;) {
        pthread_mutex_lock(&runtime->event_mutex);
        while(runtime->message_queue.empty()) {
            pthread_cond_wait(&runtime->event_cond, &runtime->event_mutex);
        }
        const RuntimeService::VoiceMessage *message = runtime->message_queue.front();
        ALOGV("event : -------------------------%d----", message->event);

		if(!(message->event == SIREN_EVENT_VAD_DATA || message->event == SIREN_EVENT_WAKE_VAD_END)){
			runtime->send_siren_event(message->event, message->sl_degree, message->has_sl);
		}
        switch(message->event) {
        case SIREN_EVENT_WAKE_CMD:
            break;
        case SIREN_EVENT_WAKE_NOCMD:
            ALOGV("WAKE_NOCMD");
            break;
        case SIREN_EVENT_SLEEP:
            ALOGV("SLEEP");
            break;
        case SIREN_EVENT_VAD_START:
        case SIREN_EVENT_WAKE_VAD_START:
            id = runtime->_speech->start_voice();
            ALOGV("VAD_START\t\t ID  :  <<%d>>", id);
            break;
        case SIREN_EVENT_VAD_DATA:
        case SIREN_EVENT_WAKE_VAD_DATA:
            if (id > 0 && message->has_voice > 0) {
                runtime->_speech->put_voice(id, (uint8_t *)message->buff, message->length);
            }
            break;
        case SIREN_EVENT_VAD_END:
        case SIREN_EVENT_WAKE_VAD_END:
            ALOGV("VAD_END\t\t ID  <<%d>> ", id);
            if(id > 0) {
                runtime->_speech->end_voice(id);
                id = -1;
            }
            break;
        case SIREN_EVENT_VAD_CANCEL:
        case SIREN_EVENT_WAKE_CANCEL:
            if(id > 0) {
                runtime->_speech->cancel(id);
                ALOGI("VAD_CANCEL\t\t ID   <<%d>>", id);
                id = -1;
            }
            break;
        case SIREN_EVENT_WAKE_PRE:
            break;
        }
        runtime->message_queue.pop_front();
        delete message;
        pthread_mutex_unlock(&runtime->event_mutex);
    }
    runtime->_speech->release();
    delete_speech(runtime->_speech);
    return NULL;
}

void* onResponse(void* arg) {
    RuntimeService *runtime = (RuntimeService*)arg;
    SpeechResult sr;
	//runtime->remote = defaultServiceManager()->getService(String16("runtime_java"));

    for(;;) {
        bool res = runtime->_speech->poll(sr);
        if (!res) {
			break;
        }
        ALOGV("result : asr  >>  %s    <<%d>>     <<%d>>", sr.asr.c_str(), sr.type, sr.err);
        ALOGV("result : nlp  >>  %s", sr.nlp.c_str());
        ALOGV("result : action >>  %s", sr.action.c_str());

        if(sr.type == 0 && !sr.nlp.empty()) {
			if(runtime->remote != NULL){
				Parcel data, reply;
				data.writeInterfaceToken(String16("com.rokid.openvoice.IRuntimeService"));
				data.writeString16(String16(sr.asr.c_str()));
				data.writeString16(String16(sr.nlp.c_str()));
				data.writeString16(String16(sr.action.c_str()));
				data.writeInt32(sr.type);
				runtime->remote->transact(IBinder::FIRST_CALL_TRANSACTION, data, &reply);
				reply.readExceptionCode();
			}else{
				ALOGI("Java runtime is null , Waiting for it to initialize");
			}
        }
    }
	ALOGV("exit !!");
    return NULL;
}
