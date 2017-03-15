#ifndef RKRUNTIME_SERVICE_H
#define RKRUNTIME_SERVICE_H

#include <cutils/log.h>
#include <pthread.h>
#include <stdlib.h>
#include <list>

#include "siren.h"
#include "IRKRuntimeService.h"

using namespace android;
using namespace std;

class RKRuntimeService : public BnRKRuntimeService{
	public:
		class VoiceMessage{
			public:
				int length;
				siren_event_t event;
				void* buff;
		};
		static char const* getServiceName(){
			return "RKRuntimeService";
		}
		~RKRuntimeService(){
			free(siren_callback);
		}
		bool init();

		pthread_mutex_t *siren_mutex;
		pthread_cond_t *siren_cond;
		pthread_t siren_thread;

		list<VoiceMessage*> siren_msgs;

	private:
		siren_proc_callback_t *siren_callback;
};

void* siren_thread_loop(void*);
void voice_event_callback(void* token, int length, siren_event_t event, 
			void* buff, int has_sl,
			int has_voice, double sl_degree,
			int has_voiceprint);

#endif // RKRUNTIME_SERVICE_H
