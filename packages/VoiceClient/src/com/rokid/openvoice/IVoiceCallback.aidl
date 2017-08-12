package com.rokid.openvoice;

interface IVoiceCallback{
    void onVoiceCommand(String asr, String nlp, String action);
    void onVoiceEvent(int event, boolean has_sl, double sl, double energy, double threshold);
    void onArbitration(String extra);
    void onSpeechError(int errcode);
}
