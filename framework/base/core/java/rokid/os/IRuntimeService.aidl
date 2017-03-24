package rokid.os;

interface IRuntimeService{
	void nativeNlpMessage(String nlp);
	void setSirenState(int state);
	int getSirenState();
	void receiveNlpMessage(String s);
	void bindService();
}
