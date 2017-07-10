#ifndef VOICE_SERVICE_H
#define VOICE_SERVICE_H

#define SPEECH_CONFIG_FILE "/system/etc/openvoice_profile.json"

#include <cutils/log.h>
#include <utils/String8.h>
#include <pthread.h>
#include <stdlib.h>
#include <list>

#if defined(__ANDROID__) || defined(ANDROID)
#include "siren.h"
#include "speech.h"
#else
#include <blacksiren/siren.h>
#include <speech/speech.h>
#endif

#include "IVoiceService.h"

using namespace android;
using namespace std;
using namespace rokid;
using namespace speech;

class VoiceService: public BnVoiceService{
	public:
        class DeathNotifier : public IBinder::DeathRecipient{
            public:
                DeathNotifier(VoiceService *service) : _service(service){}
                void binderDied(const wp<IBinder> &binder){_service->proxy.clear();}
            private:
                VoiceService *_service = NULL;
        };

		static char const* getServiceName(){
			return "openvoice_process";
		}

		VoiceService();
		void send_siren_event(int, double, int);
        int vad_start(const voice_event_t *);

		pthread_mutex_t event_mutex;
		pthread_mutex_t speech_mutex;
		pthread_mutex_t siren_mutex;
		pthread_cond_t event_cond;
		pthread_t event_thread;
		pthread_t response_thread;

        shared_ptr<Speech> _speech;
		sp<IBinder> proxy;
        list<voice_event_t*> message_queue;

    private:
		bool init();
		void config();
		void start_siren(bool);
		void set_siren_state(const int);
		void network_state_change(bool);
		void update_stack(String16&);
		void add_binder(sp<IBinder>);
#ifdef USB_AUDIO_DEVICE
        bool wait_for_alsa_usb_card();
#endif

        int mCurrentSirenState = SIREN_STATE_UNKNOWN;
        int mCurrentSpeechState = SPEECH_STATE_UNKNOWN;
        enum{
            SIREN_STATE_UNKNOWN = 0,
            SIREN_STATE_INITED,
            SIREN_STATE_STARTED,
            SIREN_STATE_STOPED
        };
        enum{
            SPEECH_STATE_UNKNOWN = 0,
            SPEECH_STATE_PREPARED,
            SPEECH_STATE_RELEASED
        };
        String8 appid;
        bool openSiren = true;
};

void* onEvent(void *);
void* onResponse(void *);

#endif // VOICE_SERVICE_H
