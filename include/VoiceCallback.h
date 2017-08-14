#ifndef VOICE_CALLBACK_H
#define VOICE_CALLBACK_H

#include "IVoiceCallback.h"

class VoiceCallback: public BnVoiceCallback{
public:

    static constexpr int SPEECH_TIMEOUT = 3;
    static constexpr int SERVICE_UNAVAILABLE = 6;

    static constexpr int EVENT_VAD_ATART = 100;
    static constexpr int EVENT_VAD_DATA = 101;
    static constexpr int EVENT_VAD_END = 102;
    static constexpr int EVENT_VAD_CANCEL = 103;
    static constexpr int EVENT_WAKE_NOCMD = 108;
    static constexpr int EVENT_WAKE_CMD = 109;
    static constexpr int EVENT_SLEEP = 111;

    void voice_command(const string& asr, const string& nlp, const string& action);

    void voice_event(int event, bool has_sl, double sl, double energy, double threshold);

    void arbitration(const string& extra) = 0;

    void speech_error(int errcode);
};
