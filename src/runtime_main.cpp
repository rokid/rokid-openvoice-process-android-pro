#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>

#include "include/RuntimeService.h"

using namespace android;

int main(void){
	ALOGV("-----------runtime service launch----------------");
	sp<ProcessState> proc(ProcessState::self());
	sp<RuntimeService> mRuntimeService = NULL;
	//mRuntimeService->init();
	sp<IServiceManager> sm(defaultServiceManager());
	sm->addService(String16(RuntimeService::getServiceName()), mRuntimeService, false);

	return 0;
}
