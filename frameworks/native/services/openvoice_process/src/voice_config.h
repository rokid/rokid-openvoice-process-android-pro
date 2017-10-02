#ifndef VOICE_CONFIG_H
#define VOICE_CONFIG_H

#define OPENVOICE_PREFILE "/system/etc/openvoice_profile.json"

#include <cutils/log.h>
#include <string>

#include "json.h"
#include "speech.h"

using namespace std;
using namespace rokid::speech;

class VoiceConfig{
private:
    std::string _device_id;
    std::string _device_type_id;
    std::string _key;
    std::string _secret;
    std::string _vad_mode;

    static Lang lang_str2i(const char* str) {
        if (strcmp(str, "en") == 0)
            return Lang::EN;
        return Lang::ZH;
    }
    
    static Codec codec_str2i(const char* str) {
        if (strcmp(str, "opu") == 0)
            return Codec::OPU;
        // speech not support "opu2"
        return Codec::PCM;
    }

    static VadMode vadmode_str2i(const char* str) {
        if (strcmp(str, "cloud") == 0)
            return VadMode::CLOUD;
        return VadMode::LOCAL;
    }

public:
    bool prepare(shared_ptr<Speech> _speech){

        json_object *json_obj = json_object_from_file(OPENVOICE_PREFILE);
    
        if(json_obj == NULL) {
            ALOGE("%s cannot find", OPENVOICE_PREFILE);
            return false;
        }
        PrepareOptions preopts;
        shared_ptr<SpeechOptions> spopts = SpeechOptions::new_instance();

        json_object *host, *port, *branch, *auth_key, *device_type_id, *device_id, 
                                *secret, *lang, *codec, *vad_mode, *vend_timeout;
    
        if(TRUE == json_object_object_get_ex(json_obj, "host", &host))
            preopts.host = json_object_get_string(host);
        if(TRUE == json_object_object_get_ex(json_obj, "port", &port))
            preopts.port = json_object_get_int(port);
        if(TRUE == json_object_object_get_ex(json_obj, "branch", &branch))
            preopts.branch = json_object_get_string(branch);

        if(_key.empty()){
            if(TRUE == json_object_object_get_ex(json_obj, "key", &auth_key))
                preopts.key = json_object_get_string(auth_key);
        }else{
            preopts.key = _key;
        }
        if(_device_type_id.empty()){
            if(TRUE == json_object_object_get_ex(json_obj, "device_type_id", &device_type_id))
                preopts.device_type_id = json_object_get_string(device_type_id);
        }else{
            preopts.device_type_id = _device_type_id;
        }
        if(_device_id.empty()){
            if(TRUE == json_object_object_get_ex(json_obj, "device_id", &device_id))
                preopts.device_id = json_object_get_string(device_id);
        }else{
            preopts.device_id = _device_id;
        }
        if(_secret.empty()){
            if(TRUE == json_object_object_get_ex(json_obj, "secret", &secret))
                preopts.secret = json_object_get_string(secret);
        }else{
            preopts.secret = _secret;
        }

        if(TRUE == json_object_object_get_ex(json_obj, "lang", &lang))
            spopts->set_lang(lang_str2i(json_object_get_string(lang)));
        if(TRUE == json_object_object_get_ex(json_obj, "codec", &codec))
            spopts->set_codec(codec_str2i(json_object_get_string(codec)));
        if(TRUE == json_object_object_get_ex(json_obj, "vad_mode", &vad_mode)){
            _vad_mode = json_object_get_string(vad_mode);
            if(TRUE == json_object_object_get_ex(json_obj, "vend_timeout", &vend_timeout))
                spopts->set_vad_mode(vadmode_str2i(_vad_mode.c_str()), json_object_get_int(vend_timeout));
        }
        if(!_speech->prepare(preopts))
            return false;
        _speech->config(spopts);
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

    bool cloud_vad_enable(){
        if (_vad_mode == "cloud")
            return true;
        return false;
    }
};

#endif // VOICE_CONFIG_H
