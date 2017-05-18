package com.rokid.openvoice;

interface IRuntimeService{
	void onResponse(String asr, String nlp, String action, int type);
	void onEvent(int event, double sl_degree, int has_sl);
}
