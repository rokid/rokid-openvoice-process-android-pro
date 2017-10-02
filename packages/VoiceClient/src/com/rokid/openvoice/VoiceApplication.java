package com.rokid.openvoice;

import android.content.Context;

public class VoiceApplication extends android.app.Application{

    private static Context mContext = null;

    static{
        VoiceManager.asInstance().init();
    }

    @Override
    public void onCreate() {
        mContext = this;
    }

    public static Context getContext(){
        return mContext;
    }
}
