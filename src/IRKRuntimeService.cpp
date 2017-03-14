#include "include/IRKRuntimeService.h"

using namespace android;

class BpRKRuntimeService : public BpInterface<IRKRuntimeService> {
	public:
		BpRKRuntimeService(const sp<IBinder> &impl):BpInterface<IRKRuntimeService>(impl) {}
};

IMPLEMENT_META_INTERFACE (RKRuntimeService, "rk_runtime_service");

status_t BnRKRuntimeService::onTransact(uint32_t code, const Parcel &data, Parcel *reply, uint32_t flag){
	return BBinder::onTransact (code, data, reply, flag);
}
