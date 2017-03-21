package com.rokid.server;

import android.content.Context;
import android.content.Intent;
import org.json.JSONObject;
import android.util.Log;
import android.os.Parcel;
import android.os.RemoteException;


public class RuntimeService extends rokid.os.IRuntimeService.Stub{

	String TAG = getClass().getSimpleName();

	Context mContext;

	public RuntimeService(Context mContext){
		Log.e(TAG, "RuntimeService  created");
		this.mContext = mContext;
	}

	@Override
	public void setSirenState(int state){
		Log.e(TAG, "set siren state   >>>   " + state);
		android.os.IBinder binder = android.os.ServiceManager.getService("runtime_native");
		if(binder == null){
			Log.e(TAG, "Permission denied in (RuntimeService setSirenState)");
			return;
		}
		Parcel data = Parcel.obtain();
		Parcel reply = Parcel.obtain();
		try{
			data.writeInterfaceToken(binder.getInterfaceDescriptor());
			binder.transact(android.os.IBinder.FIRST_CALL_TRANSACTION + 0, data, reply, 0);
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
		Log.e(TAG, "get siren state");
		android.os.IBinder binder = android.os.ServiceManager.getService("runtime_native");
		if(binder == null){
			Log.e(TAG, "Permission denied in (RuntimeService getSirenState)");
			return -1;
		}
		Parcel data = Parcel.obtain();
		Parcel reply = Parcel.obtain();
		try{
			data.writeInterfaceToken("com.rokid.server.RuntimeService");
			binder.transact(android.os.IBinder.FIRST_CALL_TRANSACTION + 1, data, reply, 0);
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
			//startService(null);
		}catch(Exception e){
			e.printStackTrace();
			return ;
		}
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
