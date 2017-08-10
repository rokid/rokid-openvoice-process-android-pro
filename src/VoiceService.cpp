#define LOG_TAG "VoiceService"
#define LOG_NDEBUG 0

#include <stdio.h>
#include <sys/prctl.h>
#include <functional>
#include <binder/IPCThreadState.h>

#include "VoiceService.h"
#include "engine.h"

#if defined(__ANDROID__) || defined(ANDROID)
#include "json.h"
#else
#include <json-c/json.h>
#endif

#ifdef USB_AUDIO_DEVICE
#warning "=============================USB_AUDIO_DEVICE==============================="
#endif

VoiceService::VoiceService() {
    pthread_mutex_init(&event_mutex, NULL);
    pthread_mutex_init(&speech_mutex, NULL);
    pthread_mutex_init(&siren_mutex, NULL);
    pthread_cond_init(&event_cond, NULL);
}

bool VoiceService::init() {
    pthread_mutex_lock(&siren_mutex);
    if(mCurrentSirenState == SIREN_STATE_UNKNOWN) {
        if(!_init_siren(this)) {
            ALOGE("init siren failed.");
            pthread_mutex_unlock(&siren_mutex);
            return false;
        }
    } else {
        goto done;
    }
    mCurrentSirenState = SIREN_STATE_INITED;
    if(!_speech.get())_speech = new_speech();
    pthread_create(&event_thread, NULL, ::onEvent, this);
done:
    pthread_mutex_unlock(&siren_mutex);
    return true;
}

void VoiceService::start_siren(bool flag) {
    pid_t pid = IPCThreadState::self()->getCallingPid();

    ALOGV("%s \t flag : %d \t mCurrState : %d \t opensiren : %d \t calling pid : %d", 
            __FUNCTION__, flag, mCurrentSirenState, openSiren, pid);

    pthread_mutex_lock(&siren_mutex);
    if(flag && (mCurrentSirenState == SIREN_STATE_INITED
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
    } else if(!flag && mCurrentSirenState == SIREN_STATE_STARTED) {
        _stop_siren_process_stream();
        mCurrentSirenState = SIREN_STATE_STOPED;
    }
    if(!flag && mCurrentSirenState != SIREN_STATE_UNKNOWN) openSiren = false;
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
    if(!_speech.get())_speech = new_speech();
    if(connected && mCurrentSpeechState != SPEECH_STATE_PREPARED) {
        this->config();
        if(_speech->prepare()) {
            mCurrentSpeechState = SPEECH_STATE_PREPARED;
            pthread_create(&response_thread, NULL, ::onResponse, this);
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
        ALOGV("=================================END==============================");
        mCurrentSpeechState = SPEECH_STATE_RELEASED;
    }
    pthread_mutex_unlock(&speech_mutex);
}

void VoiceService::send_voice_event(int event, int has_sl, double sl_degree, double energy, double threshold) {
    if(proxy.get()) {
        Parcel data, reply;
        data.writeInterfaceToken(proxy->getInterfaceDescriptor());
        data.writeInt32(event);
        data.writeInt32(has_sl);
        data.writeDouble(sl_degree);
        data.writeDouble(energy);
        data.writeDouble(threshold);
        proxy->transact(IBinder::FIRST_CALL_TRANSACTION + 1, data, &reply);
        reply.readExceptionCode();
    } else {
        ALOGI("Java service is null , Waiting for it to initialize");
    }
}

void VoiceService::update_stack(const string &appid) {
    this->appid = appid;
    ALOGE("appid  %s", this->appid.c_str());
}

int VoiceService::vad_start() {
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

void VoiceService::regist_callback(const sp<IBinder> &binder) {
    binder->linkToDeath(sp<DeathRecipient>(new VoiceService::DeathNotifier(this)));
    proxy = binder;
}

void VoiceService::config() {
    json_object *json_obj = json_object_from_file(SPEECH_CONFIG_FILE);

    if(json_obj == NULL) {
        ALOGE("%s cannot find", SPEECH_CONFIG_FILE);
        return;
    }
    json_object *host, *port, *branch, *ssl_roots_pem, *auth_key, *device_type, *device_id, *secret, *api_version, *codec;

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
    json_object_put(json_obj);
}

void* onEvent(void* args) {
    prctl(PR_SET_NAME, __FUNCTION__);
    VoiceService *service = (VoiceService*)args;
    int id = -1;
    for(;;) {
        pthread_mutex_lock(&service->event_mutex);
        while(service->message_queue.empty()) {
            pthread_cond_wait(&service->event_cond, &service->event_mutex);
        }
        voice_event_t *message = service->message_queue.front();
        service->message_queue.pop_front();
        pthread_mutex_unlock(&service->event_mutex);

        ALOGV("event : -------------------------%d----", message->event);

        if(!(message->event == SIREN_EVENT_VAD_DATA || message->event == SIREN_EVENT_WAKE_VAD_DATA)) {
            service->send_voice_event(message->event, HAS_SL(message->flag), message->sl, message->background_energy, message->background_threshold);
        }
        switch(message->event) {
            case SIREN_EVENT_WAKE_CMD:
                ALOGV("WAKE_CMD");
                break;
            case SIREN_EVENT_WAKE_NOCMD:
                ALOGV("WAKE_NOCMD");
                break;
            case SIREN_EVENT_SLEEP:
                ALOGV("SLEEP");
                break;
            case SIREN_EVENT_VAD_START:
            case SIREN_EVENT_WAKE_VAD_START:
                id = service->vad_start();
                ALOGV("VAD_START\t\t ID  :  <<%d>>", id);
                break;
            case SIREN_EVENT_VAD_DATA:
            case SIREN_EVENT_WAKE_VAD_DATA:
                if (id > 0 && HAS_VOICE(message->flag)) {
                    service->_speech->put_voice(id, (uint8_t *)message->buff, message->length);
                }
                break;
            case SIREN_EVENT_VAD_END:
            case SIREN_EVENT_WAKE_VAD_END:
                ALOGV("VAD_END\t\t ID  <<%d>> ", id);
                if(id > 0) {
                    service->_speech->end_voice(id);
                    id = -1;
                }
                break;
            case SIREN_EVENT_VAD_CANCEL:
            case SIREN_EVENT_WAKE_CANCEL:
                if(id > 0) {
                    service->_speech->cancel(id);
                    ALOGI("VAD_CANCEL\t\t ID   <<%d>>", id);
                    id = -1;
                }
                break;
            case SIREN_EVENT_VOICE_PRINT:
                service->voice_print(message);
                ALOGI("VOICE_PRINT");
                break;
        }
        free(message->buff);
        free(message);
    }
    service->_speech->release();
    service->_speech.reset();
    return NULL;
}

void* onResponse(void* args) {
    prctl(PR_SET_NAME, __FUNCTION__);
    VoiceService *service = (VoiceService*)args;
    function<bool(const string&)> arbitration = [](const string& activation){return ("fake" == activation || "reject" == activation);};
    SpeechResult sr;
    string activation;
    json_object *nlp_obj, *activation_obj;
    while(1) {
        if (!service->_speech->poll(sr)) {
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
                if(arbitration(activation)) {
                    set_siren_state_change(SIREN_STATE_SLEEP);
                    continue;
                }
                if(service->proxy.get()) {
                    Parcel data, reply;
                    data.writeInterfaceToken(service->proxy->getInterfaceDescriptor());
                    data.writeString16(String16(activation.c_str()));
                    service->proxy->transact(IBinder::FIRST_CALL_TRANSACTION + 2, data, &reply);
                    reply.readExceptionCode();
                }
            }
        }
        if(!arbitration(activation)) {
            if(sr.type == SPEECH_RES_END) {
                ALOGV("result : asr\t%s", sr.asr.c_str());
                ALOGV("result : nlp\t%s", sr.nlp.c_str());
                ALOGV("result : action  %s", sr.action.c_str());
                if(service->proxy.get()) {
                    Parcel data, reply;
                    data.writeInterfaceToken(service->proxy->getInterfaceDescriptor());
                    data.writeString16(String16(sr.asr.c_str()));
                    data.writeString16(String16(sr.nlp.c_str()));
                    data.writeString16(String16(sr.action.c_str()));
                    service->proxy->transact(IBinder::FIRST_CALL_TRANSACTION, data, &reply);
                    reply.readExceptionCode();
                } else {
                    ALOGI("Java service is null , Waiting for it to initialize");
                }
            } else if(sr.type == SPEECH_RES_ERROR && sr.err == SPEECH_TIMEOUT) {
                if(service->proxy.get()) {
                    Parcel data, reply;
                    data.writeInterfaceToken(service->proxy->getInterfaceDescriptor());
                    service->proxy->transact(IBinder::FIRST_CALL_TRANSACTION + 3, data, &reply);
                    reply.readExceptionCode();
                }
            }
        }
    }
    ALOGV("exit !!");
    return NULL;
}
