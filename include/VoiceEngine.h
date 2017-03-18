#ifndef VOICE_ENGINE_H
#define VOICE_ENGINE_H

#include "RuntimeService.h"
#include "mic/mic_array.h"

namespace siren{

class VoiceEngine{
	public:
		bool init(RuntimeService*);
		void set_siren_state_change(int state);
		RuntimeService* runtime_service;
	private:
		siren_t _siren;
};

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

} // namespace siren

#endif // VOICE_ENGINE_H
