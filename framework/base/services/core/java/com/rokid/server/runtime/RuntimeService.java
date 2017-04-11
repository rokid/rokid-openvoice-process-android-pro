package com.rokid.server.runtime;

import android.content.Context;
import android.content.Intent;
import android.content.ComponentName;
import android.content.ServiceConnection;
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
	IBinder _thiz = null;
	
	Map<String, Pair<String, String>> domains = new HashMap<String, Pair<String, String>>();

	public RuntimeService(Context mContext){
		Log.e(TAG, "RuntimeService  created " + mContext);
		this.mContext = mContext;

		domains.put("com.rokid.system.cloudapp.client.scene", new Pair("activity", "com.rokid.system.cloudapp.client.scene"));
		domains.put("com.rokid.system.cloudapp.client.cut", new Pair("activity", "com.rokid.system.cloudapp.client.cut"));
		domains.put("com.rokid.system.cloudapp.engine", new Pair("service", "com.rokid.system.cloudapp.engine"));

		bindService();
	}

	ServiceConnection connect = new ServiceConnection(){	
		@Override
		public void onServiceConnected(ComponentName name, IBinder service){
			_service = service;
			Parcel data = Parcel.obtain();
			Parcel reply = Parcel.obtain();
			try{
				data.writeInterfaceToken("com.rokid.system.cloudapp.engine.service.RKCloudAppEngineService");
				Log.e(TAG, "service conneted   _binder : " + android.os.ServiceManager.getService("runtime_java") + "    " + RuntimeService.this);
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
	public void setSirenState(int state){
		Log.e(TAG, "set siren state   >>>   " + state + "    " + _thiz);
		if(_thiz == null){
			Log.e(TAG, "Permission denied in (RuntimeService setSirenState)");
			return;
		}
		getNativeService();
		Parcel data = Parcel.obtain();
		Parcel reply = Parcel.obtain();
		try{
			data.writeInterfaceToken(_thiz.getInterfaceDescriptor());
			data.writeInt(state);
			_thiz.transact(android.os.IBinder.FIRST_CALL_TRANSACTION + 0, data, reply, 0);
			reply.readException();
		}catch(RemoteException e){
			e.printStackTrace();
		}finally{
			data.recycle();
			reply.recycle();
		}
	}

	@Override
	public int getSirenState(){
		_thiz = android.os.ServiceManager.getService("runtime_native");
		Log.e(TAG, "get siren state  " + _thiz);
		if(_thiz == null){
			Log.e(TAG, "Permission denied in (RuntimeService getSirenState)");
			return -1;
		}
		getNativeService();
		Parcel data = Parcel.obtain();
		Parcel reply = Parcel.obtain();
		try{
			data.writeInterfaceToken("com.rokid.native.RuntimeService");
			_thiz.transact(android.os.IBinder.FIRST_CALL_TRANSACTION + 1, data, reply, 0);
			reply.readException();
			return reply.readInt();
		}catch(RemoteException e){
			e.printStackTrace();
		}finally{
			data.recycle();
			reply.recycle();
		}
		return -1;
	}

	@Override
	public void bindService(){
		Log.e(TAG, "mContext   " + mContext);
		if(mContext != null){
			ComponentName cn = new ComponentName("com.rokid.system.cloudappclient", 
					"com.rokid.system.cloudapp.engine.service.RKCloudAppEngineService");
			Intent intent = new Intent().setComponent(cn);
			mContext.bindServiceAsUser(intent, connect, Context.BIND_AUTO_CREATE, android.os.UserHandle.OWNER);
		}

		Parcel data = Parcel.obtain();
		Parcel reply = Parcel.obtain();
		try{
			data.writeInterfaceToken("com.rokid.native.RuntimeService");
			data.writeStrongBinder(this);
			//data.writeStrongBinder(android.os.ServiceManager.getService("runtime_java"));
			getNativeService();
			Log.e(TAG, "service conneted   _thiz : " + _thiz);
			_thiz.transact(android.os.IBinder.FIRST_CALL_TRANSACTION + 2, data, reply, 0);
			reply.readException();
		}catch(RemoteException e){
			e.printStackTrace();
		}finally{
			data.recycle();
			reply.recycle();
		}
	}
	
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
			getNativeService();
			Parcel data = Parcel.obtain();
			Parcel reply = Parcel.obtain();
			try{
				data.writeInterfaceToken("com.rokid.system.cloudapp.engine.service.RKCloudAppEngineService");
				Log.e(TAG, "_thiz    >>>   " + _thiz);
				data.writeString(msg);
				_service.transact(android.os.IBinder.FIRST_CALL_TRANSACTION + 665, data, reply, 0);
				reply.readException();
			}catch(RemoteException e){
				e.printStackTrace();
			}finally{
				data.recycle();
				reply.recycle();
			}
		}else{
			Log.e(TAG, "nativeNlpMessage    " + msg);
		}
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

	private IBinder getNativeService(){
		if(_thiz != null && _thiz.isBinderAlive()) {
			return _thiz;
		}	
		_thiz = android.os.ServiceManager.getService("runtime_native");
		return _thiz;
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
}
