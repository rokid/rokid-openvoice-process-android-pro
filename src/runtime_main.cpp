#define LOG_TAG "RKRuntimeService"
#define LOG_NDEBUG 0

#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <cutils/log.h>

#include "include/RKRuntimeService.h"

using namespace android;

int main(int argc, char **argv){
	ALOGV("-----------runtime service launch----------------");
	sp<ProcessState> proc(ProcessState::self());
	sp<RKRuntimeService> mRuntimeService = new RKRuntimeService();
	sp<IServiceManager> sm(defaultServiceManager());
	sm->addService(String16(RKRuntimeService::getServiceName()), mRuntimeService, false);
	return 0;
}
