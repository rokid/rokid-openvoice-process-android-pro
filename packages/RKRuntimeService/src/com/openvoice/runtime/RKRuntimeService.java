package com.openvoice.runtime;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;

public class RKRuntimeService extends Service {

	@Override
	public void onCreate() {
		android.util.Log.e("DXX", "onCreate");
	}

	@Override
	public IBinder onBind(Intent intent) {
		return null;
	}
}
