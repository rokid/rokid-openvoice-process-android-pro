package com.rokid.openvoice;

import android.app.Service;
import android.content.Intent;
import android.content.Context;
import android.os.IBinder;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.net.NetworkInfo;
import android.net.ConnectivityManager;

public class RuntimeService extends Service{

	String TAG = getClass().getSimpleName();

	public RuntimeNative mRuntimeNative = null;
    public static MainHandler mHandler = null;
    public static final int MSG_NATIVE_SERVICE_DIED = 0;
    public static final int MSG_REINIT = 1;

	public static boolean initialized = false;

	public static final int SIREN_STATE_AWAKE = 1;
	public static final int SIREN_STATE_SLEEP = 2;

	private static final int VAD_ATART = 100;
	private static final int VAD_END = 102;
	private static final int WAKE_VAD_START = 104;
	private static final int WAKE_VAD_END = 106;
	private static final int WAKE_NOCMD = 107;
	private static final int WAKE_CMD = 108;

    class MainHandler extends Handler{

        public void handleMessage(Message msg) {
            switch(msg.what){
                case MSG_NATIVE_SERVICE_DIED:
                    nativeServiceDied();
                    break;
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

    private void nativeServiceDied(){
        mHandler.sendEmptyMessageDelayed(MSG_REINIT, 1000 * 5);
    }

	private final IRuntimeService.Stub proxy = new IRuntimeService.Stub(){

		@Override
		public void onNLP(String asr, String nlp, String action, int type){
            Log.e(TAG, "asr\t" + asr);
            Log.e(TAG, "nlp\t" + nlp);
            Log.e(TAG, "action " + action);
		}
	
		@Override
		public void onEvent(int event, double sl_degree, int has_sl){
			Log.e(TAG, event+" ,has_sl : " + has_sl + " ,sl_degree : " + (float)sl_degree);
			if(event == VAD_ATART || event == WAKE_VAD_START){

			}else if(event == VAD_END || event == WAKE_VAD_END){
				//mRuntimeNative.setSirenState(SIREN_STATE_SLEEP);
			}
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
	public IBinder onBind(Intent intent) {
		return null;
	}
}
