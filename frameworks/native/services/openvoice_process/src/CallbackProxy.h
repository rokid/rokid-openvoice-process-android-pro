#ifndef CALLBACK_PROXY_H
#define CALLBACK_PROXY_H

#include "IVoiceCallback.h"

class CallbackProxy{
private:

    sp<IVoiceCallback>  _callback;

    class DeathNotifier : public IBinder::DeathRecipient {
    public:
        DeathNotifier(CallbackProxy *proxy): _proxy(proxy){}
        ~DeathNotifier(){}
        void binderDied(const wp<IBinder> &binder) {
            _proxy->_callback.clear();
        }
    private:
        CallbackProxy *_proxy;
    };

public:

    void set_callback(const sp<IBinder>& callback);

    void voice_event(const int32_t id, const int32_t event, const double sl = 0.0, const double energy = 0.0);

    void intermediate_result(const int32_t id, const int32_t type, const string& asr);

    void voice_command(const int32_t id, const string& asr, const string& nlp, const string& action);

    void speech_error(const int32_t id, const int32_t errcode);

    const string get_skill_options();
};

#endif
