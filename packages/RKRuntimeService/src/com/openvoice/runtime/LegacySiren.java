package com.openvoice.runtime;

import android.os.Parcel;
import android.os.IBinder;
import android.os.Bundle;
import android.os.RemoteException;

public class LegacySiren{

	private static final String DESCRIPTOR = "com.rokid.native.RuntimeService";

	private IBinder runtime;

	public LegacySiren(){
		runtime = android.os.ServiceManager.getService("runtime_native");
	}

	public boolean initSiren(){
		Parcel data = Parcel.obtain();
		Parcel reply = Parcel.obtain();
		try{
			data.writeInterfaceToken(DESCRIPTOR);
			runtime.transact(IBinder.FIRST_CALL_TRANSACTION, data, reply, 0);
			reply.readException();
			int err = reply.readInt();
			return (err > 0 ? true : false);
		}catch(RemoteException e){
			e.printStackTrace();
		}finally{
			data.recycle();
			reply.recycle();
		}
		return false;
	}

	public void startSiren(boolean flag){
		Parcel data = Parcel.obtain();
		Parcel reply = Parcel.obtain();
		try{
			data.writeInterfaceToken(DESCRIPTOR);
			data.writeInt((flag ? 1 : 0));
			runtime.transact(IBinder.FIRST_CALL_TRANSACTION + 1, data, reply, 0);
			reply.readException();
		}catch(RemoteException e){
			e.printStackTrace();
		}finally{
			data.recycle();
			reply.recycle();
		}
	}

	public void setSirenState(int state){
		Parcel data = Parcel.obtain();
		Parcel reply = Parcel.obtain();
		try{
			data.writeInterfaceToken(DESCRIPTOR);
			data.writeInt(state);
			runtime.transact(IBinder.FIRST_CALL_TRANSACTION + 2, data, reply, 0);
			reply.readException();
		}catch(RemoteException e){
			e.printStackTrace();
		}finally{
			data.recycle();
			reply.recycle();
		}
	}

	public void sirenEvent(int event, double sl_degree, int has_sl){
		android.util.Log.e("DX", event+" ,has_sl : " + has_sl + " ,sl_degree : " + (float)sl_degree);
	}
}
