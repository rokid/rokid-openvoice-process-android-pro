#ifndef TTS_CALLBACK_H
#define TTS_CALLBACK_H

#include "ITtsCallback.h"

class TtsCallback: public BnTtsCallback{
public:

    static constexpr int TTS_TIMEOUT = 103;

	void onStart(int id);

	void onText(int id, const string& text);

	void onCancel(int id);

	void onComplete(int id);

	void onError(int id, int err);
};

#endif
