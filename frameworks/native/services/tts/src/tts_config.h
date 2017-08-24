#ifndef TTS_CONFIG_H
#define TTS_CONFIG_H

#define OPENVOICE_PREFILE "/system/etc/openvoice_profile.json"

#include <cutils/log.h>
#include <string>
#include "json.h"

class TtsConfig{

private:
    std::string _device_id;
    std::string _device_type_id;
    std::string _key;
    std::string _secret;

public:

    template<typename F>
    bool config(const F& writer){

        json_object *json_obj = json_object_from_file(OPENVOICE_PREFILE);
    
        if(json_obj == NULL) {
            ALOGE("%s cannot find", OPENVOICE_PREFILE);
            return false;
        }
        json_object *host, *port, *branch, *ssl_roots_pem, *auth_key, *device_type_id, 
                                        *device_id, *secret, *api_version;
    
        if(TRUE == json_object_object_get_ex(json_obj, "host", &host))
            writer("host", json_object_get_string(host));
        if(TRUE == json_object_object_get_ex(json_obj, "port", &port))
            writer("port", json_object_get_string(port));
        if(TRUE == json_object_object_get_ex(json_obj, "branch", &branch))
            writer("branch", json_object_get_string(branch));
        if(TRUE == json_object_object_get_ex(json_obj, "ssl_roots_pem", &ssl_roots_pem))
            writer("ssl_roots_pem", json_object_get_string(ssl_roots_pem));
        if(TRUE == json_object_object_get_ex(json_obj, "api_version", &api_version))
            writer("api_version", json_object_get_string(api_version));
        if(_device_id.empty()){
            if(TRUE == json_object_object_get_ex(json_obj, "device_id", &device_id))
            writer("device_id", json_object_get_string(device_id));
        }else{
            writer("device_id", _device_id.c_str());
        }
        if(_device_type_id.empty()){
            if(TRUE == json_object_object_get_ex(json_obj, "device_type_id", &device_type_id))
            writer("device_type_id", json_object_get_string(device_type_id));
        }else{
            writer("device_type_id", _device_type_id.c_str());
        }
        if(_key.empty()){
            if(TRUE == json_object_object_get_ex(json_obj, "key", &auth_key))
            writer("key", json_object_get_string(auth_key));
        }else{
            writer("key", _key.c_str());
        }
        if(_secret.empty()){
            if(TRUE == json_object_object_get_ex(json_obj, "secret", &secret))
            writer("secret", json_object_get_string(secret));
        }else{
            writer("secret", _secret.c_str());
        }
        writer("codec", "opu2");
        json_object_put(json_obj);
        return true;
    }
    
    bool save_config(const std::string& device_id, const std::string& device_type_id,
                                        const std::string& key, const std::string& secret) {

        ALOGI("dev_id = %s dev_t_id = %s key = %s secret = %s", device_id.c_str(), 
                device_type_id.c_str(), key.c_str(), secret.c_str());
    
        if (_device_id.empty() && !device_id.empty()) _device_id = device_id;
        if (_device_type_id.empty() && !device_type_id.empty()) _device_type_id = device_type_id;
        if (_key.empty() && !key.empty()) _key = key;
        if (_secret.empty() && !secret.empty()) _secret = secret;
        return true;
    }
};

#endif // TTS_CONFIG_H
