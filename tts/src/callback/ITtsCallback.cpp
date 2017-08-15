#define DESCRIPTOR "rokid.tts.ITtsCallback"
//#define DESCRIPTOR "com.rokid.openvoice.TtsCallback"

#include "ITtsCallback.h"

class BpTtsCallback : public BpInterface<ITtsCallback>{
public:
	BpTtsCallback(const sp<IBinder> &impl):BpInterface<ITtsCallback>(impl) {}
	~BpTtsCallback(){}
	void onStart(int id){
		Parcel data, reply;
		data.writeInterfaceToken(String16(DESCRIPTOR));
		data.writeInt32(id);
		remote()->transact(IBinder::FIRST_CALL_TRANSACTION + 0,data, &reply);
		data.readExceptionCode();
	}
	void onCancel(int id){
		Parcel data, reply;
		data.writeInterfaceToken(String16(DESCRIPTOR));
		data.writeInt32(id);
		remote()->transact(IBinder::FIRST_CALL_TRANSACTION + 1,data, &reply);
		data.readExceptionCode();
	}
	void onComplete(int id){
		Parcel data, reply;
		data.writeInterfaceToken(String16(DESCRIPTOR));
		data.writeInt32(id);
		remote()->transact(IBinder::FIRST_CALL_TRANSACTION + 2,data, &reply);
		data.readExceptionCode();
	}
	void onError(int id, int err){
		Parcel data, reply;
		data.writeInterfaceToken(String16(DESCRIPTOR));
		data.writeInt32(id);
		data.writeInt32(err);
		remote()->transact(IBinder::FIRST_CALL_TRANSACTION + 3,data, &reply);
		data.readExceptionCode();

	}
};

IMPLEMENT_META_INTERFACE (TtsCallback, DESCRIPTOR);

status_t BnTtsCallback::onTransact(uint32_t code, const Parcel &data, Parcel *reply, uint32_t flag){
	switch(code){
		case TRANSACTION_ONSTART:{
			CHECK_INTERFACE(ITtsCallback, data, reply);
			onStart(data.readInt32());
			reply->writeNoException();
			return NO_ERROR;
		}
		case TRANSACTION_ONCANCEL:{
			CHECK_INTERFACE(ITtsCallback, data, reply);
			onCancel(data.readInt32());
			reply->writeNoException();
			return NO_ERROR;
		}
		case TRANSACTION_ONCOMPLETE:{
			CHECK_INTERFACE(ITtsCallback, data, reply);
			onComplete(data.readInt32());
			reply->writeNoException();
			return NO_ERROR;
		}
		case TRANSACTION_ONERROR:{
			CHECK_INTERFACE(ITtsCallback, data, reply);
			int id = data.readInt32();
			int error = data.readInt32();
			onError(id, error);
			reply->writeNoException();
			return NO_ERROR;
		}
	}
	return BBinder::onTransact (code, data, reply, flag);
}
