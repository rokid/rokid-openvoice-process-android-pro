package com.rokid.openvoice;

import android.content.Intent;
import android.content.Context;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.net.NetworkInfo;
import android.net.ConnectivityManager;

import com.rokid.openvoice.entity.VoiceCommand;
import com.rokid.openvoice.entity.VoiceEvent;

import org.json.JSONObject;
import org.json.JSONException;

public class VoiceService extends android.app.Service {

    String TAG = getClass().getSimpleName();

    public VoiceNative mVoiceNative = null;
    public static MainHandler mHandler = null;

    public static final int MSG_REINIT = 0;
    public static final int MSG_TIMEOUT = 1;
    public static final int MSG_VOICE_COMMAND = 2;
    public static final int MSG_VOICE_EVENT = 3;
    public static final int MSG_ARBITRATION = 4;
    public static final int MSG_SPEECH_ERROR = 5;

    public static boolean initialized = false;

    public static final int SPEECH_TIMEOUT = 3;
    public static final int SERVICE_UNAVAILABLE = 6;

    public static final int SIREN_STATE_AWAKE = 1;
    public static final int SIREN_STATE_SLEEP = 2;

    private static final int DELAY = 15 * 1000;

    private static final int EVENT_VAD_ATART = 100;
    private static final int EVENT_VAD_DATA = 101;
    private static final int EVENT_VAD_END = 102;
    private static final int EVENT_VAD_CANCEL = 103;
    private static final int EVENT_WAKE_NOCMD = 108;
    private static final int EVENT_WAKE_CMD = 109;
    private static final int EVENT_SLEEP = 111;

    class MainHandler extends Handler {

        public void handleMessage(Message msg) {
            switch(msg.what) {
            case MSG_REINIT:
                handleReinit();
                break;
            case MSG_TIMEOUT:
                handleTimeout();
                break;
            case MSG_ARBITRATION:
                handleArbitration((String)msg.obj);
                break;
            case MSG_SPEECH_ERROR:
                handleSpeechError(msg.arg1);
                break;
            case MSG_VOICE_COMMAND:
                handleVoiceCommand((VoiceCommand)msg.obj);
                break;
            case MSG_VOICE_EVENT:
                handleVoiceEvent((VoiceEvent)msg.obj);
                break;
            }
        }
    }

    public VoiceService() {
        Log.e(TAG, "VoiceService  created ");
        mVoiceNative = VoiceNative.asInstance();
        mVoiceNative.init();
        mVoiceNative.registCallback(callback);
        initialized = true;
    }

    @Override
    public void onCreate() {
        mHandler = new MainHandler();
        ConnectivityManager cm = (ConnectivityManager)getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo mNetworkInfo = cm.getActiveNetworkInfo();
        if(mNetworkInfo != null) {
            mVoiceNative.networkStateChange(true);
        }
        try{
            mUEventObserver.startObserving("/sound/card1/pcmC1D0c");
        }catch(Exception e){
            e.printStackTrace();
        }
    }

    private void handleReinit() {
        Log.e(TAG, "+++++++++++++++++++++REINITT+++++++++++++++++++++");
        mVoiceNative = VoiceNative.asInstance();
        mVoiceNative.init();
        mVoiceNative.registCallback(callback);
        initialized = true;
        ConnectivityManager cm = (ConnectivityManager)getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo mNetworkInfo = cm.getActiveNetworkInfo();
        if(mNetworkInfo != null) {
            mVoiceNative.networkStateChange(true);
        }
    }

    private void handleTimeout(){
        mVoiceNative.setSirenState(SIREN_STATE_SLEEP);
    }

    private void handleVoiceCommand(VoiceCommand command){
        Log.e(TAG, "asr\t" + command.asr);
        Log.e(TAG, "nlp\t" + command.nlp);
        Log.e(TAG, "action " + command.action);
        String appId = "";
        try{
            appId = new JSONObject(command.nlp).getString("appId");
        }catch(JSONException e){
            e.printStackTrace();    
        }
        if(appId != null && appId.length() > 0 && !appId.equals("ROKID.EXCEPTION")){
		    mVoiceNative.updateStack(appId + ":");
        } 
        mHandler.removeMessages(MSG_TIMEOUT);
        mHandler.sendEmptyMessageDelayed(MSG_TIMEOUT, DELAY);
        mVoiceNative.setSirenState(SIREN_STATE_AWAKE);

    }

    private void handleVoiceEvent(VoiceEvent event){
        Log.e(TAG, event.event + " ,has_sl : " + event.has_sl + " ,sl : " + event.sl);
        if(event.event == EVENT_VAD_ATART) {

        } else if(event.event == EVENT_VAD_END) {
            mVoiceNative.setSirenState(SIREN_STATE_SLEEP);
        }
    }

    private void handleArbitration(String extra){
        if("accept".equals(extra)){
            mHandler.removeMessages(MSG_TIMEOUT);
            mHandler.sendEmptyMessageDelayed(MSG_TIMEOUT, DELAY);
            mVoiceNative.setSirenState(SIREN_STATE_AWAKE);
        }
    }

    private void handleSpeechError(int errcode){
        if(errcode == SPEECH_TIMEOUT){
            mHandler.removeMessages(MSG_TIMEOUT);
            mHandler.sendEmptyMessageDelayed(MSG_TIMEOUT, DELAY);
            mVoiceNative.setSirenState(SIREN_STATE_AWAKE);
        }

    }

    private final IVoiceCallback.Stub callback = new IVoiceCallback.Stub() {

        @Override
        public void onVoiceCommand(String asr, String nlp, String action) {
            mHandler.obtainMessage(MSG_VOICE_COMMAND, new VoiceCommand(asr, nlp, action)).sendToTarget();
        }

        @Override
        public void onVoiceEvent(int event, boolean has_sl, double sl_degree, double energy, double threshold) {
            mHandler.obtainMessage(MSG_VOICE_EVENT, new VoiceEvent(event, has_sl, sl_degree, energy, threshold)).sendToTarget();
        }

        @Override
        public void onArbitration(String extra) {
            mHandler.obtainMessage(MSG_ARBITRATION, extra).sendToTarget();
        }

        @Override
        public void onSpeechError(int errcode) {
            mHandler.obtainMessage(MSG_SPEECH_ERROR, errcode, -1).sendToTarget();
        }
    };

    private final android.os.UEventObserver mUEventObserver = new android.os.UEventObserver() {

        @Override
        public void onUEvent(android.os.UEventObserver.UEvent event) {
            Log.e(TAG, event.toString());
            if(initialized) {
                String action = event.get("ACTION");
                if("add".equals(action)) {
                    mVoiceNative.startSiren(true);
                } else if("remove".equals(action)) {
                    mVoiceNative.startSiren(false);
                }
            }
        }
    };

    @Override
    public android.os.IBinder onBind(Intent intent) {
        return null;
    }
}
