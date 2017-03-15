#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>

#include "include/RKRuntimeService.h"

using namespace android;

int main(void){
	ALOGV("-----------runtime service launch----------------");
	sp<ProcessState> proc(ProcessState::self());
	sp<RKRuntimeService> mRuntimeService = NULL;
	//mRuntimeService->init();
	sp<IServiceManager> sm(defaultServiceManager());
	sm->addService(String16(RKRuntimeService::getServiceName()), mRuntimeService, false);

	return 0;
}
