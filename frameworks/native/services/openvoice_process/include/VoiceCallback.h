#ifndef VOICE_CALLBACK_H
#define VOICE_CALLBACK_H

#include "IVoiceCallback.h"

class VoiceCallback: public BnVoiceCallback{
public:

    void voice_event(const int32_t id, const int32_t event, const double sl, const double energy);

    void intermediate_result(const int32_t id, const int32_t type, const string& asr);

    void voice_command(const int32_t id, const string& asr, const string& nlp, const string& action);

    void speech_error(const int32_t id, const int32_t errcode);

    const string get_skill_options();
};

#endif
