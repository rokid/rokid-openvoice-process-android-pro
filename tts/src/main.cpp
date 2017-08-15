#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>

#include "TtsService.h"

int main(void){
	sp<ProcessState> proc(ProcessState::self());
	sp<TtsService> tts = new TtsService();
	defaultServiceManager()->addService(String16(TtsService::getServiceName()), tts, false);

	ProcessState::self()->startThreadPool();
	IPCThreadState::self()->joinThreadPool();

	return 0;
}

