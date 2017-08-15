#include <stdio.h>
#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>

#include "ITtsService.h"
#include "TtsCallback.h"

using namespace std;
using namespace android;

sp<ITtsService> tts = interface_cast<ITtsService>(defaultServiceManager()->getService(String16("tts_process")));

void TtsCallback::onStart(int id) {

}

void TtsCallback::onComplete(int id) {

}

void TtsCallback::onCancel(int id) {

}

void TtsCallback::onError(int id, int err) {

}

int main(int argc, char** argv)
{
    tts->prepare();
    sp<IBinder> callback(new TtsCallback);
    sleep(2);
    tts->speak("啦啦啦", callback);

	ProcessState::self()->startThreadPool();
	IPCThreadState::self()->joinThreadPool();
	return 0;
}
