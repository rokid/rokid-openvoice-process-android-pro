#include "include/IRuntimeService.h"

using namespace android;

class BpRuntimeService : public BpInterface<IRuntimeService>{
	public:
		BpRuntimeService(const sp<IBinder> &impl):BpInterface<IRuntimeService>(impl) {}
		void set_siren_state(const int &state){}
		int get_siren_state(){
			return 0;
		}
};

IMPLEMENT_META_INTERFACE (RuntimeService, "runtime_service");

status_t BnRuntimeService::onTransact(uint32_t code, const Parcel &data, Parcel *reply, uint32_t flag){
	switch(code){
		case TRANSACTION_SET_SIREN_STATUS:{
			CHECK_INTERFACE(IRuntimeService, data, reply);
			int state = data.readInt32();
			set_siren_state(state);
			return NO_ERROR;
		}
		case TRANSACTION_GET_SIREN_STATUS:{
			CHECK_INTERFACE(IRuntimeService, data, reply);
			int state = get_siren_state();
			reply->writeInt32(state);		
			return NO_ERROR;
		}
	}
	return BBinder::onTransact (code, data, reply, flag);
}
