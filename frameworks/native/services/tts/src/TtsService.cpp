#define LOG_TAG "TtsService"
#define LOG_NDEBUG 0

#include <sys/prctl.h>
#include "TtsService.h"

TtsService::TtsService() {
	pthread_mutex_init(&mutex, NULL);
    _player = make_shared<TtsPlayer>();
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
		hmap.insert(pair<int, sp<ITtsCallback> >(id, interface_cast<ITtsCallback>(callback)));

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

void TtsService::config() {
    json_object *json_obj = json_object_from_file(OPENVOICE_PROFILE);

    if(!json_obj) {
        ALOGE("%s cannot find", OPENVOICE_PROFILE);
		return;
    }
    json_object *host, *port, *branch, *ssl_roots_pem, *auth_key, *device_type, *device_id, *secret, *api_version;

    if(TRUE == json_object_object_get_ex(json_obj, "host", &host))
        _tts->config("host", json_object_get_string(host));
    if(TRUE == json_object_object_get_ex(json_obj, "port", &port))
        _tts->config("port", json_object_get_string(port));
    if(TRUE == json_object_object_get_ex(json_obj, "branch", &branch))
        _tts->config("branch", json_object_get_string(branch));
    if(TRUE == json_object_object_get_ex(json_obj, "ssl_roots_pem", &ssl_roots_pem))
        _tts->config("ssl_roots_pem", json_object_get_string(ssl_roots_pem));
    if(TRUE == json_object_object_get_ex(json_obj, "key", &auth_key))
        _tts->config("key", json_object_get_string(auth_key));
    if(TRUE == json_object_object_get_ex(json_obj, "device_type_id", &device_type))
        _tts->config("device_type_id", json_object_get_string(device_type));
    if(TRUE == json_object_object_get_ex(json_obj, "device_id", &device_id))
        _tts->config("device_id", json_object_get_string(device_id));
    if(TRUE == json_object_object_get_ex(json_obj, "api_version", &api_version))
        _tts->config("api_version", json_object_get_string(api_version));
    if(TRUE == json_object_object_get_ex(json_obj, "secret", &secret))
        _tts->config("secret", json_object_get_string(secret));

	_tts->config("codec", "opu2");
    json_object_put(json_obj);
}

bool TtsService::prepare() {
	pthread_mutex_lock(&mutex);

	if(prepared)
        goto done;

	_tts = new_tts();
	this->config();

	if (!_tts->prepare()) {
		ALOGW("prepare failed");
	    pthread_mutex_unlock(&mutex);
		return false;
	}
    pthread_create(&poll_thread, NULL, PollEvent, this);
	prepared = true;

done:
	pthread_mutex_unlock(&mutex);
    return true;
}

void* PollEvent(void* args){
    prctl(PR_SET_NAME, __FUNCTION__);
    TtsResult res;
    sp<ITtsCallback> callback;
    TtsService *tts = (TtsService *)args;
    while (true) {
        if (!tts->_tts->poll(res)) {
        	break;
        }
        auto it = tts->hmap.find(res.id);
        ALOGW("type  %d", res.type);
        if(it == tts->hmap.end())
            continue;
        
        callback = it->second;
        
        switch(res.type) {
        case TTS_RES_VOICE:
            tts->_player->play(res.voice->data(), res.voice->size());
        	break;
        case TTS_RES_START:
            if(callback.get())
        	    callback->onStart(res.id);
        	break;
        case TTS_RES_END:
            if(callback.get())
        	    callback->onComplete(res.id);
        	break;
        case TTS_RES_CANCELLED:
            if(callback.get())
        	    callback->onCancel(res.id);
        	break;
        case TTS_RES_ERROR:
            if(callback.get())
        	    callback->onError(res.id, res.err);
        	break;
        }
        if(res.type == TTS_RES_CANCELLED || res.type == TTS_RES_CANCELLED || res.type == TTS_RES_ERROR){
        	tts->hmap.erase(res.id);
        }
    }
    tts->_tts->release();
    tts->_tts.reset();
    ALOGW("exit !!");
    return NULL;
}
