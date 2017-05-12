package com.rokid.server.runtime;

import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ComponentName;
import android.content.ServiceConnection;
import android.content.BroadcastReceiver;
import android.net.NetworkInfo;
import android.net.wifi.WifiManager;
import android.net.ConnectivityManager;
import android.net.NetworkInfo.DetailedState;
import org.json.JSONObject;
import android.util.Log;
import android.os.Parcel;
import android.os.IBinder;
import android.os.RemoteException;
import android.os.Bundle;

public class RuntimeService extends rokid.os.IRuntimeService.Stub{

	String TAG = getClass().getSimpleName();

	Context mContext;
	LegacySiren mLegacySiren = null;
	
	public RuntimeService(Context mContext){
		Log.e(TAG, "RuntimeService  created " + mContext);
		this.mContext = mContext;
		mLegacySiren = new LegacySiren();
		mLegacySiren.initSiren();
		new NetworkBroadcastReceiver().registReceiver();
		ConnectivityManager cm = (ConnectivityManager)mContext.getSystemService(Context.CONNECTIVITY_SERVICE);
		NetworkInfo mNetworkInfo = cm.getNetworkInfo(ConnectivityManager.TYPE_WIFI);
		if(mNetworkInfo.isConnected()){
			networkStateChange(true);
		}
	}

	@Override
	public void nativeNlpMessage(String asr, String nlp, String action, int type){
		Log.e(TAG, "asr\t" + asr);
		Log.e(TAG, "nlp\t" + nlp);
		Log.e(TAG, "action\t" + action);
	}

	@Override
	public void setSirenState(int state){
		mLegacySiren.setSirenState(state);
	}

	@Override
	public void sirenEvent(int event, double sl_degree, double has_sl){
		mLegacySiren.sirenEvent(event, sl_degree, has_sl);
	}

	private void networkStateChange(boolean connected){
		Parcel data	= Parcel.obtain();
		Parcel reply = Parcel.obtain();
		IBinder runtime = android.os.ServiceManager.getService("runtime_native");
		try{
			data.writeInterfaceToken(runtime.getInterfaceDescriptor());
			data.writeInt(connected ? 1 : 0);
			runtime.transact(android.os.IBinder.FIRST_CALL_TRANSACTION + 3, data, reply, 0);
			reply.readException();
		}catch(RemoteException e){
			e.printStackTrace();
		}finally{
			data.recycle();
			reply.recycle();
		}
	}

	class NetworkBroadcastReceiver extends BroadcastReceiver{

		public void registReceiver(){
			IntentFilter intent = new IntentFilter();
			intent.addAction(WifiManager.NETWORK_STATE_CHANGED_ACTION);
			mContext.registerReceiver(this, intent);
		}

		@Override
		public void onReceive(Context ctx, Intent intent) {
			Log.e(TAG, intent.getAction());
			if(android.net.wifi.WifiManager.NETWORK_STATE_CHANGED_ACTION.equals(intent.getAction())){
				NetworkInfo info = intent.getParcelableExtra(WifiManager.EXTRA_NETWORK_INFO);
				DetailedState state = info.getDetailedState();
				if(state == DetailedState.CONNECTED){
					networkStateChange(true);
				}else if(state == DetailedState.DISCONNECTED){
					networkStateChange(false);
				}
			}
		}
	}
}
