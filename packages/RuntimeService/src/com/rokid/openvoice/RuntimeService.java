package com.rokid.openvoice;

import android.app.Service;
import android.content.Intent;
import android.content.Context;
import android.os.IBinder;
import android.util.Log;
import android.net.NetworkInfo;
import android.net.ConnectivityManager;

public class RuntimeService extends Service{

	String TAG = getClass().getSimpleName();
	RuntimeNative mRuntimeNative = null;

	public static boolean initialized = false;

	public static final int SIREN_STATE_AWAKE = 1;
	public static final int SIREN_STATE_SLEEP = 2;

	public static final int VAD_START = 100;
	public static final int VAD_END = 102;
	public static final int VAD_CANCEL = 103;
	public static final int WAKE_VAD_START = 104;
	public static final int WAKE_VAD_END = 106;
	public static final int WAKE_NOCMD = 108;
	public static final int WAKE_CMD = 109;
	public static final int WAKE_CANCEL = 110;
	public static final int SLEEP = 111;
	
	public RuntimeService(){
		Log.e(TAG, "RuntimeService  created ");
		mRuntimeNative = RuntimeNative.asInstance();
		mRuntimeNative.init();
		mRuntimeNative.addBinder(new RuntimeProxy());
		initialized = true;
	}

	@Override
	public void onCreate(){
		mUEventObserver.startObserving("/sound/card1/pcmC1D0c");

		ConnectivityManager cm = (ConnectivityManager)getSystemService(Context.CONNECTIVITY_SERVICE);
		NetworkInfo mNetworkInfo = cm.getActiveNetworkInfo();
		if(mNetworkInfo != null){
			mRuntimeNative.networkStateChange(true);
		}
	}

	class RuntimeProxy extends IRuntimeService.Stub{

		@Override
		public void onResponse(String asr, String nlp, String action, int type){
			Log.e(TAG, "asr\t" + asr);
			Log.e(TAG, "nlp\t" + nlp);
			Log.e(TAG, "action\t" + action);
		}
	
		@Override
		public void onEvent(int event, double sl_degree, int has_sl){
			Log.e(TAG, event+" ,has_sl : " + has_sl + " ,sl_degree : " + (float)sl_degree);
		}
	}	

	private final android.os.UEventObserver mUEventObserver = new android.os.UEventObserver() {
		 
		@Override
		public void onUEvent(android.os.UEventObserver.UEvent event){
			Log.e(TAG, event.toString());
			String action = event.get("ACTION");
			if("add".equals(action)){
				mRuntimeNative.startSiren(true);
			}else if("remove".equals(action)){
				mRuntimeNative.startSiren(false);
			}
		}
	};

	@Override
	public IBinder onBind(Intent intent) {
		return null;
	}
}
