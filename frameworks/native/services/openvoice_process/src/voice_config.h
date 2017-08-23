#ifndef VOICE_CONFIG_H
#define VOICE_CONFIG_H

#define OPENVOICE_PREFILE "/system/etc/openvoice_profile.json"

#include <cutils/log.h>
#include <string>
#include <memory>

#include "json.h"
#include "speech.h"

#ifdef __cplusplus
extern "C" {
#endif

std::string _device_id;
std::string _device_type_id;
std::string _key;
std::string _secret;

bool config(const shared_ptr<rokid::speech::Speech>& _speech){
    json_object *json_obj = json_object_from_file(OPENVOICE_PREFILE);

    if(json_obj == NULL) {
        ALOGE("%s cannot find", OPENVOICE_PREFILE);
        return false;
    }
    json_object *host, *port, *branch, *ssl_roots_pem, *auth_key, 
                            *device_type_id, *device_id, *secret, *api_version;

    if(TRUE == json_object_object_get_ex(json_obj, "host", &host))
        _speech->config("host", json_object_get_string(host));
    if(TRUE == json_object_object_get_ex(json_obj, "port", &port))
        _speech->config("port", json_object_get_string(port));
    if(TRUE == json_object_object_get_ex(json_obj, "branch", &branch))
        _speech->config("branch", json_object_get_string(branch));
    if(TRUE == json_object_object_get_ex(json_obj, "ssl_roots_pem", &ssl_roots_pem))
        _speech->config("ssl_roots_pem", json_object_get_string(ssl_roots_pem));
    if(TRUE == json_object_object_get_ex(json_obj, "api_version", &api_version))
        _speech->config("api_version", json_object_get_string(api_version));

    if(_device_id.empty()){
        if(TRUE == json_object_object_get_ex(json_obj, "device_id", &device_id))
        _speech->config("device_id", json_object_get_string(device_id));
    }else{
        _speech->config("device_id", _device_id.c_str());
    }
    if(_device_type_id.empty()){
        if(TRUE == json_object_object_get_ex(json_obj, "device_type_id", &device_type_id))
        _speech->config("device_type_id", json_object_get_string(device_type_id));
    }else{
        _speech->config("device_type_id", _device_type_id.c_str());
    }
    if(_key.empty()){
        if(TRUE == json_object_object_get_ex(json_obj, "key", &auth_key))
        _speech->config("key", json_object_get_string(auth_key));
    }else{
        _speech->config("key", _key.c_str());
    }
    if(_secret.empty()){
        if(TRUE == json_object_object_get_ex(json_obj, "secret", &secret))
        _speech->config("secret", json_object_get_string(secret));
    }else{
        _speech->config("secret", _secret.c_str());
    }
    _speech->config("codec", "opu");
    json_object_put(json_obj);
    return true;
}

bool save_config(const std::string& device_id, const std::string& device_type_id,
                                    const std::string& key, const std::string& secret) {

    ALOGI("dev_id = %s dev_t_id = %s key = %s secret = %s", device_id.c_str(), 
            device_type_id.c_str(), key.c_str(), secret.c_str());

    if (_device_id.empty()) _device_id = device_id;
    if (_device_type_id.empty()) _device_type_id = device_type_id;
    if (_key.empty()) _key = key;
    if (_secret.empty()) _secret = secret;
    return true;
}

#ifdef __cplusplus
}
#endif
#endif // VOICE_CONFIG_H
