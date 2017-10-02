package com.rokid.openvoice;

import android.content.Intent;
import android.content.Context;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.net.NetworkInfo;
import android.net.ConnectivityManager;

import org.json.JSONObject;
import org.json.JSONException;

//import rokid.tts.Tts;

public class VoiceService extends android.app.Service {

    String TAG = getClass().getSimpleName();

    public VoiceManager mVoiceManager = null;
    public static MainHandler mHandler = null;

    private static final int DELAY                      = 15 * 1000;

    public static final int MSG_REINIT                  = 0;
    public static final int MSG_TIMEOUT                 = 1;

    private static final int VOICE_COMING               = 0;
    private static final int VOICE_LOCAL_WAKE           = 1;
    private static final int VOICE_START                = 2;
    private static final int VOICE_ACCEPT               = 3;
    private static final int VOICE_REJECT               = 4;
    private static final int VOICE_CANCEL               = 5;
    private static final int VOICE_LOCAL_SLEEP          = 6;

    private static final int SPEECH_SERVER_INTERNAL     = 6;
    private static final int SPEECH_VAD_TIMEOUT         = 7;
    private static final int SPEECH_SERVICE_UNAVAILABLE = 101;
    private static final int SPEECH_TIMEOUT             = 103;

    class MainHandler extends Handler {

        public void handleMessage(Message msg) {
            switch(msg.what) {
            case MSG_REINIT:
                handleReinit();
                break;
            case MSG_TIMEOUT:
                handleTimeout();
                break;
            }
        }
    }

    public VoiceService() {
        Log.e(TAG, "VoiceService  created ");
        mVoiceManager = VoiceManager.asInstance();
        mVoiceManager.registCallback(callback);
    }

    @Override
    public void onCreate() {
        mHandler = new MainHandler();
        ConnectivityManager cm = (ConnectivityManager)getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo mNetworkInfo = cm.getActiveNetworkInfo();
        if(mNetworkInfo != null) {
            mVoiceManager.networkStateChange(true);
        }
        try{
            mUEventObserver.startObserving("/sound/card1/pcmC1D0c");
        }catch(Exception e){
            e.printStackTrace();
        }

//        Tts _tts = new Tts();
//        if(_tts.speak("测试", null) <= 0){
//            //tts speak faild
//        }
    }

    private void handleReinit() {
        Log.e(TAG, "+++++++++++++++++++++REINITT+++++++++++++++++++++");
        mVoiceManager = VoiceManager.asInstance();
        mVoiceManager.init();
        mVoiceManager.registCallback(callback);
        ConnectivityManager cm = (ConnectivityManager)getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo mNetworkInfo = cm.getActiveNetworkInfo();
        if(mNetworkInfo != null) {
            mVoiceManager.networkStateChange(true);
        }
    }

    private void handleTimeout(){
        mVoiceManager.setSirenState(VoiceManager.SIREN_STATE_SLEEP);
    }

    private final IVoiceCallback.Stub callback = new IVoiceCallback.Stub() {

        @Override
        public void onVoiceEvent(int id, int event, double sl, double energy) {
            Log.e(TAG, event + " ,sl : " + sl);
            if(event == VOICE_COMING){

            }
        }

        @Override
        public void onIntermediateResult(int id, int type, String asr) {

        }

        @Override
        public void onVoiceCommand(int id, String asr, String nlp, String action) {
            Log.e(TAG, "asr\t" + asr);
            Log.e(TAG, "nlp\t" + nlp);
            Log.e(TAG, "action " + action);
            String appId = "";
            try{
                appId = new JSONObject(nlp).getString("appId");
            }catch(JSONException e){
                e.printStackTrace();    
            }
            if(appId != null && appId.length() > 0 && !appId.equals("ROKID.EXCEPTION")){
    		    mVoiceManager.updateStack(appId + ":");
            } 
        }

        @Override
        public void onSpeechError(int id, int errcode) {

        }

        @Override
        public String getSkillOptions() {
            return "{}";
        }
    };

    private final android.os.UEventObserver mUEventObserver = new android.os.UEventObserver() {

        @Override
        public void onUEvent(android.os.UEventObserver.UEvent event) {
            Log.e(TAG, event.toString());
            String action = event.get("ACTION");
            if("add".equals(action)) {
                mVoiceManager.startSiren(true);
            } else if("remove".equals(action)) {
                mVoiceManager.startSiren(false);
            }
        }
    };

    @Override
    public android.os.IBinder onBind(Intent intent) {
        return null;
    }
}
