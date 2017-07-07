#include "IRuntimeService.h"

using namespace android;

class BpRuntimeService : public BpInterface<IRuntimeService>{
	public:
		BpRuntimeService(const sp<IBinder> &impl):BpInterface<IRuntimeService>(impl) {}
		virtual bool init();
		virtual void start_siren(bool);
		virtual void set_siren_state(const int&);
		virtual void network_state_change(bool);
		virtual void update_stack(String16);
		virtual void add_binder(sp<IBinder>);
};

IMPLEMENT_META_INTERFACE (RuntimeService, "com.rokid.openvoice.native.RuntimeService");

status_t BnRuntimeService::onTransact(uint32_t code, const Parcel &data, Parcel *reply, uint32_t flag){
	switch(code){
		case TRANSACTION_INIT:{
			CHECK_INTERFACE(IRuntimeService, data, reply);
			bool err = init();
			reply->writeNoException();
			reply->writeInt32((err ? 1 : 0));
			return NO_ERROR;
		}
		case TRANSACTION_START_SIREN:{
			CHECK_INTERFACE(IRuntimeService, data, reply);
			int flag = data.readInt32();
			start_siren((flag > 0 ? true : false));
			reply->writeNoException();
			return NO_ERROR;
		}
		case TRANSACTION_SET_SIREN_STATUS:{
			CHECK_INTERFACE(IRuntimeService, data, reply);
			int state = data.readInt32();
			set_siren_state(state);
			reply->writeNoException();
			return NO_ERROR;
		}
		case TRANSACTION_NETWORK_STATE_CHANGE:{
			CHECK_INTERFACE(IRuntimeService, data, reply);
			int state = data.readInt32();
			network_state_change((state > 0 ? true : false));
			reply->writeNoException();
			return NO_ERROR;
		}
		case TRANSACTION_UPDATE_STACK:{
			CHECK_INTERFACE(IRuntimeService, data, reply);
			String16 appid(data.readString16());
			update_stack(appid);
			reply->writeNoException();
			return NO_ERROR;
		}
		case TRANSACTION_ADD_BINDER:{
			CHECK_INTERFACE(IRuntimeService, data, reply);
			add_binder(data.readStrongBinder());
			reply->writeNoException();
			return NO_ERROR;
		}
	}
	return BBinder::onTransact (code, data, reply, flag);
}
