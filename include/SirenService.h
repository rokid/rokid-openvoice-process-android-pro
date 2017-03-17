#ifndef SIREN_SERVICE_H
#define SIREN_SERVICE_H

#include "RuntimeService.h"
#include "mic/mic_array.h"

namespace siren{

class SirenService{
	public:
		bool init(RuntimeService*);
		RuntimeService* runtime_service;
	private:
		siren_t _siren;
		siren_state_changed_callback_t siren_state_change;
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
			int has_voice, double sl_degree,
			int has_voiceprint);

} // namespace siren

#endif // SIREN_SERVICE_H
