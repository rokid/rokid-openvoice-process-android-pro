package com.rokid.openvoice;

import android.content.Intent;
import android.net.NetworkInfo;
import android.net.NetworkInfo.DetailedState;

public class RuntimeReceiver extends android.content.BroadcastReceiver{

	@Override
	public void onReceive(android.content.Context context, Intent intent){
		String action = intent.getAction();
		android.util.Log.e("RuntimeReceiver", "onReceive : " + action);
		if(Intent.ACTION_BOOT_COMPLETED.equals(action)){
			context.startServiceAsUser(new Intent(context, RuntimeService.class), android.os.UserHandle.OWNER);
		}else if(android.net.wifi.WifiManager.NETWORK_STATE_CHANGED_ACTION.equals(action)){
			NetworkInfo info = intent.getParcelableExtra(android.net.wifi.WifiManager.EXTRA_NETWORK_INFO);
			DetailedState state = info.getDetailedState();
			if(state == DetailedState.CONNECTED){
				RuntimeNative.asInstance().networkStateChange(true);
			}else if(state == DetailedState.DISCONNECTED){
				RuntimeNative.asInstance().networkStateChange(false);
			}
		}
	}
}
