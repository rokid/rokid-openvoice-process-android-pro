#define LOG_TAG "RuntimeService"
#define LOG_NDEBUG 0

#include <stdio.h>
#include <unistd.h>
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
	pthread_mutex_init(&siren_mutex, NULL);
	pthread_cond_init(&event_cond, NULL);
}

bool RuntimeService::init() {
    pthread_mutex_lock(&siren_mutex);
    if(mCurrentSirenState == SIREN_STATE_UNKNOW){
        if(!_init(this)) {
            ALOGE("init siren failed.");
            pthread_mutex_unlock(&siren_mutex);
            return false;
        }
    }else{
        goto done;
    }
    mCurrentSirenState = SIREN_STATE_INITED;
    if(_speech == NULL)_speech = new_speech();
    pthread_create(&event_thread, NULL, onEvent, this);
done:
    pthread_mutex_unlock(&siren_mutex);
    return true;
}

void RuntimeService::start_siren(bool flag) {
    ALOGV("%s \t %d", __FUNCTION__, flag);
    pthread_mutex_lock(&siren_mutex);
	if(flag && (mCurrentSirenState == SIREN_STATE_INITED
            || mCurrentSirenState == SIREN_STATE_STOPED)){
//            && find_card("PawPaw Microphone") > 0){
        openSiren = true;
//        if(wait_for_alsa_usb_card()){
		    start_siren_process_stream();
            mCurrentSirenState = SIREN_STATE_STARTED;
//       }
	}else if(!flag && mCurrentSirenState == SIREN_STATE_STARTED){
		stop_siren_process_stream();
        openSiren = false;
        mCurrentSirenState = SIREN_STATE_STOPED;
	}
    pthread_mutex_unlock(&siren_mutex);
}

bool RuntimeService::wait_for_alsa_usb_card(){
    int index = 0;
    while (index++ < 3){
        if(find_card("PawPaw Microphone") > 0){
            return true;
        }
        usleep(1000 * 100);
    }
    return false;
}

void RuntimeService::set_siren_state(const int &state) {
    set_siren_state_change(state);
    ALOGV("current_status     >>   %d", state);
}

void RuntimeService::network_state_change(bool connected) {
    ALOGV("network_state_change      isconnect  <<%d>>", connected);
    pthread_mutex_lock(&speech_mutex);
//    if(pthread_mutex_trylock(&speech_mutex) == EBUSY) {
//        ALOGE("blocking 。。。");
//        return;
//    }
    if(_speech == NULL)_speech = new_speech();
    if(connected && mCurrentSpeechState != SPEECH_STATE_PREPARED) {
        this->config();
        if(_speech->prepare()) {
            mCurrentSpeechState = SPEECH_STATE_PREPARED;
	        pthread_create(&response_thread, NULL, onResponse, this);
	        pthread_detach(response_thread);

            pthread_mutex_lock(&siren_mutex);
	        if(openSiren && (mCurrentSirenState == SIREN_STATE_INITED
                    || mCurrentSirenState == SIREN_STATE_STOPED)){
                    //&& find_card("PawPaw Microphone") > 0){
	    	    start_siren_process_stream();
                mCurrentSirenState = SIREN_STATE_STARTED;
            }
            pthread_mutex_unlock(&siren_mutex);
        }
    } else if(!connected && mCurrentSpeechState == SPEECH_STATE_PREPARED) {
        pthread_mutex_lock(&siren_mutex);
	    if(mCurrentSirenState == SIREN_STATE_STARTED){
		    stop_siren_process_stream();
            mCurrentSirenState = SIREN_STATE_STOPED;
        }
        pthread_mutex_unlock(&siren_mutex);

        _speech->release();
        mCurrentSpeechState = SPEECH_STATE_RELEASED;
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

void RuntimeService::update_stack(String16 appid){
//	if(_speech != NULL && mCurrentSpeechState == SPEECH_STATE_PREPARED){
//		if(appid.size() > 0){
//			String8 appid8(appid);
//			ALOGE("appid  %s", appid8.string());
//			_speech->config("stack", appid8.string());
//		}else{
//			_speech->config("stack", "");
//		}
//	}
}

void RuntimeService::add_binder(sp<IBinder> binder){
    binder->linkToDeath(new RuntimeService::DeathNotifier(this));
	remote = binder;
}

void RuntimeService::config() {
    json_object *json_obj = json_object_from_file(SPEECH_CONFIG_FILE);

    if(json_obj == NULL) {
        ALOGE("%s cannot find", SPEECH_CONFIG_FILE);
		return;
    }
    json_object *host = NULL;
    json_object *port = NULL;
    json_object *branch = NULL;
    json_object *ssl_roots_pem = NULL;
    json_object *auth_key = NULL;
    json_object *device_type = NULL;
    json_object *device_id = NULL;
    json_object *secret = NULL;
    json_object *api_version = NULL;
    json_object *codec = NULL;

    if(TRUE == json_object_object_get_ex(json_obj, "host", &host)) {
        _speech->config("host", json_object_get_string(host));
        ALOGE("%s", json_object_get_string(host));
    }
    if(TRUE == json_object_object_get_ex(json_obj, "port", &port)) {
        _speech->config("port", json_object_get_string(port));
        ALOGE("%s", json_object_get_string(port));
    }
    if(TRUE == json_object_object_get_ex(json_obj, "branch", &branch)) {
        _speech->config("branch", json_object_get_string(branch));
        ALOGE("%s", json_object_get_string(branch));
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

void* onEvent(void* args) {
    RuntimeService *runtime = (RuntimeService*)args;
    int id = -1;
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
            ALOGV("WAKE_CMD");
            break;
        case SIREN_EVENT_WAKE_NOCMD:
            ALOGV("WAKE_NOCMD");
            break;
        case SIREN_EVENT_SLEEP:
            ALOGV("EVENT_SLEEP");
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

void* onResponse(void* args) {
    RuntimeService *runtime = (RuntimeService*)args;
    SpeechResult sr;
    for(;;) {
        bool res = runtime->_speech->poll(sr);
        if (!res) {
			break;
        }
        ALOGV("result : type \t %d \t err \t %d \t id \t %d", sr.type, sr.err, sr.id);
        if(sr.type == 0 && !sr.asr.empty()) {
            ALOGV("result : asr\t%s", sr.asr.c_str());
            ALOGV("result : asr\t%s", sr.nlp.c_str());
            ALOGV("result : asr\t%s", sr.action.c_str());
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
