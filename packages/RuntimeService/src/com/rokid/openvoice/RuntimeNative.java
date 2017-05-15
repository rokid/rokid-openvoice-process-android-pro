package com.rokid.openvoice;

import android.os.Parcel;
import android.os.IBinder;
import android.os.Bundle;
import android.os.RemoteException;

public class RuntimeNative{

	private static final String DESCRIPTOR = "com.rokid.native.RuntimeService";

	private IBinder runtime = null;
	private static RuntimeNative mRuntimeNative = null;

	private RuntimeNative(){
		runtime = android.os.ServiceManager.getService("runtime_native");
	}

	public static RuntimeNative asInstance(){
		if(mRuntimeNative == null){
			synchronized(new Object()){
				if(mRuntimeNative == null){
					mRuntimeNative = new RuntimeNative();
				}
			}
		}
		return mRuntimeNative;
	}

	public boolean init(){
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

	public void networkStateChange(boolean connected){
		Parcel data	= Parcel.obtain();
		Parcel reply = Parcel.obtain();
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

	public void updateStack(String currAppid, String prevAppid){
		Parcel data	= Parcel.obtain();
		Parcel reply = Parcel.obtain();
		try{
			data.writeInterfaceToken(runtime.getInterfaceDescriptor());
			data.writeString(currAppid);
			data.writeString(prevAppid);
			runtime.transact(android.os.IBinder.FIRST_CALL_TRANSACTION + 4, data, reply, 0);
			reply.readException();
		}catch(RemoteException e){
			e.printStackTrace();
		}finally{
			data.recycle();
			reply.recycle();
		}
	}

	public void addBinder(IBinder binder){
		Parcel data = Parcel.obtain();
		Parcel reply = Parcel.obtain();
		try{
			data.writeInterfaceToken(DESCRIPTOR);
			data.writeStrongBinder(binder);
			runtime.transact(IBinder.FIRST_CALL_TRANSACTION + 5, data, reply, 0);
			reply.readException();
		}catch(RemoteException e){
			e.printStackTrace();
		}finally{
			data.recycle();
			reply.recycle();
		}
	}
}
