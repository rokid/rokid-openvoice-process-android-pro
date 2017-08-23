#ifndef IVOICE_SERVICE_H
#define IVOIDE_SERVICE_H

#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <utils/String8.h>
#include <string>

using namespace android;
using namespace std;

enum {
    TRANSACTION_SETUP = IBinder::FIRST_CALL_TRANSACTION + 0,
    TRANSACTION_START_SIREN,
    TRANSACTION_SET_SIREN_STATUS,
    TRANSACTION_NETWORK_STATE_CHANGE,
    TRANSACTION_UPDATE_STACK,
    TRANSACTION_UPDATE_CONFIG,
    TRANSACTION_REGIST_CALLBACK,
};

class IVoiceService : public IInterface {
public:
    DECLARE_META_INTERFACE(VoiceService);
    virtual bool setup() = 0;
    virtual void start_siren(bool) = 0;
    virtual void set_siren_state(const int) = 0;
    virtual void network_state_change(bool) = 0;
    virtual void update_stack(const string&) = 0;
    virtual void update_config(const string&, const string&, const string&, const string&) = 0;
    virtual void regist_callback(const sp<IBinder>&) = 0;
};

class BnVoiceService : public BnInterface<IVoiceService> {
public:
    virtual status_t onTransact(uint32_t code, const Parcel &data, Parcel *reply, uint32_t flag = 0);
};

#endif // IVOICE_SERVICE_H
