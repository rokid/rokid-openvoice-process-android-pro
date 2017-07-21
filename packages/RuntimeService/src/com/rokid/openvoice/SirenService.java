package com.rokid.openvoice;
/**
 * am startservice -n com.rokid.openvoice/com.rokid.openvoice.SirenService --ei action 1
 */
public class SirenService extends android.app.Service{

    private static final int SIREN_AWAKE = 1;
    private static final int SIREN_SLEEP = 2;
    private static final int START_SIREN = 3;
    private static final int STOP_SIREN = 4;

    @Override
    public int onStartCommand(android.content.Intent intent, int flags, int startId) {
        int action = intent.getIntExtra("action", -1); 
		RuntimeNative mRuntimeNative = RuntimeNative.asInstance();
        android.util.Log.e("RuntimeService", "onStartCommand  action : " + action);
        if(RuntimeService.initialized){
            switch(action){
                case SIREN_AWAKE:
                    mRuntimeNative.setSirenState(RuntimeService.SIREN_STATE_AWAKE);
                   break;
                case SIREN_SLEEP:
                    mRuntimeNative.setSirenState(RuntimeService.SIREN_STATE_SLEEP);
                   break;
                case START_SIREN:
                   mRuntimeNative.startSiren(true);
                   break;
                case STOP_SIREN:
                   mRuntimeNative.startSiren(false);
                   break;
            }
        }
        return super.onStartCommand(intent, flags, startId);
    }

	@Override
	public android.os.IBinder onBind(android.content.Intent intent){
		return null;
	}
}
