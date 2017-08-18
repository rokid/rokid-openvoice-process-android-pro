#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>

#include "VoiceService.h"

using namespace android;

int main(void) {
    sp<ProcessState> proc(ProcessState::self());
    sp<VoiceService> service = new VoiceService();
    defaultServiceManager()->addService(String16(VoiceService::getServiceName()), service, false);

    ProcessState::self()->startThreadPool();
    IPCThreadState::self()->joinThreadPool();

    return 0;
}
