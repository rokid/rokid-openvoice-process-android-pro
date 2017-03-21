#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>

#include "include/RuntimeService.h"

using namespace android;

int main(void){
	ALOGV("-----------runtime service launch----------------");
	sp<ProcessState> proc(ProcessState::self());
	sp<RuntimeService> mRuntimeService = new RuntimeService();
	String16 s(mRuntimeService->getInterfaceDescriptor());
	sp<IServiceManager> sm(defaultServiceManager());
	status_t state = sm->addService(String16(RuntimeService::getServiceName()), mRuntimeService, false);
	mRuntimeService->init();
	return 0;
}
