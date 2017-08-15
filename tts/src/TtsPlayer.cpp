#include "TtsPlayer.h"

TtsPlayer::TtsPlayer() {

	_Opus = shared_ptr<TtsOpus>(new TtsOpus((int)SampleRate, channels, 16000, (int)Application));
}

TtsPlayer::~TtsPlayer() {

}

void TtsPlayer::play(const char* data, size_t length) {
    int error;
    char* pcm_out;
    uint32_t pcm_out_length;
    pcm_out_length = _Opus->native_opus_decode(_Opus->decoder, data, length, pcm_out);
}
