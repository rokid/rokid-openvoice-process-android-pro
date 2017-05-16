#ifndef VOICE_ENGINE_H
#define VOICE_ENGINE_H

#include "RuntimeService.h"
#include "mic/mic_array.h"

#ifndef __cplusplus
extern "C" {
#endif

bool _init(RuntimeService*);

void set_siren_state_change(int state);

int init_input(void*);

void release_input(void*);

void start_input(void*);

void stop_input(void*);

int read_input(void*, char*, int);

void on_err_input(void*);

void state_changed_callback(void*, int);

void voice_event_callback(void* token, int length, siren_event_t event, 
		void* buff, int has_sl,
		int has_voice, double sl_degree, double energy, double threshold,
		int has_voiceprint);

#ifndef __cplusplus
}
#endif

#endif // VOICE_ENGINE_H
