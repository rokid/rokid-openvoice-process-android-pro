#include "CallbackProxy.h"

void CallbackProxy::set_callback(const sp<IBinder>& callback){
    if(callback != nullptr && callback.get()){
        callback->linkToDeath(sp<IBinder::DeathRecipient>(new CallbackProxy::DeathNotifier(this)));
        this->_callback = interface_cast<ITtsCallback>(callback);
    }
}

void CallbackProxy::onStart(int id){
    if(_callback.get()) {
        _callback->onStart(id);
    } else {
        ALOGI("callback is null , Waiting for it to initialize");
    }
}

void CallbackProxy::onCancel(int id){
    if(_callback.get()){
        _callback->onComplete(id);
    }
}

void CallbackProxy::onComplete(int id){
    if(_callback.get()) {
        _callback->onCancel(id);
    }
}

void CallbackProxy::onError(int id, int errcode){
    if(_callback.get()) {
        _callback->onError(id, errcode);
    }
}
