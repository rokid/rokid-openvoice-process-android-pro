package rokid.os;

import android.os.Bundle;

interface IRuntimeService{
	void nativeNlpMessage(String nlp);
	void setSirenState(int state);
	void sirenEvent(int event, double sl_degree, double has_sl);
	void receiveNlpMessage(in Bundle bundle);
}
