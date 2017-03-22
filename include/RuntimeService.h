#ifndef RUNTIME_SERVICE_H
#define RUNTIME_SERVICE_H

#include <cutils/log.h>
#include <pthread.h>
#include <stdlib.h>
#include <list>
#include <map>

#include "siren.h"
#include "nlp.h"
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

		class MyNlpCallback : public NlpCallback{
			public:

				MyNlpCallback(RuntimeService *runtime, Nlp *nlp):runtime_service(runtime), _nlp(nlp){
				}
				~MyNlpCallback(){
					if(event == VOICE_STATE_END || event == VOICE_STATE_CANCEL){
						_nlp->release();
						delete _nlp;
					}
				}
				//Just used to delete
				Nlp *_nlp;
				int event = VOICE_STATE_UNKNOW;
				RuntimeService *runtime_service;

				void onNlp(int id, const char* nlp);

				void onError(int id, int err);
		};

		enum{
			VOICE_STATE_UNKNOW = 100,
			VOICE_STATE_DATA,
			VOICE_STATE_END,
			VOICE_STATE_CANCEL,
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
		map<int, MyNlpCallback*> mNlpCallback;

	private:
};


void* siren_thread_loop(void*);

#endif // RUNTIME_SERVICE_H
