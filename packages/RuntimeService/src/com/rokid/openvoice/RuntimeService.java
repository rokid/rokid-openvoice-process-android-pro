package com.rokid.openvoice;

import android.content.Intent;
import android.content.Context;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.net.NetworkInfo;
import android.net.ConnectivityManager;

public class RuntimeService extends android.app.Service{

    String TAG = getClass().getSimpleName();
    
    public RuntimeNative mRuntimeNative = null;
    public static MainHandler mHandler = null;
    public static final int MSG_REINIT = 1;
    
    public static boolean initialized = false;
    
    public static final int SIREN_STATE_AWAKE = 1;
    public static final int SIREN_STATE_SLEEP = 2;
    
    /**
     * @see #onVoiceEvent(int, double, bool, double, double)
     */
    private static final int EVENT_VAD_ATART = 100;
    private static final int EVENT_VAD_DATA = 101;
    private static final int EVENT_VAD_END = 102;
    private static final int EVENT_VAD_CANCEL = 103;
    private static final int EVENT_WAKE_NOCMD = 108;
    private static final int EVENT_WAKE_CMD = 109;
    private static final int EVENT_SLEEP = 111;
    
    class MainHandler extends Handler{
    
        public void handleMessage(Message msg) {
            switch(msg.what){
                case MSG_REINIT:
                    reinit();
                    break;
            }
        }
    }

    public RuntimeService(){
        Log.e(TAG, "RuntimeService  created ");
        mRuntimeNative = RuntimeNative.asInstance();
        mRuntimeNative.init();
        mRuntimeNative.addBinder(proxy);
        initialized = true;
    }

    @Override
    public void onCreate(){
        mHandler = new MainHandler();
        ConnectivityManager cm = (ConnectivityManager)getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo mNetworkInfo = cm.getActiveNetworkInfo();
        if(mNetworkInfo != null){
            mRuntimeNative.networkStateChange(true);
        }
        mUEventObserver.startObserving("/sound/card1/pcmC1D0c");
    }

    private void reinit(){
        Log.e(TAG, "==========================REINITT=============================");
        mRuntimeNative = RuntimeNative.asInstance();
        mRuntimeNative.init();
        mRuntimeNative.addBinder(proxy);
        initialized = true;
        ConnectivityManager cm = (ConnectivityManager)getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo mNetworkInfo = cm.getActiveNetworkInfo();
        if(mNetworkInfo != null){
            mRuntimeNative.networkStateChange(true);
        }
    }

    private final IRuntimeService.Stub proxy = new IRuntimeService.Stub(){

        /**
         *  使用激活词+指令或设备激活后，输入一段语音, 此方法将会返回语音识别后的结果
         *  如"若琪，打开我要喝咖啡"，或在激活状态说"打开我要喝咖啡"
         *  注意:有些情况不会调用此接口
         *  @see #onVoiceReject()
         *  @see #onSpeechTimeout()
         *
         *  @param asr 语音识别结果
         *  @param nlp 语义理解
         *  @param action 
         *  @return Nothing
         */
        @Override
        public void onVoiceCommand(String asr, String nlp, String action){
            mRuntimeNative.setSirenState(SIREN_STATE_AWAKE);
            Log.e(TAG, "asr\t" + asr);
            Log.e(TAG, "nlp\t" + nlp);
            Log.e(TAG, "action " + action);
        }

        /**
         * 使用激活词，如果本地判断为激活词,会产生{@link #EVENT_WAKE_NOCMD}事件
         * 每一段语音都将产生{@link #EVENT_VAD_ATART}, {@link #EVENT_VAD_DATA}
         *                  {@link #EVENT_VAD_END}, or {@link #EVENT_VAD_CANCEL}
         * 如果本地本地判断为误激活，会产生{@link #EVENT_VAD_CANCEL}事件
         *
         * @param event 语音事件{@link #EVENT_WAKE_NOCMD}, {@link #EVENT_WAKE_CMD}
         *                      {@link #EVENT_VAD_ATART}, {@link #EVENT_VAD_DATA}
         *                      {@link #EVENT_VAD_END}, {@link #EVENT_VAD_CANCEL}
         *                      {@link #EVENT_SLEEP}
         * @param sl_degree 声音角度值
         * @param has_sl 是否有角度值
         * @param energy 声音能量值
         * @param threshold 
         * @return Nothing
         */
        @Override
        public void onVoiceEvent(int event, double sl_degree, boolean has_sl, double energy, double threshold){
            Log.e(TAG, event+" ,has_sl : " + has_sl + " ,sl_degree : " + (float)sl_degree);
            if(event == EVENT_VAD_ATART){
            
            }else if(event == EVENT_VAD_END || event == EVENT_VAD_CANCEL){
            	mRuntimeNative.setSirenState(SIREN_STATE_SLEEP);
            }
        }

        /**
         * 设备被仲裁，此接口被调用
         * 并且会设置拾音状态为{@link SIREN_STATE_SLEEP}
         * @return Nothing
         */
        @Override
        public void onVoiceReject(){

        }

        /**
         * speech 发生网络超时将调用此接口
         * @return Nothing
         */
        @Override
        public void onSpeechTimeout(){

        }
    };

    private final android.os.UEventObserver mUEventObserver = new android.os.UEventObserver() {
		 
        @Override
        public void onUEvent(android.os.UEventObserver.UEvent event){
            Log.e(TAG, event.toString());
            if(initialized){
                String action = event.get("ACTION");
                if("add".equals(action)){
                	mRuntimeNative.startSiren(true);	
                }else if("remove".equals(action)){
                	mRuntimeNative.startSiren(false);
                }
            }
        }
    };

	@Override
	public android.os.IBinder onBind(Intent intent) {
		return null;
	}
}
