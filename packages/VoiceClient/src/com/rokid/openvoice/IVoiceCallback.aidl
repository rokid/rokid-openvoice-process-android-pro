package com.rokid.openvoice;

interface IVoiceCallback{
    void onVoiceEvent(int id, int event, double sl, double energy);
    void onIntermediateResult(int id, int type, String asr);
    void onVoiceCommand(int id, String asr, String nlp, String action);
    void onSpeechError(int id, int errcode);
    String getSkillOptions();
}
