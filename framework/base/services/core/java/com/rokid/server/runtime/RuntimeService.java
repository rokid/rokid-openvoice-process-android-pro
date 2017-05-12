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

import android.util.Pair;
import java.util.Map;
import java.util.HashMap;

public class RuntimeService extends rokid.os.IRuntimeService.Stub{

	String TAG = getClass().getSimpleName();

	Context mContext;
	IBinder _service = null;
	LegacySiren mLegacySiren = null;
	
	Map<String, Pair<String, String>> domains = new HashMap<String, Pair<String, String>>();

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

		domains.put("com.rokid.system.cloudapp.client.scene", new Pair("activity", "com.rokid.system.cloudapp.client.scene"));
		domains.put("com.rokid.system.cloudapp.client.cut", new Pair("activity", "com.rokid.system.cloudapp.client.cut"));
		domains.put("com.rokid.system.cloudapp.engine", new Pair("service", "com.rokid.system.cloudapp.engine"));

		ComponentName cn = new ComponentName("com.rokid.system.cloudappclient", 
				"com.rokid.system.cloudapp.engine.service.RKCloudAppEngineService");
		Intent intent = new Intent().setComponent(cn);
		mContext.bindServiceAsUser(intent, connect, Context.BIND_AUTO_CREATE, android.os.UserHandle.OWNER);
	}

	ServiceConnection connect = new ServiceConnection(){	
		@Override
		public void onServiceConnected(ComponentName name, IBinder service){
			_service = service;
			Parcel data = Parcel.obtain();
			Parcel reply = Parcel.obtain();
			try{
				data.writeInterfaceToken("com.rokid.system.cloudapp.engine.service.RKCloudAppEngineService");
				Log.e(TAG, "service conneted");
				data.writeStrongBinder(RuntimeService.this);
				_service.transact(android.os.IBinder.FIRST_CALL_TRANSACTION + 998, data, reply, 0);
				reply.readException();
			}catch(RemoteException e){
				e.printStackTrace();
			}finally{
				data.recycle();
				reply.recycle();
			}
		}
		@Override
		public void onServiceDisconnected(ComponentName name){
			Log.e(TAG, "service disconnected");
			_service = null;
		}
	};	

	@Override
	public void nativeNlpMessage(String msg){
		if(msg == null) return;
		JSONObject json = null;
		boolean cloud = false;
		try{
			json = new JSONObject(msg);
			String nlp = json.getString("nlp");
			Log.e(TAG, nlp);
			json = new JSONObject(nlp);
			if(json != null && json.has("cloud")){
				cloud = json.getBoolean("cloud");
			}
		}catch(Exception e){
			e.printStackTrace();
		}
		if(cloud){
			Parcel data = Parcel.obtain();
			Parcel reply = Parcel.obtain();
			try{
				data.writeInterfaceToken("com.rokid.system.cloudapp.engine.service.RKCloudAppEngineService");
				data.writeString(msg);
				_service.transact(android.os.IBinder.FIRST_CALL_TRANSACTION + 665, data, reply, 0);
				reply.readException();
			}catch(RemoteException e){
				e.printStackTrace();
			}finally{
				data.recycle();
				reply.recycle();
			}
		}
	}

	@Override
	public void setSirenState(int state){
		mLegacySiren.setSirenState(state);
	}

	@Override
	public void sirenEvent(int event, double sl_degree, double has_sl){
		mLegacySiren.sirenEvent(event, sl_degree, has_sl);
	}

	@Override
	public void receiveNlpMessage(Bundle bundle){
		String nlp = bundle.getString("nlp");
		Log.e(TAG, nlp);
		if(nlp == null || nlp.length() == 0){
			return;
		}
		JSONObject json = null;
		try{
			json = new JSONObject(nlp);
			String domain = json.getString("domain");
			if(domain != null){
				Pair pair = domains.get(domain);
				if(pair != null){
					if("activity".equals(pair.first)){
						startActivity((String)pair.second, nlp);
					}else if("service".equals(pair.first)){
						startService((String)pair.second, nlp);
					}
				}else Log.e(TAG, "Connot find domain  :  " + domain);
			}else Log.e(TAG, "domain is null");
		}catch(Exception e){
			e.printStackTrace();
		}
	}

	private void startService(String action, String nlp){
		if(mContext != null){
			Intent intent = new Intent(action);
			intent.putExtra("nlp", nlp);
			mContext.startServiceAsUser(intent, android.os.UserHandle.OWNER);
		}else{
			Log.e(TAG, "context is null ");
		}
	}

	private void startActivity(String action, String nlp){
		if(mContext != null){
			Intent intent = new Intent(action);
			intent.putExtra("nlp", nlp);
			intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
			mContext.startActivityAsUser(intent, android.os.UserHandle.OWNER);
		}else{
			Log.e(TAG, "context is null ");
		}
	}

	private void networkStateChange(boolean connected){
		Parcel data	= Parcel.obtain();
		Parcel reply = Parcel.obtain();
		try{
			data.writeInterfaceToken(DESCRIPTOR);
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
