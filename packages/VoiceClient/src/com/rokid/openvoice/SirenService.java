package com.rokid.openvoice;
/**
 * am startservice -n com.rokid.openvoice/com.rokid.openvoice.SirenService --ei action 1
 */
public class SirenService extends android.app.Service{

    @Override
    public int onStartCommand(android.content.Intent intent, int flags, int startId) {
        int action = intent.getIntExtra("action", -1);
        android.util.Log.e("VoiceService", "onStartCommand  action : " + action);
        VoiceManager mVoiceManager = VoiceManager.asInstance();
        switch(action){
            case VoiceManager.SIREN_STATE_AWAKE:
                mVoiceManager.setSirenState(VoiceManager.SIREN_STATE_AWAKE);
                break;
            case VoiceManager.SIREN_STATE_SLEEP:
			    mVoiceManager.setSirenState(VoiceManager.SIREN_STATE_SLEEP);
                break;
            case VoiceManager.SIREN_STATE_START:
			    mVoiceManager.startSiren(true);
                break;
            case VoiceManager.SIREN_STATE_STOP:
			    mVoiceManager.startSiren(false);
                break;
        }
        return super.onStartCommand(intent, flags, startId);
    }

	@Override
	public android.os.IBinder onBind(android.content.Intent intent){
		return null;
	}
}
