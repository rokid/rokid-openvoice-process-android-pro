#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>

#include "include/RuntimeService.h"

using namespace android;

int main(void){
	sp<ProcessState> proc(ProcessState::self());
	sp<RuntimeService> runtime = new RuntimeService();
	defaultServiceManager()->addService(String16(RuntimeService::getServiceName()), runtime, false);

	ProcessState::self()->startThreadPool();
	IPCThreadState::self()->joinThreadPool();

	return 0;
}
