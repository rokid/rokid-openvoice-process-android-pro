#define LOG_TAG "VoiceService"
#define LOG_NDEBUG 0

#include <stdio.h>
#include <sys/prctl.h>
//#include <functional>
#include <binder/IPCThreadState.h>

#include "VoiceService.h"
#include "audio_recorder.h"

#ifdef USB_AUDIO_DEVICE
#warning "=============================USB_AUDIO_DEVICE==============================="
#endif

VoiceService::VoiceService() {
    pthread_mutex_init(&event_mutex, NULL);
    pthread_mutex_init(&speech_mutex, NULL);
    pthread_mutex_init(&siren_mutex, NULL);
    pthread_cond_init(&event_cond, NULL);

    voice_config = make_shared<VoiceConfig>();
    callback = make_shared<CallbackProxy>();
    _speech = new_speech();
}

bool VoiceService::setup() {
    pthread_mutex_lock(&siren_mutex);
    if(mCurrentSirenState == SIREN_STATE_UNKNOWN) {
		if (!init(this,
				[](void *token, voice_event_t *event) {((VoiceService*)token)->voice_event_callback(event);})) {
            ALOGE("init siren failed.");
            pthread_mutex_unlock(&siren_mutex);
            return false;
        }
    } else {
        goto done;
    }
    mCurrentSirenState = SIREN_STATE_INITED;
	pthread_create(&event_thread, NULL,
			[](void* token)->void* {return ((VoiceService*)token)->onEvent();},
			this);
done:
    pthread_mutex_unlock(&siren_mutex);
    return true;
}

void VoiceService::start_siren(bool isopen) {
    pid_t pid = IPCThreadState::self()->getCallingPid();

    ALOGV("%s \t isopen : %d \t mCurrState : %d \t opensiren : %d \t calling pid : %d", 
            __FUNCTION__, isopen, mCurrentSirenState, openSiren, pid);

    pthread_mutex_lock(&siren_mutex);
    if(isopen && (mCurrentSirenState == SIREN_STATE_INITED
                || mCurrentSirenState == SIREN_STATE_STOPED)) {
        openSiren = true;
#ifdef USB_AUDIO_DEVICE
        if(wait_for_alsa_usb_card()) {
#endif
            _start_siren_process_stream();
            mCurrentSirenState = SIREN_STATE_STARTED;
#ifdef USB_AUDIO_DEVICE
        }
#endif
    } else if(!isopen && mCurrentSirenState == SIREN_STATE_STARTED) {
        _stop_siren_process_stream();
        mCurrentSirenState = SIREN_STATE_STOPED;
    }
    if(!isopen && mCurrentSirenState != SIREN_STATE_UNKNOWN) openSiren = false;
    pthread_mutex_unlock(&siren_mutex);
}

#ifdef USB_AUDIO_DEVICE
bool VoiceService::wait_for_alsa_usb_card() {
    int index = 0;
    while (index++ < 3) {
        if(find_card("USB-Audio")) {
            return true;
        }
        usleep(1000 * 100);
    }
    return false;
}
#endif

void VoiceService::set_siren_state(const int state) {
    set_siren_state_change(state);
    ALOGV("current_status     >>   %d", state);
}

void VoiceService::network_state_change(bool connected) {
    ALOGV("network_state_change      isconnect  <<%d>>", connected);
    pthread_mutex_lock(&speech_mutex);
    if(connected && mCurrentSpeechState != SPEECH_STATE_PREPARED) {
        if(voice_config->config(
                    [&](const char* key, const char* value){_speech->config(key, value);}) 
                    && _speech->prepare()) {
            mCurrentSpeechState = SPEECH_STATE_PREPARED;
			pthread_create(&response_thread, NULL,
					[](void* token)->void* {return ((VoiceService*)token)->onResponse();},
					this);
            pthread_detach(response_thread);

            pthread_mutex_lock(&siren_mutex);
            if(openSiren && (mCurrentSirenState == SIREN_STATE_INITED
                        || mCurrentSirenState == SIREN_STATE_STOPED)) {
#ifdef USB_AUDIO_DEVICE
                if(find_card("USB-Audio")) {
#endif
                    _start_siren_process_stream();
                    mCurrentSirenState = SIREN_STATE_STARTED;
#ifdef USB_AUDIO_DEVICE
                }
#endif
            }
            pthread_mutex_unlock(&siren_mutex);
        }
    } else if(!connected && mCurrentSpeechState == SPEECH_STATE_PREPARED) {
        pthread_mutex_lock(&siren_mutex);
        if(mCurrentSirenState == SIREN_STATE_STARTED) {
            _stop_siren_process_stream();
            mCurrentSirenState = SIREN_STATE_STOPED;
        }
        pthread_mutex_unlock(&siren_mutex);
        ALOGV("===============================BEGIN================================");
        _speech->release();
        ALOGV("================================END=================================");
        mCurrentSpeechState = SPEECH_STATE_RELEASED;
    }
    pthread_mutex_unlock(&speech_mutex);
}

void VoiceService::update_stack(const string &appid) {
    this->appid = appid;
    ALOGE("%s  %s", __FUNCTION__, this->appid.c_str());
}

void VoiceService::update_config(const string& device_id, const string& device_type_id,
                                const string& key, const string& secret) {

    if(!voice_config->save_config(device_id, device_type_id, key, secret)){

    }
}

int32_t VoiceService::vad_start() {
    if(mCurrentSpeechState == SPEECH_STATE_PREPARED) {
        shared_ptr<Options> options = new_options();
        if(options.get() && has_vt) {
            options->set("voice_trigger", vt_data.c_str());
            options->set("trigger_start", to_string(vt_start).c_str());
            options->set("trigger_length", to_string(vt_end - vt_start).c_str());
            options->set("voice_power", to_string(vt_energy).c_str());
            has_vt = false;
        }
        options->set("stack", appid.empty() ? "" : appid.c_str());
        string json;
        options->to_json_string(json);
        ALOGV("%s \t %s", __FUNCTION__, json.c_str());
        return _speech->start_voice(options);
    }
    return -1;
}

void VoiceService::voice_print(const voice_event_t *voice_event) {
    if(voice_event && HAS_VT(voice_event->flag)) {
        vt_start = voice_event->vt.start;
        vt_end = voice_event->vt.end;
        vt_energy = voice_event->vt.energy;
        vt_data = (char*)voice_event->buff;
        has_vt = true;
    }
}

void VoiceService::regist_callback(const sp<IBinder>& callback) {
    this->callback->set_callback(callback);
}

void VoiceService::voice_event_callback(voice_event_t *voice_event) {
	pthread_mutex_lock(&event_mutex);

    int32_t len =  0;
    char *buff = nullptr;
    if(voice_event->length > 0){
        len = voice_event->length;
    }
    char *temp = new char[sizeof(voice_event_t) + len];
    if(len) buff = temp + sizeof(voice_event_t);
    
    voice_event_t *_event_t = (voice_event_t*)temp;
    memcpy(_event_t, voice_event, sizeof(voice_event_t));
    
    if ((HAS_VOICE(_event_t->flag) || HAS_VT(_event_t->flag)) && len) {
    	memcpy(buff, voice_event->buff, len);
    	_event_t->buff = buff;
    }
    _events.push_back(_event_t);
	pthread_cond_signal(&event_cond);
	pthread_mutex_unlock(&event_mutex);
}

void* VoiceService::onEvent() {
    prctl(PR_SET_NAME, __FUNCTION__);
    int32_t id = -1;
    while(true) {
        pthread_mutex_lock(&event_mutex);
        while(_events.empty()) {
            pthread_cond_wait(&event_cond, &event_mutex);
        }
        voice_event_t *_event = _events.front();
        _events.pop_front();
        pthread_mutex_unlock(&event_mutex);

        ALOGV("event : -------------------------%d----", _event->event);

        callback->voice_event(_event->event, HAS_SL(_event->flag), _event->sl, 
                                    _event->background_energy, _event->background_threshold);

        switch(_event->event) {
            case SIREN_EVENT_VAD_START:
                id = vad_start();
                ALOGV("VAD_START\t\t ID  :  <<%d>>", id);
                break;
            case SIREN_EVENT_VAD_DATA:
                if (id > 0 && HAS_VOICE(_event->flag))
                    _speech->put_voice(id, (uint8_t *)_event->buff, _event->length);
                break;
            case SIREN_EVENT_VAD_END:
                ALOGV("VAD_END\t\t ID  :   <<%d>> ", id);
                if(id > 0) _speech->end_voice(id);
                id = -1;
                break;
            case SIREN_EVENT_VAD_CANCEL:
                if(id > 0) _speech->cancel(id);
                id = -1;
                ALOGI("VAD_CANCEL\t\t ID  :   <<%d>>", id);
                break;
            case SIREN_EVENT_VOICE_PRINT:
                voice_print(_event);
                break;
        }
		delete[] (char *)_event;
    }
    _speech->release();
    _speech.reset();
    return NULL;
}

void* VoiceService::onResponse() {
    prctl(PR_SET_NAME, __FUNCTION__);
    auto arbitration = [](const string& activation)->bool{return ("fake" == activation || "reject" == activation);};
    SpeechResult sr;
    string activation;
    json_object *nlp_obj, *activation_obj;
    while(true) {
        if (!_speech->poll(sr)) {
            break;
        }
        ALOGV("result : type \t %d \t err \t %d \t id \t %d", sr.type, sr.err, sr.id);
        if(sr.type == SPEECH_RES_START) {
            activation.clear();
        } else if((sr.type == SPEECH_RES_INTER || sr.type == SPEECH_RES_END) && !sr.extra.empty()) {
            nlp_obj = json_tokener_parse(sr.extra.c_str());
            if(TRUE == json_object_object_get_ex(nlp_obj, "activation", &activation_obj)){
                activation = json_object_get_string(activation_obj);
                json_object_put(nlp_obj);
                ALOGV("result : activation %s", activation.c_str());
                callback->arbitration(activation);
                if(arbitration(activation)) {
                    set_siren_state_change(SIREN_STATE_SLEEP);
                    continue;
                }
            }
        }
        if(!arbitration(activation)) {
            if(sr.type == SPEECH_RES_END) {
                ALOGV("result : asr\t%s", sr.asr.c_str());
                ALOGV("result : nlp\t%s", sr.nlp.c_str());
                ALOGV("result : action  %s", sr.action.c_str());
                callback->voice_command(sr.asr, sr.nlp, sr.action);
            } else if(sr.type == SPEECH_RES_ERROR && (sr.err == SPEECH_TIMEOUT || sr.err == SPEECH_SERVER_INTERNAL)) {
                callback->speech_error(sr.err);
            }
        }
    }
    ALOGV("exit !!");
    return NULL;
}
