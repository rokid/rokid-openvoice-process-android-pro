package com.rokid.openvoice;

interface IRuntimeService{
	void onVoiceCommand(String asr, String nlp, String action);
	void onVoiceEvent(int event, double sl_degree, int has_sl, double energy, double threshold);
    void onVoiceReject();
    void onSpeechTimeout();
}
