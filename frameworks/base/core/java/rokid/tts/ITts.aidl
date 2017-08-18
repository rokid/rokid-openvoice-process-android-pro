package rokid.tts;

import rokid.tts.ITtsCallback;

/** @hide */
interface ITts {

    boolean prepare();

	int speak(String content, ITtsCallback cb);

	void cancel(int id);
}
