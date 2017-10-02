#define DESCRIPTOR "com.rokid.openvoice.openvoice_process"

#include "IVoiceService.h"

class BpVoiceService : public BpInterface<IVoiceService> {
public:
    BpVoiceService(const sp<IBinder> &impl): BpInterface<IVoiceService>(impl) {}
    ~BpVoiceService() {}
    bool setup() {
        Parcel data, reply;
        data.writeInterfaceToken(String16(DESCRIPTOR));
        remote()->transact(TRANSACTION_SETUP, data, &reply);
        reply.readExceptionCode();
        return ((reply.readInt32() > 0) ? true : false);        
    }
    void start_siren(bool flag) {
        Parcel data, reply;
        data.writeInterfaceToken(String16(DESCRIPTOR));
        data.writeInt32(flag ? 1 : 0);
        remote()->transact(TRANSACTION_START_SIREN, data, &reply);
        reply.readExceptionCode();
    }
    void set_siren_state(const int state) {
        Parcel data, reply;
        data.writeInterfaceToken(String16(DESCRIPTOR));
        data.writeInt32(state);
        remote()->transact(TRANSACTION_SET_SIREN_STATUS, data, &reply);
        reply.readExceptionCode();
    }
    void network_state_change(bool connected) {
        Parcel data, reply;
        data.writeInterfaceToken(String16(DESCRIPTOR));
        data.writeInt32(connected ? 1 : 0);
        remote()->transact(TRANSACTION_NETWORK_STATE_CHANGE, data, &reply);
        reply.readExceptionCode();
    }
    void update_stack(const string& appid) {
        Parcel data, reply;
        data.writeInterfaceToken(String16(DESCRIPTOR));
        data.writeString16(String16(appid.c_str()));
        remote()->transact(TRANSACTION_UPDATE_STACK, data, &reply);
        reply.readExceptionCode();
    }
    void update_config(const string& device_id, const string& device_type_id, 
                                        const string& key, const string& secret) {
        Parcel data, reply;
        data.writeInterfaceToken(String16(DESCRIPTOR));
        data.writeString16(String16(device_id.c_str()));
        data.writeString16(String16(device_type_id.c_str()));
        data.writeString16(String16(key.c_str()));
        data.writeString16(String16(secret.c_str()));
        remote()->transact(TRANSACTION_UPDATE_CONFIG, data, &reply);
        reply.readExceptionCode();
    }
    void regist_callback(const sp<IBinder>& callback) {
        Parcel data, reply;
        data.writeInterfaceToken(String16(DESCRIPTOR));
        data.writeStrongBinder(callback);
        remote()->transact(TRANSACTION_REGIST_CALLBACK, data, &reply);
        reply.readExceptionCode();
    }
};

IMPLEMENT_META_INTERFACE (VoiceService, DESCRIPTOR);

status_t BnVoiceService::onTransact(uint32_t code, const Parcel &data, Parcel *reply, uint32_t flag) {
    switch(code) {
    case TRANSACTION_SETUP: {
        CHECK_INTERFACE(IVoiceService, data, reply);
        bool err = setup();
        reply->writeNoException();
        reply->writeInt32((err ? 1 : 0));
        return NO_ERROR;
    }
    case TRANSACTION_START_SIREN: {
        CHECK_INTERFACE(IVoiceService, data, reply);
        int flag = data.readInt32();
        start_siren((flag > 0 ? true : false));
        reply->writeNoException();
        return NO_ERROR;
    }
    case TRANSACTION_SET_SIREN_STATUS: {
        CHECK_INTERFACE(IVoiceService, data, reply);
        int state = data.readInt32();
        set_siren_state(state);
        reply->writeNoException();
        return NO_ERROR;
    }
    case TRANSACTION_NETWORK_STATE_CHANGE: {
        CHECK_INTERFACE(IVoiceService, data, reply);
        int state = data.readInt32();
        network_state_change((state > 0 ? true : false));
        reply->writeNoException();
        return NO_ERROR;
    }
    case TRANSACTION_UPDATE_STACK: {
        CHECK_INTERFACE(IVoiceService, data, reply);
        String8 appid(data.readString16());
        update_stack(appid.string());
        reply->writeNoException();
        return NO_ERROR;
    }
    case TRANSACTION_UPDATE_CONFIG: {
        CHECK_INTERFACE(IVoiceService, data, reply);
        String8 device_id(data.readString16());
        String8 device_type_id(data.readString16());
        String8 key(data.readString16());
        String8 secret(data.readString16());
        update_config(device_id.string(), device_type_id.string(), key.string(), secret.string());
        reply->writeNoException();
        return NO_ERROR;
    }
    case TRANSACTION_REGIST_CALLBACK: {
        CHECK_INTERFACE(IVoiceService, data, reply);
        regist_callback(data.readStrongBinder());
        reply->writeNoException();
        return NO_ERROR;
    }
    }
    return BBinder::onTransact (code, data, reply, flag);
}
