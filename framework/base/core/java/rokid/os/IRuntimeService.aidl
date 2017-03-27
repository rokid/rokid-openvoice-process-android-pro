package rokid.os;

import android.os.Bundle;

interface IRuntimeService{
	void nativeNlpMessage(String nlp);
	void setSirenState(int state);
	int getSirenState();
	void receiveNlpMessage(in Bundle bundle);
	void bindService();
}
