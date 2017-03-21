#include "include/IRuntimeService.h"

using namespace android;

class BpRuntimeService : public BpInterface<IRuntimeService>{
	public:
		BpRuntimeService(const sp<IBinder> &impl):BpInterface<IRuntimeService>(impl) {}
		virtual void set_siren_state(const int &state);
		virtual int get_siren_state();
};

IMPLEMENT_META_INTERFACE (RuntimeService, "com.rokid.server.RuntimeService");

status_t BnRuntimeService::onTransact(uint32_t code, const Parcel &data, Parcel *reply, uint32_t flag){
	switch(code){
		case TRANSACTION_SET_SIREN_STATUS:{
			CHECK_INTERFACE(IRuntimeService, data, reply);
			int state = data.readInt32();
			set_siren_state(state);
			reply->writeNoException();
			return NO_ERROR;
		}
		case TRANSACTION_GET_SIREN_STATUS:{
			CHECK_INTERFACE(IRuntimeService, data, reply);
			int state = get_siren_state();
			reply->writeNoException();
			reply->writeInt32(state);		
			return NO_ERROR;
		}
	}
	return BBinder::onTransact (code, data, reply, flag);
}
