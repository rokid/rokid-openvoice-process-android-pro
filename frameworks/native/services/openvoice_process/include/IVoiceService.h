#ifndef IVOICE_SERVICE_H
#define IVOIDE_SERVICE_H

#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <utils/String8.h>
#include <string>
#include <vector>

//#include "openvoice_process.pb.h"

//using namespace openvoice_process;
using namespace android;
using namespace std;

enum {
    TRANSACTION_INIT = IBinder::FIRST_CALL_TRANSACTION + 0,
    TRANSACTION_START_SIREN,
    TRANSACTION_SET_SIREN_STATUS,
    TRANSACTION_NETWORK_STATE_CHANGE,
    TRANSACTION_UPDATE_STACK,
    TRANSACTION_UPDATE_CONFIG,
    TRANSACTION_REGIST_CALLBACK,

    TRANSACTION_INSERT_VT_WORD,
    TRANSACTION_DELETE_VT_WORD,
    TRANSACTION_QUERY_VT_WORD,
};

class IVoiceService : public IInterface {
public:
    DECLARE_META_INTERFACE(VoiceService);
    virtual bool init() = 0;
    virtual void start_siren(const bool isopen) = 0;
    virtual void set_siren_state(const int state) = 0;
    virtual void network_state_change(const bool isconnect) = 0;
    virtual void update_stack(const string& appid) = 0;
    virtual void update_config(const string& device_id, const string& device_type_id, const string& key, const string& secret) = 0;
    virtual void regist_callback(const sp<IBinder>& callback) = 0;

//    virtual int32_t insert_vt_word(const vt_word_t& _vt_word) = 0;
//    virtual int32_t delete_vt_word(const string& _vt_word) = 0;
//    virtual int32_t query_vt_word(vector<vt_word_t>& _vt_words_in) = 0;
};

class BnVoiceService : public BnInterface<IVoiceService> {
public:
    virtual status_t onTransact(uint32_t code, const Parcel &data, Parcel *reply, uint32_t flag = 0);
};

#endif // IVOICE_SERVICE_H
