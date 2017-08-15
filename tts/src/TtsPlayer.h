#ifndef TTS_PLAYER_H
#define TTS_PLAYER_H

#include "TtsOpus.h"
#include <memory>
#include <utils/Log.h>

using namespace std;

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
};

#endif

