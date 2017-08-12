package com.rokid.openvoice.entity;

public class VoiceEvent{

    public VoiceEvent(int event, boolean has_sl, double sl, double energy, double threshold){
        this.event = event;
        this.has_sl = has_sl;
        this.sl = sl;
        this.energy = energy;
        this.threshold = threshold;
    }

    public int event;
    public boolean has_sl;
    public double sl;
    public double energy;
    public double threshold;
}
