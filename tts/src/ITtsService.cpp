#define DESCRIPTOR "com.rokid.openvoice.TtsService"

#include "ITtsService.h"

class BpTtsService : public BpInterface<ITtsService>{
public:
	BpTtsService(const sp<IBinder> &impl):BpInterface<ITtsService>(impl) {}
	~BpTtsService(){}
	bool prepare() {
        Parcel data, reply;
        data.writeInterfaceToken(String16(DESCRIPTOR));
        remote()->transact(TRANSACTION_PREPARE, data, &reply);
        reply.readExceptionCode();
        return (reply.readInt32() > 0 ? true : false);
    }
	int speak(const string& content, sp<IBinder>& callback){
        Parcel data, reply;
        data.writeInterfaceToken(String16(DESCRIPTOR));
        data.writeString16(String16(content.c_str()));
        data.writeStrongBinder(callback);
        remote()->transact(TRANSACTION_SPEAK, data, &reply);
        reply.readExceptionCode();
        return reply.readInt32();
    }
	void cancel(int id){
        Parcel data, reply;
        data.writeInterfaceToken(String16(DESCRIPTOR));
        data.writeInt32(id);
        remote()->transact(TRANSACTION_PREPARE, data, &reply);
        reply.readExceptionCode();
    }
};

IMPLEMENT_META_INTERFACE (TtsService, DESCRIPTOR);

status_t BnTtsService::onTransact(uint32_t code, const Parcel &data, Parcel *reply, uint32_t flag){
	switch(code){
		case TRANSACTION_PREPARE:{
			CHECK_INTERFACE(ITtsService, data, reply);
			int result = prepare();
			reply->writeNoException();
            reply->writeInt32((result ? 1 : 0));
			return NO_ERROR;
		}
		case TRANSACTION_SPEAK:{
			CHECK_INTERFACE(ITtsService, data, reply);
			String8 _content(data.readString16());
			sp<IBinder> callback(data.readStrongBinder());
			int id = speak(_content.string(), callback);
			reply->writeNoException();
			reply->writeInt32(id);
			return NO_ERROR;
		}
		case TRANSACTION_CANCEL:{
			CHECK_INTERFACE(ITtsService, data, reply);
			cancel(data.readInt32());
			reply->writeNoException();
			return NO_ERROR;
		}
	}
	return BBinder::onTransact (code, data, reply, flag);
}

