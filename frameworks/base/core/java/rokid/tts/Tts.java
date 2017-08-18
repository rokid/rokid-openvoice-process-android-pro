package rokid.tts;

public class Tts implements android.os.IBinder.DeathRecipient{

    private static ITts _tts;
    private Object mLock = new Object();

    public Tts(){
        _tts = ITts.Stub.asInterface(android.os.ServiceManager.getService("tts_process"));
        if(_tts != null){
            try{
                _tts.prepare();
                _tts.asBinder().linkToDeath(this, 0);
            }catch(android.os.RemoteException e){
                e.printStackTrace();
            }
        }
    }

//    /** @hide */
//    public boolean prepare(){
//        if(_tts != null){
//            try{
//                return _tts.prepare();
//            }catch(android.os.RemoteException e){
//                e.printStackTrace();
//            }
//        }
//        return false;
//    }

	@Override
	public void binderDied(){
        _tts = null;
		android.util.Log.e("Tts", "tts_process died !");
	}

    public int speak(String content, TtsCallback callback){

        if(_tts != null){
            synchronized(mLock){
                if(content != null && content.length() > 0){
                    try{
                        return _tts.speak(content, new TtsCallbackWrap(callback));
                    }catch(android.os.RemoteException e){
                        e.printStackTrace();
                    }
                }
            }
        }
        return -1;
    }

	// param id:  > 0  cancel tts request specified by 'id'
	//           <= 0  cancel all tts requests
    public void cancel(int id){

        if(_tts != null){
            synchronized(mLock){
                try{
                    _tts.cancel(id);
                }catch(android.os.RemoteException e){
                    e.printStackTrace();
                }
            }
        }
    }

	private class TtsCallbackWrap extends ITtsCallback.Stub {
		private TtsCallback callback;

		public TtsCallbackWrap(TtsCallback cb) {
			callback = cb;
		}

		@Override
		public void onStart(int id) {
			if (callback != null)
				callback.onStart(id);
		}

		@Override
		public void onCancel(int id) {
			if (callback != null)
				callback.onCancel(id);
		}

		@Override
		public void onComplete(int id) {
			if (callback != null)
				callback.onComplete(id);
		}

		@Override
		public void onError(int id, int err) {
			if (callback != null)
				callback.onError(id, err);
		}
	}
}
