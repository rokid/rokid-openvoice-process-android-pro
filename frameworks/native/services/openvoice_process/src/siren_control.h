#ifndef __SIREN_CONTROL_H
#define __SIREN_CONTROL_H

#include <vector>

#include "mic/mic_array.h"
//#include "openvoice_process.pb.h"
#include "siren.h"

#ifdef __cplusplus
extern "C" {
#endif

bool setup(void*, on_voice_event_t);

//int32_t insert_vt_word_cmd(const vt_word_t&);

//int32_t delete_vt_word_cmd(const string&);

//int32_t query_vt_word_cmd(vector<vt_word_t>& _vt_words_in);

void set_siren_state_change(int state);

int init_input(void*);

void release_input(void*);

int start_input(void*);

void stop_input(void*);

int read_input(void*, char*, int);

void on_err_input(void*);

void state_changed_callback(void*, int);

void _start_siren_process_stream();

void _stop_siren_process_stream();

int find_card(const char*);

#ifdef __cplusplus
}
#endif

#endif // __SIREN_CONTROL_H
