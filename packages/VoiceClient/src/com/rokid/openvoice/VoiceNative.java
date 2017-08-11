package com.rokid.openvoice;

import android.os.Parcel;
import android.os.IBinder;
import android.os.RemoteException;

public class VoiceNative implements IBinder.DeathRecipient {

    private static final String DESCRIPTOR = "com.rokid.openvoice.openvoice_process";
    private static VoiceNative mVoiceNative = null;

    private IBinder remote = null;

    private VoiceNative() {
        remote = android.os.ServiceManager.getService("openvoice_process");
        if(remote != null) {
            try {
                remote.linkToDeath(this, 0);
            } catch(RemoteException e) {
                e.printStackTrace();
            }
        } else android.util.Log.e("VoiceNative", "native service is null");
    }

    public static VoiceNative asInstance() {
        if(mVoiceNative == null) {
            synchronized(new Object()) {
                if(mVoiceNative == null) {
                    mVoiceNative = new VoiceNative();
                }
            }
        }
        return mVoiceNative;
    }

    @Override
    public void binderDied() {
        remote = null;
        mVoiceNative = null;
        VoiceService.initialized = false;
        VoiceService.mHandler.sendEmptyMessageDelayed(VoiceService.MSG_REINIT, 1000 * 3);
        android.util.Log.e("VoiceNative", "native service died !");
    }

    public boolean init() {
        if(remote != null) {
            Parcel data = Parcel.obtain();
            Parcel reply = Parcel.obtain();
            try {
                data.writeInterfaceToken(DESCRIPTOR);
                remote.transact(IBinder.FIRST_CALL_TRANSACTION, data, reply, 0);
                reply.readException();
                int err = reply.readInt();
                return (err > 0 ? true : false);
            } catch(RemoteException e) {
                e.printStackTrace();
            }finally{
                data.recycle();
                reply.recycle();
            }
        }
        return false;
    }

    public void startSiren(boolean flag) {
        if(remote != null) {
            Parcel data = Parcel.obtain();
            Parcel reply = Parcel.obtain();
            try {
                data.writeInterfaceToken(DESCRIPTOR);
                data.writeInt((flag ? 1 : 0));
                remote.transact(IBinder.FIRST_CALL_TRANSACTION + 1, data, reply, 0);
                reply.readException();
            } catch(RemoteException e) {
                e.printStackTrace();
            }finally{
                data.recycle();
                reply.recycle();
            }
        }
    }

    public void setSirenState(int state) {
        if(remote != null) {
            Parcel data = Parcel.obtain();
            Parcel reply = Parcel.obtain();
            try {
                data.writeInterfaceToken(DESCRIPTOR);
                data.writeInt(state);
                remote.transact(IBinder.FIRST_CALL_TRANSACTION + 2, data, reply, 0);
                reply.readException();
            } catch(RemoteException e) {
                e.printStackTrace();
            }finally{
                data.recycle();
                reply.recycle();
            }
        }
    }

    public void networkStateChange(boolean connected) {
        if(remote != null) {
            Parcel data	= Parcel.obtain();
            Parcel reply = Parcel.obtain();
            try {
                data.writeInterfaceToken(DESCRIPTOR);
                data.writeInt(connected ? 1 : 0);
                remote.transact(IBinder.FIRST_CALL_TRANSACTION + 3, data, reply, 0);
                reply.readException();
            } catch(RemoteException e) {
                e.printStackTrace();
            }finally{
                data.recycle();
                reply.recycle();
            }
        }
    }

    public void updateStack(String appid) {
        if(remote != null) {
            Parcel data	= Parcel.obtain();
            Parcel reply = Parcel.obtain();
            try {
                data.writeInterfaceToken(DESCRIPTOR);
                data.writeString(appid);
                remote.transact(IBinder.FIRST_CALL_TRANSACTION + 4, data, reply, 0);
                reply.readException();
            } catch(RemoteException e) {
                e.printStackTrace();
            }finally{
                data.recycle();
                reply.recycle();
            }
        }
    }

    public void registCallback(IBinder callback) {
        if(remote != null) {
            Parcel data = Parcel.obtain();
            Parcel reply = Parcel.obtain();
            try {
                data.writeInterfaceToken(DESCRIPTOR);
                data.writeStrongBinder(callback);
                remote.transact(IBinder.FIRST_CALL_TRANSACTION + 5, data, reply, 0);
                reply.readException();
            } catch(RemoteException e) {
                e.printStackTrace();
            }finally{
                data.recycle();
                reply.recycle();
            }
        }
    }
}
