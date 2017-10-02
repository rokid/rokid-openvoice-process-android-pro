package com.rokid.openvoice;

import android.content.Intent;

public class VoiceReceiver extends android.content.BroadcastReceiver {

    @Override
    public void onReceive(android.content.Context context, Intent intent) {

        String action = intent.getAction();
        android.util.Log.e("VoiceReceiver", "onReceive : " + action);

        if(Intent.ACTION_BOOT_COMPLETED.equals(action)) {
            context.startServiceAsUser(new Intent(context, VoiceService.class), android.os.UserHandle.OWNER);
        } else if(android.net.ConnectivityManager.CONNECTIVITY_ACTION.equals(action)) {

            android.net.NetworkInfo mNetworkInfo = intent.getParcelableExtra(android.net.ConnectivityManager.EXTRA_NETWORK_INFO);
            if(mNetworkInfo != null) {
                android.util.Log.e("VoiceReceiver", mNetworkInfo.toString());
                VoiceManager.asInstance().networkStateChange(mNetworkInfo.isConnected());
            }
        }
    }
}
