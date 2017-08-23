#ifndef TTS_CONFIG_H
#define TTS_CONFIG_H

#define OPENVOICE_PREFILE "/system/etc/openvoice_profile.json"

#include <cutils/log.h>
#include <memory>

#include "json.h"
#include "tts.h"

#ifdef __cplusplus
extern "C" {
#endif

bool config(const shared_ptr<rokid::speech::Tts>& _tts){
    json_object *json_obj = json_object_from_file(OPENVOICE_PREFILE);

    if(json_obj == NULL) {
        ALOGE("%s cannot find", OPENVOICE_PREFILE);
        return false;
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
    return true;
}

bool updata_config(){
    return true;
}

#ifdef __cplusplus
}
#endif
#endif // TTS_CONFIG_H
