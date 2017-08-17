#define LOG_TAG "TtsService"
#define LOG_NDEBUG 0

#include "TtsPlayer.h"

static void audioCallback(int event, void* user, void *info) {
    ALOGV("%s", __FUNCTION__);
}

TtsPlayer::TtsPlayer() {

	_Opus = shared_ptr<TtsOpus>(new TtsOpus((int)SampleRate, channels, 16000, (int)Application));

    size_t frameCount;

    lpTrack = new AudioTrack();
    status_t status = lpTrack->set(
            AUDIO_STREAM_MUSIC,
            24000,
            AUDIO_FORMAT_PCM_16_BIT,
            AUDIO_CHANNEL_OUT_MONO,
            0,
            AUDIO_OUTPUT_FLAG_FAST,
            audioCallback, this, //callback callback
            0,
            0,
            true,
            AUDIO_SESSION_ALLOCATE, //sessionId
            AudioTrack::TRANSFER_SYNC);
    if (status != NO_ERROR) {
        ALOGE("Error %d initializing AudioTrack", status);
        return;
    }
}

TtsPlayer::~TtsPlayer() {

}

void TtsPlayer::play(const char* data, size_t length) {
    int error;
    char* pcm_out;
    uint32_t pcm_out_length;
    ssize_t offset = 0;
    ssize_t written;
    pcm_out_length = _Opus->native_opus_decode(_Opus->decoder, data, length, pcm_out);

    lpTrack->stop();
    lpTrack->start();
    while(offset < pcm_out_length){
        written = lpTrack->write(pcm_out + offset, pcm_out_length - offset);
        offset += written;
        ALOGE("%s written %d, offset %d, pcm_out %d", __FUNCTION__, written, offset, pcm_out_length);
        if(written < 0) break;
    }
    delete[] pcm_out;
}
