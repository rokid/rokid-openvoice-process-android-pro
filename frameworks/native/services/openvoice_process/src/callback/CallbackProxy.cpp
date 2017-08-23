#include "CallbackProxy.h"

void CallbackProxy::set_callback(const sp<IBinder>& callback){
    if(callback != nullptr && callback.get()){
        callback->linkToDeath(sp<IBinder::DeathRecipient>(new CallbackProxy::DeathNotifier(this)));
        this->_callback = interface_cast<IVoiceCallback>(callback);
    }
}

void CallbackProxy::voice_command(const string& asr, const string& nlp, const string& action){
    if(_callback.get()) {
        _callback->voice_command(asr, nlp, action);
    } else {
        ALOGI("Java service is null , Waiting for it to initialize");
    }
}

void CallbackProxy::voice_event(int event, bool has_sl, double sl, double energy, double threshold){
    if(_callback.get()){
        _callback->voice_event(event, has_sl, sl, energy, threshold);
    }
}

void CallbackProxy::arbitration(const string& extra){
    if(_callback.get()) {
        _callback->arbitration(extra);
    }
}

void CallbackProxy::speech_error(int errcode){
    if(_callback.get()) {
         _callback->speech_error(errcode);
     }
}
