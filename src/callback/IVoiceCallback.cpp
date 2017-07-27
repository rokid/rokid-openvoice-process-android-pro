#include "IVoiceCallback.h"

class BpVoiceCallback : public BpInterface<IVoiceCallback> {
public:
    BpVoiceCallback(const sp<IBinder> &impl): BpInterface<IVoiceCallback>(impl) {}
    ~BpVoiceCallback() {}
    void voice_command(const string &asr, const string &nlp, const string &action) {
        Parcel data, reply;
        data.writeInterfaceToken(remote()->getInterfaceDescriptor());
        data.writeString16(String16(asr.c_str()));
        data.writeString16(String16(nlp.c_str()));
        data.writeString16(String16(action.c_str()));
        remote()->transact(TRANSACTION_VOICE_COMMAND, data, &reply);
        reply.readExceptionCode();
    }

    void voice_event(int event, bool has_sl, double sl, double energy, double threshold) {
        Parcel data, reply;
        data.writeInterfaceToken(remote()->getInterfaceDescriptor());
        data.writeInt32(event);
        data.writeInt32((has_sl ? 1 : 0));
        data.writeDouble(sl);
        data.writeDouble(energy);
        data.writeDouble(threshold);
        remote()->transact(TRANSACTION_VOICE_EVENT, data, &reply);
    }

    void voice_reject() {
        Parcel data, reply;
        data.writeInterfaceToken(remote()->getInterfaceDescriptor());
        remote()->transact(TRANSACTION_VOICE_REJECT, data, &reply);
    }

    void speech_timeout() {
        Parcel data, reply;
        data.writeInterfaceToken(remote()->getInterfaceDescriptor());
        remote()->transact(TRANSACTION_SPEECH_TIMEOUT, data, &reply);
    }
};

IMPLEMENT_META_INTERFACE (VoiceCallback, DESCRIPTOR);

status_t BnVoiceCallback::onTransact(uint32_t code, const Parcel &data, Parcel *reply, uint32_t flag) {
    switch(code) {
    case TRANSACTION_VOICE_COMMAND: {
        CHECK_INTERFACE(IVoiceCallback, data, reply);
        String8 asr(data.readString16());
        String8 nlp(data.readString16());
        String8 action(data.readString16());
        voice_command(asr.string(), nlp.string(), action.string());
        reply->writeNoException();
        return NO_ERROR;
    }
    case TRANSACTION_VOICE_EVENT: {
        CHECK_INTERFACE(IVoiceCallback, data, reply);
        int32_t event = data.readInt32();
        int32_t has_sl = data.readInt32();
        double sl = data.readDouble();
        double energy = data.readDouble();
        double threshold = data.readDouble();
        voice_event(event, (has_sl > 0 ? true : false), sl, energy, threshold);
        reply->writeNoException();
        return NO_ERROR;
    }
    case TRANSACTION_VOICE_REJECT: {
        CHECK_INTERFACE(IVoiceCallback, data, reply);
        voice_reject();
        reply->writeNoException();
        return NO_ERROR;
    }
    case TRANSACTION_SPEECH_TIMEOUT: {
        CHECK_INTERFACE(IVoiceCallback, data, reply);
        speech_timeout();
        reply->writeNoException();
        return NO_ERROR;
    }
    }
    return BBinder::onTransact (code, data, reply, flag);
}
