#include "include/IRuntimeService.h"

using namespace android;

class BpRuntimeService : public BpInterface<IRuntimeService>{
	public:
		BpRuntimeService(const sp<IBinder> &impl):BpInterface<IRuntimeService>(impl) {}
};

IMPLEMENT_META_INTERFACE (RuntimeService, "runtime_service");

status_t BnRuntimeService::onTransact(uint32_t code, const Parcel &data, Parcel *reply, uint32_t flag){
	return BBinder::onTransact (code, data, reply, flag);
}
