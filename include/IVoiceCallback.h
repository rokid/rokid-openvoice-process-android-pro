#ifndef VOICE_CALLBACK_H
#define VOICE_CALLBACK_H

#define DESCRIPTOR "com.rokid.openvoice.callback"

#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <utils/String8.h>
#include <string>

using namespace android;
using namespace std;

enum {
    TRANSACTION_VOICE_COMMAND = IBinder::FIRST_CALL_TRANSACTION + 0,
    TRANSACTION_VOICE_EVENT,
    TRANSACTION_VOICE_REJECT,
    TRANSACTION_SPEECH_TIMEOUT,
};

class IVoiceCallback: public IInterface {
public:
    DECLARE_META_INTERFACE(VoiceCallback);
    virtual void voice_command(const string&, const string&, const string&) = 0;
    virtual void voice_event(int, bool, double, double, double) = 0;
    virtual void voice_reject() = 0;
    virtual void speech_timeout() = 0;
};

class BnVoiceCallback : public BnInterface<IVoiceCallback> {
public:
    virtual status_t onTransact(uint32_t code, const Parcel &data, Parcel *reply, uint32_t flag = 0);
};
#endif
