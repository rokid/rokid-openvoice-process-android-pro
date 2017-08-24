#define LOG_TAG "TtsService"
#define LOG_NDEBUG 0

#include <sys/prctl.h>

#include "TtsService.h"

TtsService::TtsService() {
	pthread_mutex_init(&mutex, NULL);
    _player = make_shared<TtsPlayer>();
    _tts_config = make_shared<TtsConfig>();
	_tts = new_tts();
}

int TtsService::speak(const string& content, sp<IBinder> &callback) {
	pthread_mutex_lock(&mutex);
	if(!prepared) {
        ALOGV("speak not prepare");
	    pthread_mutex_unlock(&mutex);
        return -1;
    }

	int id = _tts->speak(content.c_str());

	ALOGD("tts speak begin id: = %d", id);

	if (id > 0)
		hmap.insert(pair<int, shared_ptr<CallbackProxy> >(id, make_shared<CallbackProxy>(callback)));

	pthread_mutex_unlock(&mutex);
	return id;
}

void TtsService::cancel(int id) {
	pthread_mutex_lock(&mutex);
    if(!prepared) {
        ALOGV("prepared is false");
	    pthread_mutex_unlock(&mutex);
        return;
    }
    _tts->cancel(id);
	pthread_mutex_unlock(&mutex);
}

bool TtsService::is_speaking(int id){
    // XXX
    return false;
}

void TtsService::set_volume(int volume){
    // XXX
}

bool TtsService::prepare() {
	pthread_mutex_lock(&mutex);

	if(prepared)
        goto done;

	if (_tts_config->config(
                [&](const char* key, const char* value){_tts->config(key, value);}) 
                && _tts->prepare()) {
	    pthread_create(&poll_thread, NULL,
	    		[](void* token)->void* {return ((TtsService*)token)->PollEvent();},
	    		this);
	    prepared = true;
	}else{
		ALOGW("prepare failed");
	    pthread_mutex_unlock(&mutex);
		return false;
    }

done:
	pthread_mutex_unlock(&mutex);
    return true;
}

void* TtsService::PollEvent(){
    prctl(PR_SET_NAME, __FUNCTION__);
    TtsResult res;
    shared_ptr<CallbackProxy> callback;
    while (true) {
        if (!_tts->poll(res)) {
        	break;
        }
        auto it = hmap.find(res.id);
        ALOGW("type  %d", res.type);
        if(it == hmap.end())
            continue;
        
        callback = it->second;
        
        switch(res.type) {
        case TTS_RES_VOICE:
            _player->play(res.voice->data(), res.voice->size());
        	break;
        case TTS_RES_START:
        	callback->onStart(res.id);
        	break;
        case TTS_RES_END:
        	callback->onComplete(res.id);
        	break;
        case TTS_RES_CANCELLED:
        	callback->onCancel(res.id);
        	break;
        case TTS_RES_ERROR:
        	callback->onError(res.id, res.err);
        	break;
        }
        if(res.type == TTS_RES_CANCELLED || res.type == TTS_RES_CANCELLED || res.type == TTS_RES_ERROR){
        	hmap.erase(res.id);
        }
    }
    _tts->release();
    _tts.reset();
    ALOGW("exit !!");
    return NULL;
}
