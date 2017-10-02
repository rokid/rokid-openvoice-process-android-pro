package com.rokid.openvoice;

import android.os.Parcel;
import android.os.IBinder;
import android.os.RemoteException;
import java.util.List;

public class VoiceManager implements IBinder.DeathRecipient {

    public static final int SIREN_STATE_AWAKE           = 1;
    public static final int SIREN_STATE_SLEEP           = 2;
    public static final int SIREN_STATE_START           = 3;
    public static final int SIREN_STATE_STOP            = 4;

    private static final String DESCRIPTOR = "com.rokid.openvoice.openvoice_process";
    private static VoiceManager mVoiceManager = null;
    private IBinder remote = null;

    private VoiceManager() {
        remote = android.os.ServiceManager.getService("openvoice_process");
        if(remote != null) {
            try {
                remote.linkToDeath(this, 0);
            } catch(RemoteException e) {
                e.printStackTrace();
            }
        } else android.util.Log.e("VoiceManager", "native service is null");
    }

    public static VoiceManager asInstance() {
        if(mVoiceManager == null) {
            synchronized(new Object()) {
                if(mVoiceManager == null) {
                    mVoiceManager = new VoiceManager();
                }
            }
        }
        return mVoiceManager;
    }

    @Override
    public void binderDied() {
        remote = null;
        mVoiceManager = null;
        VoiceService.mHandler.sendEmptyMessageDelayed(VoiceService.MSG_REINIT, 1000 * 3);
        android.util.Log.e("VoiceManager", "native service died !");
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

    public void updateConfig(String device_id, String device_type_id, String key, String secret) {
        if(remote != null) {
            Parcel data	= Parcel.obtain();
            Parcel reply = Parcel.obtain();
            try {
                data.writeInterfaceToken(DESCRIPTOR);
                data.writeString(device_id);
                data.writeString(device_type_id);
                data.writeString(key);
                data.writeString(secret);
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

    public void registCallback(IBinder callback) {
        if(remote != null) {
            Parcel data = Parcel.obtain();
            Parcel reply = Parcel.obtain();
            try {
                data.writeInterfaceToken(DESCRIPTOR);
                data.writeStrongBinder(callback);
                remote.transact(IBinder.FIRST_CALL_TRANSACTION + 6, data, reply, 0);
                reply.readException();
            } catch(RemoteException e) {
                e.printStackTrace();
            }finally{
                data.recycle();
                reply.recycle();
            }
        }
    }

//	public int insertVTWord(VTConfig.VTWord vt_word) {
//		if (remote != null) {
//			Parcel data = Parcel.obtain();
//			Parcel reply = Parcel.obtain();
//			try {
//				data.writeInterfaceToken(DESCRIPTOR);
//				data.writeByteArray(vt_word.toByteArray());
//				remote.transact(IBinder.FIRST_CALL_TRANSACTION + 7, data, reply, 0);
//				reply.readException();
//				return reply.readInt();
//			} catch (RemoteException e) {
//				e.printStackTrace();
//			} finally {
//				data.recycle();
//				reply.recycle();
//			}
//		}
//		return -1;
//	}
//
//	public int deleteVTWord(String word) {
//		if (remote != null) {
//			Parcel data = Parcel.obtain();
//			Parcel reply = Parcel.obtain();
//			try {
//				data.writeInterfaceToken(DESCRIPTOR);
//				data.writeString(word);
//				remote.transact(IBinder.FIRST_CALL_TRANSACTION + 8, data, reply, 0);
//				reply.readException();
//				return reply.readInt();
//			} catch (RemoteException e) {
//				e.printStackTrace();
//			} finally {
//				data.recycle();
//				reply.recycle();
//			}
//		}
//		return -1;
//	}
//
//	public int queryVTWord(List<VTConfig.VTWord> vt_words) {
//		if (remote != null) {
//			Parcel data = Parcel.obtain();
//			Parcel reply = Parcel.obtain();
//			try {
//				data.writeInterfaceToken(DESCRIPTOR);
//				remote.transact(IBinder.FIRST_CALL_TRANSACTION + 9, data, reply, 0);
//				reply.readException();
//				int temp = 0;
//				int count = reply.readInt();
//				if (count > 0) {
//					for (int i = 0; i < count; i++) {
//						int len = reply.readInt();
//						byte[] vt_word = new byte[len];
//						reply.readByteArray(vt_word);
//						if (vt_word != null && vt_word.length > 0) {
//							vt_words.add(VTConfig.VTWord.parseFrom(vt_word));
//							temp++;
//						}
//					}
//				}
//				return temp;
//			} catch (Exception e) {
//				e.printStackTrace();
//			} finally {
//				data.recycle();
//				reply.recycle();
//			}
//		}
//		return -1;
//	}
}
