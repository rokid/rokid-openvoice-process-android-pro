package com.openvoice.runtime;

import android.content.Intent;

public class RKRuntimeReceiver extends android.content.BroadcastReceiver{

	@Override
	public void onReceive(android.content.Context context, Intent intent){
		android.util.Log.e("DXX", "onReceive");
		context.startServiceAsUser(new Intent(context, RKRuntimeService.class), android.os.UserHandle.OWNER);
	}
}
