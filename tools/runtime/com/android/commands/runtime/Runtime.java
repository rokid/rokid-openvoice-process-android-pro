package com.android.commands.runtime;

import android.os.Parcel;
import rokid.os.IRuntimeService;

public class Runtime{

	public static void main(String[] args){
		String _s = null;
		for(String s : args){
			_s += s;
		}
		System.out.println(_s);
		String common = args[0];
		IRuntimeService runtime_service = IRuntimeService.Stub.asInterface(android.os.ServiceManager.getService("runtime_service"));
		try{
			if(common.equals("setState")){
				runtime_service.setSirenState(Integer.parseInt(args[1]));
			}else if(common.equals("getState")){
				System.out.printf("current state   >>>   " + runtime_service.getSirenState());
			}
		}catch(android.os.RemoteException e){
			e.printStackTrace();
		}
	}
}
