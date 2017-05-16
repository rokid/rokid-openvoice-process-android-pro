package com.rokid.openvoice;

public class RuntimeReceiver extends android.content.BroadcastReceiver{

	@Override
	public void onReceive(android.content.Context context, android.content.Intent intent){
		String action = intent.getAction();
		android.util.Log.e("RuntimeReceiver", "onReceive : " + action);
		if(Intent.ACTION_BOOT_COMPLETED.equals(action)){
			context.startServiceAsUser(new Intent(context, RuntimeService.class), android.os.UserHandle.OWNER);
		}else if(android.net.ConnectivityManager.CONNECTIVITY_ACTION.equals(action)){
			android.net.NetworkInfo mNetworkInfo = intent.getParcelableExtra(android.net.ConnectivityManager.EXTRA_NETWORK_INFO);
			if(mNetworkInfo != null){
				android.util.Log.e("RuntimeReceiver", mNetworkInfo.toString());
				if(mNetworkInfo.isConneted()){
					RuntimeNative.asInstance().networkStateChange(true);
				}else{
					RuntimeNative.asInstance().networkStateChange(false);
				}
			}
		}
	}
}
