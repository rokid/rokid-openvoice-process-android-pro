package com.rokid.server;

import android.content.Context;
import android.content.Intent;
import android.content.ComponentName;
import android.content.ServiceConnection;
import org.json.JSONObject;
import android.util.Log;
import android.os.Parcel;
import android.os.IBinder;
import android.os.RemoteException;


public class RuntimeService extends rokid.os.IRuntimeService.Stub{

	String TAG = getClass().getSimpleName();

	Context mContext;
	IBinder _service = null;
	IBinder _thiz = null;

	public RuntimeService(Context mContext){
		Log.e(TAG, "RuntimeService  created");
		this.mContext = mContext;
		_thiz = android.os.ServiceManager.getService("runtime_navive");
		if(mContext != null){
			ComponentName cn = new ComponentName("com.rokid.system.cloudappclient", 
					"com.rokid.system.cloudapp.engine.service.RKCloudAppEngineService");
			Intent intent = new Intent().setComponent(cn);
			mContext.bindServiceAsUser(intent, connect, Context.BIND_AUTO_CREATE, android.os.UserHandle.OWNER);
		}
	}

	ServiceConnection connect = new ServiceConnection(){	
		public void onServiceConnected(ComponentName name, IBinder service){
			_service = service;
			Parcel data = Parcel.obtain();
			Parcel reply = Parcel.obtain();
			try{
				data.writeInterfaceToken("com.rokid.system.cloudapp.engine.service.RKCloudAppEngineService");
				Log.e(TAG, "_thiz    >>>   " + _thiz);
				data.writeStrongBinder(_thiz);
				_service.transact(999, data, reply, 0);
				reply.readException();
			}catch(RemoteException e){
				e.printStackTrace();
			}finally{
				data.recycle();
				reply.recycle();
			}
		}
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
		Log.e(TAG, "get siren state  " + _thiz);
		if(_thiz == null){
			Log.e(TAG, "Permission denied in (RuntimeService getSirenState)");
			return -1;
		}
		Parcel data = Parcel.obtain();
		Parcel reply = Parcel.obtain();
		try{
			data.writeInterfaceToken("com.rokid.server.RuntimeService");
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
	public void nlpMessage(String nlp){
		android.util.Log.e(TAG, nlp);
		JSONObject json = null;
		try{
			json = new JSONObject(nlp);
			String domain = json.getString("domain");
			//TODO

		}catch(Exception e){
			e.printStackTrace();
			return ;
		}
		Parcel data = Parcel.obtain();
		Parcel reply = Parcel.obtain();
		try{
			data.writeInterfaceToken("com.rokid.system.cloudapp.engine.service.RKCloudAppEngineService");
			Log.e(TAG, "_thiz    >>>   " + _thiz);
			//TODO
			data.writeString("");
			_service.transact(666, data, reply, 0);
			reply.readException();
		}catch(RemoteException e){
			e.printStackTrace();
		}finally{
			data.recycle();
			reply.recycle();
		}
	}

	@Override
	public void receiveRemoteMessage(String intent){
		//startService();
	}

	private void startService(String action){
		if(mContext != null){
			Intent intent = new Intent(action);
			mContext.startServiceAsUser(intent, android.os.UserHandle.OWNER);
		}else{
			Log.e(TAG, "context is null ");
		}
	}

	private void startActivity(String action){
		if(mContext != null){
			Intent intent = new Intent(action);
			mContext.startActivityAsUser(intent, android.os.UserHandle.OWNER);
		}else{
			Log.e(TAG, "context is null ");
		}
	}
}
