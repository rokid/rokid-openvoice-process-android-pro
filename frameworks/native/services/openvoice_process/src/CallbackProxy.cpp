#include "CallbackProxy.h"

void CallbackProxy::set_callback(const sp<IBinder>& callback){
    if(callback != nullptr && callback.get()){
        callback->linkToDeath(sp<IBinder::DeathRecipient>(new CallbackProxy::DeathNotifier(this)));
        this->_callback = interface_cast<IVoiceCallback>(callback);
    }
}

void CallbackProxy::voice_event(const int32_t id, const int32_t event, const double sl, const double energy){
    if(_callback.get()){
        _callback->voice_event(id, event, sl, energy);
    }
}

void CallbackProxy::intermediate_result(const int32_t id, const int32_t type, const string& asr){
    if(_callback.get()) {
         _callback->intermediate_result(id, type, asr);
     }
}

void CallbackProxy::voice_command(const int32_t id, const string& asr, const string& nlp, const string& action){
    if(_callback.get()) {
        _callback->voice_command(id, asr, nlp, action);
    } else {
        ALOGI("Java service is null , Waiting for it to initialize");
    }
}

void CallbackProxy::speech_error(const int32_t id, const int32_t errcode){
    if(_callback.get()) {
         _callback->speech_error(id, errcode);
     }
}

const string CallbackProxy::get_skill_options(){
    if(_callback.get()) {
         return _callback->get_skill_options();
     }
    return string();
}
