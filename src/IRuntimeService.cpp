#define LOG_TAG "RuntimeService"
#define LOG_NDEBUG 0

#include "include/IRuntimeService.h"
#include <cutils/log.h>

using namespace android;

class BpRuntimeService : public BpInterface<IRuntimeService>{
	public:
		BpRuntimeService(const sp<IBinder> &impl):BpInterface<IRuntimeService>(impl) {}
		void set_siren_state(const int &state){}
		int get_siren_state(){
			return 0;
		}
};

IMPLEMENT_META_INTERFACE (RuntimeService, "com.rokid.server.RuntimeService");

status_t BnRuntimeService::onTransact(uint32_t code, const Parcel &data, Parcel *reply, uint32_t flag){
	ALOGI("onTransact    >>>   " + code);
	switch(code){
		case TRANSACTION_SET_SIREN_STATUS:{
			CHECK_INTERFACE(IRuntimeService, data, reply);
			int state = data.readInt32();
			set_siren_state(state);
			reply->readExceptionCode();
			return NO_ERROR;
		}
		case TRANSACTION_GET_SIREN_STATUS:{
			CHECK_INTERFACE(IRuntimeService, data, reply);
			int state = get_siren_state();
			reply->readExceptionCode();
			reply->writeInt32(state);		
			return NO_ERROR;
		}
	}
	return BBinder::onTransact (code, data, reply, flag);
}
