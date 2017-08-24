#ifndef VOICE_SERVICE_H
#define VOICE_SERVICE_H

#include <cutils/log.h>
#include <pthread.h>
#include <stdlib.h>
#include <list>

#include "siren.h"
#include "speech.h"
#include "voice_config.h"
#include "IVoiceService.h"
#include "callback/CallbackProxy.h"

using namespace rokid;
using namespace speech;

class VoiceService: public BnVoiceService {

public:

    static char const* getServiceName() {
        return "openvoice_process";
    }

    VoiceService();

    bool setup();
    void start_siren(bool);
    void set_siren_state(const int);
    void network_state_change(bool);
    void update_stack(const string&);
    void update_config(const string&, const string&, const string&, const string&);
    void regist_callback(const sp<IBinder>&);

private:

#ifdef USB_AUDIO_DEVICE
    bool wait_for_alsa_usb_card();
#endif

    int mCurrentSirenState = SIREN_STATE_UNKNOWN;
    int mCurrentSpeechState = SPEECH_STATE_UNKNOWN;
    enum {
        SIREN_STATE_UNKNOWN = 0,
        SIREN_STATE_INITED,
        SIREN_STATE_STARTED,
        SIREN_STATE_STOPED
    };
    enum {
        SPEECH_STATE_UNKNOWN = 0,
        SPEECH_STATE_PREPARED,
        SPEECH_STATE_RELEASED
    };

	void* onEvent();
	void* onResponse();

    int32_t vad_start();
    void voice_print(const voice_event_t *);
	void voice_event_callback(voice_event_t *voice_event);

    pthread_mutex_t event_mutex;
    pthread_mutex_t speech_mutex;
    pthread_mutex_t siren_mutex;
    pthread_cond_t event_cond;
    pthread_t event_thread;
    pthread_t response_thread;

    shared_ptr<Speech> _speech;
    shared_ptr<CallbackProxy> callback;
    shared_ptr<VoiceConfig> voice_config;
    list<voice_event_t*> _events;

    string appid;
    int vt_start;
    int vt_end;
    float vt_energy;
    string vt_data;
    bool has_vt = false;
    bool openSiren = true;
};

#endif // VOICE_SERVICE_H
