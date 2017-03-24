package rokid.os;

interface IRuntimeService{
	void nlpMessage(String nlp);
	void setSirenState(int state);
	int getSirenState();
	void receiveRemoteMessage(String s);
	void bindService();
}
