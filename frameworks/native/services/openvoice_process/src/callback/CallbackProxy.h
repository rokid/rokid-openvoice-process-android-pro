#ifndef CALLBACK_PROXY_H
#define CALLBACK_PROXY_H

#include <binder/IInterface.h>
#include <string>

#include "IVoiceCallback.h"

using namespace android;
using namespace std;

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

    CallbackProxy(){}
    CallbackProxy(const sp<IBinder>& callback){set_callback(callback);}
    ~CallbackProxy(){}

    void set_callback(const sp<IBinder>& callback);

    void voice_command(const string& asr, const string& nlp, const string& action);

    void voice_event(int event, bool has_sl, double sl, double energy, double threshold);

    void arbitration(const string& extra);

    void speech_error(int errcode);
};

#endif
