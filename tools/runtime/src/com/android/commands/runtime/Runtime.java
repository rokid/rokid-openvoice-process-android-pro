package com.android.commands.runtime;

import android.os.Parcel;
import android.os.RemoteException;
import rokid.os.IRuntimeService;

public class Runtime{

	public static void main(String[] args){
		String _s = null;
		for(String s : args){
			_s += s;
		}
		String common = args[0];
		android.os.IBinder binder = android.os.ServiceManager.getService("runtime_java");
		System.out.println("binder    >>>      " + binder + "    " + args[0] + "     ");
		Parcel data = Parcel.obtain();
		Parcel reply = Parcel.obtain();
		if(common.equals("setState")){
			try{
				data.writeInterfaceToken("rokid.os.IRuntimeService");
				data.writeInt(Integer.parseInt(args[1]));
				binder.transact(android.os.IBinder.FIRST_CALL_TRANSACTION + 1, data, reply, 0);
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
