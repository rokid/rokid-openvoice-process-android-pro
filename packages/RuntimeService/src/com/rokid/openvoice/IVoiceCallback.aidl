package com.rokid.openvoice;

interface IVoiceCallback{
    void onVoiceCommand(String asr, String nlp, String action);
    void onVoiceEvent(int event, double sl_degree, boolean has_sl, double energy, double threshold);
    void onVoiceReject();
    void onSpeechTimeout();
}
