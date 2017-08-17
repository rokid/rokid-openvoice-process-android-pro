#ifndef TTS_SERVICE_H
#define TTS_SERVICE_H

#define OPENVOICE_PROFILE "/system/etc/openvoice_profile.json"

#include <cutils/log.h>
#include <pthread.h>
#include <stdlib.h>
#include <map>

#include "TtsPlayer.h"
#include "ITtsService.h"
#include "ITtsCallback.h"
#include "json.h"
#include "tts.h"

using namespace rokid;
using namespace speech;

class TtsService : public BnTtsService{
public:
	static char const* getServiceName(){
		return "tts_process";
	}

	TtsService();

    bool is_speaking(int);
	int speak(const string&, sp<IBinder>&);
	void cancel(int id);
	void config();
    void set_volume(int);

	shared_ptr<Tts> _tts;
	shared_ptr<TtsPlayer> _player;

	map<int, sp<ITtsCallback> > hmap;

	pthread_t poll_thread;
	pthread_mutex_t mutex;

	bool prepared = false;

private:

	bool prepare();
};
void* PollEvent(void *);

#endif
