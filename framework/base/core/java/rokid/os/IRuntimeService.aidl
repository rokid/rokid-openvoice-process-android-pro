package rokid.os;

import android.os.Bundle;

interface IRuntimeService{
	void nativeNlpMessage(String nlp);
	void setSirenState(int state);
	void receiveNlpMessage(in Bundle bundle);
}
