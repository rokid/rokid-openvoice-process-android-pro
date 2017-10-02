#ifndef __AUDIO_RECORDER_H
#define __AUDIO_RECORDER_H

#include "mic/mic_array.h"
#include "siren.h"

#ifdef __cplusplus
extern "C" {
#endif

bool init(void*, on_voice_event_t);

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

#endif // __AUDIO_RECORDER_H
