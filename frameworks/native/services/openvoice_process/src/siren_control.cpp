#define LOG_TAG "siren_control"
#define LOG_NDEBUG 0

#include <string.h>
#include <cutils/properties.h>
#include <hardware/hardware.h>

#include "VoiceService.h"
#include "siren_control.h"

struct mic_array_device_t *mic_array_device = NULL;

siren_proc_callback_t event_callback;
siren_t _siren;

siren_input_if_t siren_input = { init_input, release_input, start_input,
		stop_input, read_input, on_err_input };

siren_state_changed_callback_t siren_state_change = { state_changed_callback };

static inline int mic_array_device_open(const hw_module_t *module, struct mic_array_device_t **device) {
    return module->methods->open(module, MIC_ARRAY_HARDWARE_MODULE_ID, (struct hw_device_t **)device);
}

bool setup(void* token, on_voice_event_t callback) {
	event_callback.voice_event_callback = callback;
    //1. open mic driver.
    mic_array_module_t *module;
    if(hw_get_module(MIC_ARRAY_HARDWARE_MODULE_ID, (const struct hw_module_t **)&module) != 0) {
        ALOGV("cannot find mic_array");
        return false;
    }
    if(mic_array_device_open(&module->common, &mic_array_device) != 0) {
        ALOGE("open mic_array failed");
        return false;
    }
    ALOGI ("open mic array done");
    //2. init siren
    _siren = init_siren(token, NULL, &siren_input);	
    return true;
}

//int32_t insert_vt_word_cmd(const vt_word_t& _vt_word){
//    siren_vt_word vt;
//    vt.vt_phone = _vt_word.vt_phone();
//    vt.vt_type  = _vt_word.vt_type();
//    vt.vt_word  = _vt_word.vt_word();
//    vt.use_default_config = _vt_word.use_default_config();
//    if(!vt.use_default_config){
//        vt.alg_config.vt_block_avg_score = _vt_word.alg_config().vt_block_avg_score();
//        vt.alg_config.vt_classify_shield = _vt_word.alg_config().vt_classify_shield();
//        vt.alg_config.vt_block_min_score = _vt_word.alg_config().vt_block_min_score();
//        vt.alg_config.vt_left_sil_det = _vt_word.alg_config().vt_left_sil_det();
//        vt.alg_config.vt_right_sil_det = _vt_word.alg_config().vt_right_sil_det();
//        vt.alg_config.vt_remote_check_with_aec = _vt_word.alg_config().vt_remote_check_with_aec();
//        vt.alg_config.vt_remote_check_without_aec = _vt_word.alg_config().vt_remote_check_without_aec();
//        vt.alg_config.vt_local_classify_check = _vt_word.alg_config().vt_local_classify_check();
//        vt.alg_config.nnet_path = _vt_word.alg_config().nnet_path();
//    }
//    return add_vt_word(_siren, &vt, true);
//}
//
//int32_t delete_vt_word_cmd(const string& word){
//    return remove_vt_word(_siren, word.c_str());
//}
//
//int32_t query_vt_word_cmd(vector<vt_word_t>& _vt_words_in){
//
//    siren_vt_word *_vt_words_out = nullptr;
//
//    int32_t count = get_vt_word(_siren, &_vt_words_out);
//    if(count > 0 && _vt_words_out != nullptr){
//        _vt_words_in.reserve(count);
//        for(int i = 0; i < count; i++){
////            _vt_words_in.insert(_vt_words_in.begin() + i, vt_word_t());
//            _vt_words_in.push_back(vt_word_t());
//            _vt_words_in[i].set_vt_phone(_vt_words_out[i].vt_phone);
//            _vt_words_in[i].set_vt_type(_vt_words_out[i].vt_type);
//            _vt_words_in[i].set_vt_word(_vt_words_out[i].vt_word);
//            _vt_words_in[i].set_use_default_config(_vt_words_out[i].use_default_config);
//            _vt_words_in[i].mutable_alg_config()->set_vt_block_avg_score(_vt_words_out[i].alg_config.vt_block_avg_score);
//            _vt_words_in[i].mutable_alg_config()->set_vt_classify_shield(_vt_words_out[i].alg_config.vt_classify_shield);
//            _vt_words_in[i].mutable_alg_config()->set_vt_block_min_score(_vt_words_out[i].alg_config.vt_block_min_score);
//            _vt_words_in[i].mutable_alg_config()->set_vt_left_sil_det(_vt_words_out[i].alg_config.vt_left_sil_det);
//            _vt_words_in[i].mutable_alg_config()->set_vt_right_sil_det(_vt_words_out[i].alg_config.vt_right_sil_det);
//            _vt_words_in[i].mutable_alg_config()->set_vt_remote_check_with_aec(_vt_words_out[i].alg_config.vt_remote_check_with_aec);
//            _vt_words_in[i].mutable_alg_config()->set_vt_remote_check_without_aec(_vt_words_out[i].alg_config.vt_remote_check_without_aec);
//            _vt_words_in[i].mutable_alg_config()->set_vt_local_classify_check(_vt_words_out[i].alg_config.vt_local_classify_check);
//            _vt_words_in[i].mutable_alg_config()->set_nnet_path(_vt_words_out[i].alg_config.nnet_path);
//            ALOGV("%s\n", _vt_words_in[i].DebugString().c_str());
//        }
//    }
//    return count;
//}

void _start_siren_process_stream() {
    start_siren_process_stream(_siren, &event_callback);
}

void _stop_siren_process_stream() {
    stop_siren_process_stream(_siren);
}

void set_siren_state_change(int state) {
    set_siren_state(_siren, state, &siren_state_change);
}

int init_input(void *token) {
    return 0;
}

void release_input(void *token) {
    mic_array_device->finish_stream(mic_array_device);
}

int start_input(void *token) {
    return mic_array_device->start_stream(mic_array_device);
}

void stop_input(void *token) {
    mic_array_device->stop_stream(mic_array_device);
    ALOGV("%s", __FUNCTION__);
}

int read_input(void *token, char *buff, int	frame_cnt) {
    return mic_array_device->read_stream(mic_array_device, buff, frame_cnt);
}

int find_card(const char *snd) {
    return mic_array_device->find_card(snd);
}

void on_err_input(void *token) {
    ALOGV("%s", __FUNCTION__);
}

void state_changed_callback(void *token, int state) {
    ALOGV("%s : %d", __FUNCTION__, state);
}
