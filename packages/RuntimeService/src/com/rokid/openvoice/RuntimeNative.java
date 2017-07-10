package com.rokid.openvoice;

import android.os.Parcel;
import android.os.IBinder;
import android.os.RemoteException;

public class RuntimeNative implements IBinder.DeathRecipient{

	private static final String DESCRIPTOR = "com.rokid.openvoice.openvoice_process";
    private static RuntimeNative mRuntimeNative = null;

	private IBinder runtime = null;

	private RuntimeNative(){
		runtime = android.os.ServiceManager.getService("openvoice_proc");
		if(runtime != null){
			try{
				runtime.linkToDeath(this, 0);
			}catch(RemoteException e){
				e.printStackTrace();
			}
		}else android.util.Log.e("RuntimeNative", "native service is null");
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

	@Override
	public void binderDied(){
        runtime = null;
        mRuntimeNative = null;
        RuntimeService.initialized = false;
        RuntimeService.mHandler.sendEmptyMessage(RuntimeService.MSG_NATIVE_SERVICE_DIED);
		android.util.Log.e("RuntimeNative", "native service died !");
	}

	public boolean init(){
        if(runtime != null){
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
        }
		return false;
	}

	public void startSiren(boolean flag){
        if(runtime != null){
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
	}

	public void setSirenState(int state){
        if(runtime != null){
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
	}

	public void networkStateChange(boolean connected){
        if(runtime != null){
    		Parcel data	= Parcel.obtain();
    		Parcel reply = Parcel.obtain();
    		try{
    			data.writeInterfaceToken(DESCRIPTOR);
    			data.writeInt(connected ? 1 : 0);
    			runtime.transact(IBinder.FIRST_CALL_TRANSACTION + 3, data, reply, 0);
    			reply.readException();
    		}catch(RemoteException e){
    			e.printStackTrace();
    		}finally{
    			data.recycle();
    			reply.recycle();
    		}
        }
	}

	public void updateStack(String appid){
        if(runtime != null){
    		Parcel data	= Parcel.obtain();
    		Parcel reply = Parcel.obtain();
    		try{
    			data.writeInterfaceToken(DESCRIPTOR);
    			data.writeString(appid);
    			runtime.transact(IBinder.FIRST_CALL_TRANSACTION + 4, data, reply, 0);
    			reply.readException();
    		}catch(RemoteException e){
    			e.printStackTrace();
    		}finally{
    			data.recycle();
    			reply.recycle();
    		}
        }
	}

	public void addBinder(IBinder binder){
        if(runtime != null){
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
}
