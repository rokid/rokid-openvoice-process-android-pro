#define DESCRIPTOR "com.rokid.openvoice.openvoice_process"
#define LOG_TAG "VoiceService"
//#define LOG_NDEBUG 0

#include "IVoiceService.h"
#include <cutils/log.h>

class BpVoiceService : public BpInterface<IVoiceService> {
public:
    BpVoiceService(const sp<IBinder> &impl): BpInterface<IVoiceService>(impl) {}
    ~BpVoiceService() {}
    bool init() {
        Parcel data, reply;
        data.writeInterfaceToken(String16(DESCRIPTOR));
        remote()->transact(TRANSACTION_INIT, data, &reply);
        reply.readExceptionCode();
        return ((reply.readInt32() > 0) ? true : false);        
    }
    void start_siren(const bool isopen) {
        Parcel data, reply;
        data.writeInterfaceToken(String16(DESCRIPTOR));
        data.writeInt32(isopen ? 1 : 0);
        remote()->transact(TRANSACTION_START_SIREN, data, &reply);
        reply.readExceptionCode();
    }
    void set_siren_state(const int32_t state) {
        Parcel data, reply;
        data.writeInterfaceToken(String16(DESCRIPTOR));
        data.writeInt32(state);
        remote()->transact(TRANSACTION_SET_SIREN_STATUS, data, &reply);
        reply.readExceptionCode();
    }
    void network_state_change(const bool connected) {
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
        data.writeInt32(1);
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
//    int32_t insert_vt_word(const vt_word_t& _vt_word){
//        Parcel data, reply;
//        data.writeInterfaceToken(String16(DESCRIPTOR));
//        string buff;
//        if (!_vt_word.SerializeToString(&buff)) {
//            return -1;
//
//        }
//#if (PLATFORM_SDK_VERSION > 22)
//        data.writeByteArray(buff.length(), (uint8_t*)buff.c_str());
//#endif
//        remote()->transact(TRANSACTION_INSERT_VT_WORD, data, &reply);
//        reply.readExceptionCode();
//        return reply.readInt32();        
//    }
//    int32_t delete_vt_word(const string& _vt_word){
//        Parcel data, reply;
//        data.writeInterfaceToken(String16(DESCRIPTOR));
//        data.writeString16(String16(_vt_word.c_str()));
//        remote()->transact(TRANSACTION_DELETE_VT_WORD, data, &reply);
//        reply.readExceptionCode();
//        return reply.readInt32();        
//    }
//    int32_t query_vt_word(vector<vt_word_t>& _vt_words_in){
//        Parcel data, reply;
//        data.writeInterfaceToken(String16(DESCRIPTOR));
//        remote()->transact(TRANSACTION_QUERY_VT_WORD, data, &reply);
//        reply.readExceptionCode();
//        int32_t temp = 0;
//        int32_t count = reply.readInt32();
//        if(count > 0){
//            _vt_words_in.reserve(count);
//            for(int i = 0; i < count; i++){
//                int32_t len = reply.readInt32();
//                len = reply.readInt32();
//                if(len > 0){
//                    vt_word_t _vt_word;
//                    const char* vt_word_ch = (const char*)reply.readInplace(len);
//                    if(vt_word_ch && _vt_word.ParseFromArray(vt_word_ch, len)){
//                        _vt_words_in.push_back(_vt_word);
//                        ALOGV("%s\n", _vt_words_in[i].DebugString().c_str());
//                        temp++;
//                    }
//                }
//            }
//        }
//        return temp;        
//    }
};

IMPLEMENT_META_INTERFACE (VoiceService, DESCRIPTOR);

status_t BnVoiceService::onTransact(uint32_t code, const Parcel &data, Parcel *reply, uint32_t flag) {
    switch(code) {
    case TRANSACTION_INIT: {
        CHECK_INTERFACE(IVoiceService, data, reply);
        const bool result = init();
        reply->writeNoException();
        reply->writeInt32((result ? 1 : 0));
        return NO_ERROR;
    }
    case TRANSACTION_START_SIREN: {
        CHECK_INTERFACE(IVoiceService, data, reply);
        const int32_t isopen = data.readInt32();
        start_siren((isopen > 0 ? true : false));
        reply->writeNoException();
        return NO_ERROR;
    }
    case TRANSACTION_SET_SIREN_STATUS: {
        CHECK_INTERFACE(IVoiceService, data, reply);
        const int32_t state = data.readInt32();
        set_siren_state(state);
        reply->writeNoException();
        return NO_ERROR;
    }
    case TRANSACTION_NETWORK_STATE_CHANGE: {
        CHECK_INTERFACE(IVoiceService, data, reply);
        const int32_t state = data.readInt32();
        network_state_change((state > 0 ? true : false));
        reply->writeNoException();
        return NO_ERROR;
    }
    case TRANSACTION_UPDATE_STACK: {
        CHECK_INTERFACE(IVoiceService, data, reply);
        String8 appid(data.readString16());
        update_stack(string(appid.string()));
        reply->writeNoException();
        return NO_ERROR;
    }
    case TRANSACTION_UPDATE_CONFIG: {
        CHECK_INTERFACE(IVoiceService, data, reply);
        string _device_id;
        string _device_type_id;
        string _key;
        string _secret;
        if(data.readInt32() > 0){
            String8 device_id(data.readString16());
            String8 device_type_id(data.readString16());
            String8 key(data.readString16());
            String8 secret(data.readString16());
            _device_id = device_id.string();
            _device_type_id = device_type_id.string();
            _key = key.string();
            _secret = secret.string();
        }
        update_config(_device_id, _device_type_id, _key, _secret);
        reply->writeNoException();
        return NO_ERROR;
    }
    case TRANSACTION_REGIST_CALLBACK: {
        CHECK_INTERFACE(IVoiceService, data, reply);
        regist_callback(data.readStrongBinder());
        reply->writeNoException();
        return NO_ERROR;
    }
//    case TRANSACTION_INSERT_VT_WORD: {
//        CHECK_INTERFACE(IVoiceService, data, reply);
//        vt_word_t _vt_word; 
//        int32_t result = -1;
//        int32_t len = data.readInt32();
//        if(len > 0){
//            const char* vt_word = (const char*)data.readInplace(len);
//            if(vt_word && _vt_word.ParseFromArray(vt_word, len)){
//                result = insert_vt_word(_vt_word);
//            }
//        }
//        reply->writeNoException();
//        reply->writeInt32(result);
//        return NO_ERROR;
//    }
//    case TRANSACTION_DELETE_VT_WORD: {
//        CHECK_INTERFACE(IVoiceService, data, reply);
//        String8 vt_word(data.readString16());
//        int32_t result = delete_vt_word(string(vt_word.string()));
//        reply->writeNoException();
//        reply->writeInt32(result);
//        return NO_ERROR;
//    }
//    case TRANSACTION_QUERY_VT_WORD: {
//        CHECK_INTERFACE(IVoiceService, data, reply);
//        string buff;
//        vector<vt_word_t> _vt_words_in, temp;
//        int32_t count = query_vt_word(_vt_words_in);
//        reply->writeNoException();
//        reply->writeInt32(count);
//        if(count > 0){
//            for(int i = 0; i < count; i++){
//                ALOGV("%s\n", _vt_words_in[i].DebugString().c_str());
//                if(_vt_words_in[i].SerializeToString(&buff)){
//                    reply->writeInt32(buff.length());
//#if (PLATFORM_SDK_VERSION > 22)
//                    reply->writeByteArray(buff.length(), (uint8_t*)buff.c_str());
//#endif
//                }
//                buff.clear();
//            }
//            _vt_words_in.swap(temp);
//        }
//        return NO_ERROR;
//    }
    }
    return BBinder::onTransact (code, data, reply, flag);
}
