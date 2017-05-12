package rokid.os;

import android.os.Bundle;

interface IRuntimeService{
	void nativeNlpMessage(String asr, String nlp, String action, int type);
	void setSirenState(int state);
	void sirenEvent(int event, double sl_degree, int has_sl);
}
