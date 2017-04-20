#ifndef RUNTIME_SERVICE_H
#define RUNTIME_SERVICE_H

#define SPEECH_CONFIG_FILE "/system/etc/speech_sdk.json"

#include <cutils/log.h>
#include <pthread.h>
#include <stdlib.h>
#include <list>

#include "siren.h"
#include "speech.h"
#include "IRuntimeService.h"

using namespace android;
using namespace std;
using namespace rokid;
using namespace speech;

class RuntimeService : public BnRuntimeService{
	public:
		class VoiceMessage{
			public:
				void* buff = NULL;
				int length;
				siren_event_t event;
				int has_voice;
				int has_voiceprint;
				int has_sl;
				double energy;
				double threshold;
				double sl_degree;

				~VoiceMessage(){
					if(buff != NULL)
						delete []buff;
					buff = NULL;
				}
		};

		static char const* getServiceName(){
			return "runtime_native";
		}

		bool init();
		void config();
		void start_siren(bool);
		void set_siren_state(const int&);

		pthread_mutex_t event_mutex;
		pthread_cond_t event_cond;
		pthread_t event_thread;
		pthread_t response_thread;

		bool flag = true;
		Speech *_speech = NULL;
		sp<IBinder> power_poxy = NULL;
		list<VoiceMessage*> message_queue;
};

void* onEvent(void *);
void* onResponse(void *);

#endif // RUNTIME_SERVICE_H
