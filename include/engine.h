#ifndef VOICE_ENGINE_H
#define VOICE_ENGINE_H

#include "VoiceService.h"

#if defined(__ANDROID__) || defined(ANDROID)
#include "mic/mic_array.h"
#else
#include <hardware/mic_array.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

bool _init_siren(VoiceService*);

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

void voice_event_callback(void *, voice_event_t *);

#ifdef __cplusplus
}
#endif

#endif // VOICE_ENGINE_H
