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

        class DeathNotifier : public IBinder::DeathRecipient{
            public:
                DeathNotifier(RuntimeService *runtime) : _runtime(runtime){}
            private:
                RuntimeService *_runtime = NULL;
                void binderDied(const wp<IBinder> &binder){_runtime->remote = NULL;}
        };

		static char const* getServiceName(){
			return "runtime_native";
		}
		RuntimeService();
		void send_siren_event(int, double, int);

		pthread_mutex_t event_mutex;
		pthread_mutex_t speech_mutex;
        pthread_mutex_t siren_mutex;
		pthread_cond_t event_cond;
		pthread_t event_thread;
		pthread_t response_thread;
        

        bool openSiren = true;
		Speech *_speech = NULL;
		sp<IBinder> remote = NULL;
		list<VoiceMessage*> message_queue;

    private:
		bool init();
		void config();
		void start_siren(bool);
		void set_siren_state(const int&);
		void network_state_change(bool);
		void update_stack(String16);
		void add_binder(sp<IBinder>);
#ifdef USB_AUDIO_DEVICE
        bool wait_for_alsa_usb_card();
#endif

        int mCurrentSirenState = SIREN_STATE_UNKNOW;
        int mCurrentSpeechState = SPEECH_STATE_UNKNOW;
        enum{
            SIREN_STATE_UNKNOW = 0,
            SIREN_STATE_INITED,
            SIREN_STATE_STARTED,
            SIREN_STATE_STOPED
        };
        enum{
            SPEECH_STATE_UNKNOW = 0,
            SPEECH_STATE_PREPARED,
            SPEECH_STATE_RELEASED
        };
};

void* onEvent(void *);
void* onResponse(void *);

#endif // RUNTIME_SERVICE_H
