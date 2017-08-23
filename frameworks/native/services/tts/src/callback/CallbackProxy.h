#ifndef CALLBACK_PROXY_H
#define CALLBACK_PROXY_H

#include <binder/IInterface.h>
#include <string>

#include "ITtsCallback.h"

using namespace android;
using namespace std;

class CallbackProxy{
private:

    sp<ITtsCallback>  _callback;

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

    void set_callback(const sp<IBinder>&);

	void onStart(int id);

	void onCancel(int id);

	void onComplete(int id);

	void onError(int id, int err);
};

#endif
