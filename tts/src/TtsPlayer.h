#ifndef TTS_PLAYER_H
#define TTS_PLAYER_H

#include "TtsOpus.h"
#include <memory>
#include <utils/Log.h>

#include <media/AudioTrack.h>
#include <system/audio.h>

using namespace std;
using namespace android;

class TtsPlayer {
public:

    TtsPlayer();
    ~TtsPlayer();
    void play(const char* data, size_t length);

private:

    int channels = 1;
    OpusSampleRate SampleRate = SR_24K;
    OPUS_APPLICATION Application = AUDIO;
    
    shared_ptr<TtsOpus> _Opus;
    sp<AudioTrack> lpTrack;
};

#endif

