#ifndef VOICE_SERVICE_H
#define VOICE_SERVICE_H

#define SPEECH_CONFIG_FILE "/system/etc/openvoice_profile.json"

#include <cutils/log.h>
#include <pthread.h>
#include <stdlib.h>
#include <list>

#include "siren.h"
#include "speech.h"
#include "IVoiceService.h"

using namespace rokid;
using namespace speech;

class VoiceService: public BnVoiceService {
public:
    class DeathNotifier : public IBinder::DeathRecipient {
    public:
        DeathNotifier(VoiceService *service) : _service(service) {}
        ~DeathNotifier(){}
        void binderDied(const wp<IBinder> &binder) {_service->callback.clear();}
    private:
        VoiceService *_service = NULL;
    };

    static char const* getServiceName() {
        return "openvoice_process";
    }

    VoiceService();
    void send_voice_event(int, int, double, double, double);
    int vad_start();
    void voice_print(const voice_event_t *);

    pthread_mutex_t event_mutex;
    pthread_mutex_t speech_mutex;
    pthread_mutex_t siren_mutex;
    pthread_cond_t event_cond;
    pthread_t event_thread;
    pthread_t response_thread;

    shared_ptr<Speech> _speech;
    sp<IBinder> callback;
    list<voice_event_t*> message_queue;

private:
    bool init();
    void config();
    void start_siren(bool);
    void set_siren_state(const int);
    void network_state_change(bool);
    void update_stack(const string&);
    void regist_callback(const sp<IBinder>&);
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
    string appid;
    int vt_start;
    int vt_end;
    float vt_energy;
    string vt_data;
    bool has_vt = false;
    bool openSiren = true;
};

void* onEvent(void *);
void* onResponse(void *);

#endif // VOICE_SERVICE_H
