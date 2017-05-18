package com.rokid.openvoice;

interface IRuntimeService{
	void nativeNlpMessage(String asr, String nlp, String action, int type);
	void sirenEvent(int event, double sl_degree, int has_sl);
}
