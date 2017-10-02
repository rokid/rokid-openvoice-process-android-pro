#define DESCRIPTOR "com.rokid.openvoice.IVoiceCallback"

#include "IVoiceCallback.h"

class BpVoiceCallback : public BpInterface<IVoiceCallback> {
public:
    BpVoiceCallback(const sp<IBinder> &impl): BpInterface<IVoiceCallback>(impl) {}
    ~BpVoiceCallback() {}
    void voice_event(const int32_t id, const int32_t event, const double sl, const double energy) {
        Parcel data, reply;
        data.writeInterfaceToken(String16(DESCRIPTOR));
        data.writeInt32(id);
        data.writeInt32(event);
        data.writeDouble(sl);
        data.writeDouble(energy);
        remote()->transact(TRANSACTION_VOICE_EVENT, data, &reply);
        reply.readExceptionCode();
    }
    void intermediate_result(const int32_t id, const int32_t type, const string& asr) {
        Parcel data, reply;
        data.writeInterfaceToken(String16(DESCRIPTOR));
        data.writeInt32(id);
        data.writeInt32(type);
        data.writeString16(String16(asr.c_str()));
        remote()->transact(TRANSACTION_INTERMEDIATE_RESULT, data, &reply);
        reply.readExceptionCode();
    }
    void voice_command(const int32_t id, const string &asr, const string &nlp, const string &action) {
        Parcel data, reply;
        data.writeInterfaceToken(String16(DESCRIPTOR));
        data.writeInt32(id);
        data.writeString16(String16(asr.c_str()));
        data.writeString16(String16(nlp.c_str()));
        data.writeString16(String16(action.c_str()));
        remote()->transact(TRANSACTION_VOICE_COMMAND, data, &reply);
        reply.readExceptionCode();
    }
    void speech_error(const int32_t id, const int32_t errcode) {
        Parcel data, reply;
        data.writeInterfaceToken(String16(DESCRIPTOR));
        data.writeInt32(id);
        data.writeInt32(errcode);
        remote()->transact(TRANSACTION_SPEECH_ERROR, data, &reply);
        reply.readExceptionCode();
    }
    const string get_skill_options() {
        Parcel data, reply;
        data.writeInterfaceToken(String16(DESCRIPTOR));
        remote()->transact(TRANSACTION_GET_SKILL_OPTIONS, data, &reply);
        reply.readExceptionCode();
        String8 options(reply.readString16());
        return string(options.string());
    }
};

IMPLEMENT_META_INTERFACE (VoiceCallback, DESCRIPTOR);

status_t BnVoiceCallback::onTransact(uint32_t code, const Parcel &data, Parcel *reply, uint32_t flag) {
    switch(code) {
    case TRANSACTION_VOICE_EVENT: {
        CHECK_INTERFACE(IVoiceCallback, data, reply);
        const int32_t id = data.readInt32();
        const int32_t event = data.readInt32();
        const double sl = data.readDouble();
        const double energy = data.readDouble();
        voice_event(id, event, sl, energy);
        reply->writeNoException();
        return NO_ERROR;
    }
    case TRANSACTION_INTERMEDIATE_RESULT: {
        CHECK_INTERFACE(IVoiceCallback, data, reply);
        const int32_t id = data.readInt32();
        const int32_t type = data.readInt32();
        String8 asr(data.readString16());
        intermediate_result(id, type, string(asr.string()));
        reply->writeNoException();
        return NO_ERROR;
    }
    case TRANSACTION_VOICE_COMMAND: {
        CHECK_INTERFACE(IVoiceCallback, data, reply);
        const int32_t id = data.readInt32();
        String8 asr(data.readString16());
        String8 nlp(data.readString16());
        String8 action(data.readString16());
        voice_command(id, string(asr.string()), string(nlp.string()), string(action.string()));
        reply->writeNoException();
        return NO_ERROR;
    }
    case TRANSACTION_SPEECH_ERROR: {
        CHECK_INTERFACE(IVoiceCallback, data, reply);
        const int32_t id = data.readInt32();
        const int32_t errcode = data.readInt32();
        speech_error(id, errcode);
        reply->writeNoException();
        return NO_ERROR;
    }
    case TRANSACTION_GET_SKILL_OPTIONS: {
        CHECK_INTERFACE(IVoiceCallback, data, reply);
        const string& options = get_skill_options();
        reply->writeNoException();
        reply->writeString16(String16(options.c_str()));
        return NO_ERROR;
    }
    }
    return BBinder::onTransact (code, data, reply, flag);
}
