package rokid.tts;

/** @hide */
interface ITtsCallback {

	void onStart(int id);

	void onCancel(int id);

	void onComplete(int id);

	void onError(int id, int err);
}
