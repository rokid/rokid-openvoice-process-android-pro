package com.rokid.openvoice.entity;

public class VoiceCommand{

    public VoiceCommand(String asr, String nlp, String action){
        this.asr = asr;
        this.nlp = nlp;
        this.action = action;
    }

    public String asr;

    public String nlp;

    public String action;
}
