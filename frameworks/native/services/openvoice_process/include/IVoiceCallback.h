#ifndef IVOICE_CALLBACK_H
#define IVOICE_CALLBACK_H

#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <utils/String8.h>
#include <string>

using namespace android;
using namespace std;

enum {
    TRANSACTION_VOICE_EVENT = IBinder::FIRST_CALL_TRANSACTION + 0,
    TRANSACTION_INTERMEDIATE_RESULT,
    TRANSACTION_VOICE_COMMAND,
    TRANSACTION_SPEECH_ERROR,
    TRANSACTION_GET_SKILL_OPTIONS,
};

class IVoiceCallback: public IInterface {
public:
    DECLARE_META_INTERFACE(VoiceCallback);
    virtual void voice_event(const int32_t id, const int32_t event, const double sl, const double energy) = 0;
    virtual void intermediate_result(const int32_t id, const int32_t type, const string& asr) = 0;
    virtual void voice_command(const int32_t id, const string& asr, const string& nlp, const string& action) = 0;
    virtual void speech_error(const int32_t id, const int32_t errcode) = 0;
    virtual const string get_skill_options() = 0;
};

class BnVoiceCallback : public BnInterface<IVoiceCallback> {
public:
    virtual status_t onTransact(uint32_t code, const Parcel &data, Parcel *reply, uint32_t flag = 0);
};
#endif
