package rokid.tts;

public abstract class TtsCallback {
	// tts service not connected to tts server now
	// you can try speak later
	public static final int ERROR_SERVER_UNAVAILABLE = 1;
	// rktts service send request to tts server
	// but server not response, timeout
	public static final int ERROR_REQUEST_TIMEOUT = 2;
	// rktts service send request to tts server
	// tts server return failed, don't know why
	public static final int ERROR_SERVER_FAILED = 3;

	public abstract void onStart(int id);

	public abstract void onCancel(int id);

	public abstract void onComplete(int id);

	public abstract void onError(int id, int err);
}
