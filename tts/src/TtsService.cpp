#define LOG_TAG "TtsService"
#define LOG_NDEBUG 0

#include "TtsService.h"

TtsService::TtsService() {
	pthread_mutex_init(&event_mutex, NULL);
    _player = shared_ptr<TtsPlayer>(new TtsPlayer());
}

int TtsService::speak(const string& content, sp<IBinder> &callback) {
	if(!prepared) {
        ALOGV("speak prepared is false");
        return -1;
    }

	pthread_mutex_lock(&event_mutex);
	int id = _tts->speak(content.c_str());

	ALOGD("tts speak begin id: = %d", id);

	if (id > 0)
		hmap.insert(pair<int, sp<ITtsCallback> >(id, interface_cast<ITtsCallback>(callback)));
	pthread_mutex_unlock(&event_mutex);

	return id;
}

void TtsService::cancel(int id) {
    if(!prepared) {
        ALOGV("prepared is false");
        return;
    }
    _tts->cancel(id);
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
	if(prepared)
        return true;

	_tts = new_tts();

	this->config();

	if (!_tts->prepare()) {
		ALOGW("prepare failed");
		return false;
	}
	prepared = true;

    pthread_create(&poll_thread, NULL, PollEvent, this);
    return true;

}

void* PollEvent(void* args){
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
				//callback->onVoice(res.id, res.voice.get()->data(), res.voice.get()->size());
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
        if(res.type == TTS_RES_CANCELLED || res.type == TTS_RES_CANCELLED){
			tts->hmap.erase(res.id);
		}
	}
	tts->_tts->release();
    tts->_tts.reset();
	ALOGW("exit !!");
	return NULL;
}
