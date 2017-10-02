#ifndef VOICE_SERVICE_H
#define VOICE_SERVICE_H

#define EXTRA_ACCEPT "accept"
#define EXTRA_NONE "none"

#include <cutils/log.h>
#include <pthread.h>
#include <stdlib.h>
#include <list>

#include "siren.h"
#include "speech.h"
#include "voice_config.h"
#include "IVoiceService.h"
#include "CallbackProxy.h"
#include "EventTypes.h"

using namespace rokid::speech;
using namespace openvoice_process;

class VoiceService: public BnVoiceService {

public:

    static char const* getServiceName() {
        return "openvoice_process";
    }

    VoiceService();

    bool init();
    void start_siren(const bool);
    void set_siren_state(const int);
    void network_state_change(const bool);
    void update_stack(const string&);
    void update_config(const string&, const string&, const string&, const string&);
    void regist_callback(const sp<IBinder>&);

//    int32_t insert_vt_word(const vt_word_t&);
//    int32_t delete_vt_word(const string&);
//    int32_t query_vt_word(vector<vt_word_t>& _vt_words_in);

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

    static inline int32_t transform_string_to_event(const std::string& extra){
        if(extra == EXTRA_ACCEPT){
            return VoiceEvent::VOICE_ACCEPT;
        }else if(extra == EXTRA_NONE){
            return VoiceEvent::VOICE_NONE;
        }else{
            return VoiceEvent::VOICE_REJECT;
        }
    }

    inline void clear(const int32_t id = -1){
        pthread_mutex_lock(&session_mutex);
        if(id < 0 || session_id == id) session_id = -1;
        pthread_mutex_unlock(&session_mutex);
    }

	void* onEvent();
	void* onResponse();

    int32_t vad_start();
    void voice_print(const voice_event_t *);
	void voice_event_callback(voice_event_t *voice_event);

    pthread_mutex_t event_mutex;
    pthread_mutex_t speech_mutex;
    pthread_mutex_t siren_mutex;
    pthread_mutex_t session_mutex;
    pthread_cond_t event_cond;
    pthread_t event_thread;
    pthread_t response_thread;

    shared_ptr<Speech> _speech;
    shared_ptr<CallbackProxy> _callback;
    shared_ptr<VoiceConfig> _voice_config;
    list<voice_event_t*> _events;

    string appid;
    string vt_word;
    int32_t session_id;
    int32_t vt_start;
    int32_t vt_end;
    float vt_energy;
    bool has_vt = false;
    bool asr_finished = false;
    bool openSiren = true;
    bool local_sleep = false;
};

#endif // VOICE_SERVICE_H
