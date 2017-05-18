#ifndef RUNTIME_SERVICE_H
#define RUNTIME_SERVICE_H

#define SPEECH_CONFIG_FILE "/system/etc/openvoice_profile.json"

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
		RuntimeService();

		bool init();
		void config();
		void start_siren(bool);
		void set_siren_state(const int&);
		void network_state_change(bool);
		void update_stack(String16, String16);
		void add_binder(sp<IBinder>);
		void send_siren_event(int, double, int);

		pthread_mutex_t event_mutex;
		pthread_mutex_t speech_mutex;
		pthread_cond_t event_cond;
		pthread_t event_thread;
		pthread_t response_thread;

		bool ready = false;
		bool prepared = false;
		Speech *_speech = NULL;
		sp<IBinder> remote = NULL;
		list<VoiceMessage*> message_queue;
};

void* onEvent(void *);
void* onResponse(void *);

#endif // RUNTIME_SERVICE_H
