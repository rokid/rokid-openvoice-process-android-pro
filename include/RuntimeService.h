#ifndef RUNTIME_SERVICE_H
#define RUNTIME_SERVICE_H

#include <cutils/log.h>
#include <pthread.h>
#include <stdlib.h>
#include <list>
#include <map>

#include "siren.h"
#include "asr.h"
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
						delete buff;
				}
		};

		class MyAsrCallback : public AsrCallback{
			public:

				MyAsrCallback(RuntimeService *runtime, Asr *asr):runtime_service(runtime), _asr(asr){
				}
				~MyAsrCallback(){
					_asr->release();
					delete _asr;
				}
				//Just used to delete
				Asr *_asr;
				RuntimeService *runtime_service;

				void onStart(int id);
				void onData(int id, const char* text);
				void onStop(int id);
				void onComplete(int id);
				void onError(int id, int err);
		};

		enum{
			SIREN_STATE_UNKNOW = 0,
			SIREN_STATE_AWAKE,
			SIREN_STATE_SLEEP,
		};
		int current_status = SIREN_STATE_UNKNOW;

		static char const* getServiceName(){
			return "runtime_native";
		}
		~RuntimeService();
		bool init();
		void set_siren_state(const int&);
		int get_siren_state();

		pthread_mutex_t siren_mutex;
		pthread_cond_t siren_cond;
		pthread_t siren_thread;

		list<VoiceMessage*> voice_queue;
		map<int, MyAsrCallback*> mAsrCallback;

	private:
};


void* siren_thread_loop(void*);

#endif // RUNTIME_SERVICE_H
