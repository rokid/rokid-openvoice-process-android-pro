package com.android.commands.runtime;

import android.os.Parcel;

public class Runtime{

	public static void main(String[] args){
		String _s = null;
		for(String s : args){
			_s += s;
		}
		System.out.println(_s);
		String common = args[0];
		android.os.IBinder binder = android.os.ServiceManager.getService("runtime_service");
		Parcel data = Parcel.obtain();
		Parcel reply = Parcel.obtain();
		if(common.equals("state")){
			try{
				data.writeInterfaceToken(null);
				data.writeInt(Integer.parseInt(args[1]));
				binder.transact(0, data, reply, 0);
				reply.readException();
			}catch(android.os.RemoteException e){
				e.printStackTrace();
			}finally{
				data.recycle();
				reply.recycle();
			}
		}else if(common.equals("")){

		}
	}
}
