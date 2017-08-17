#ifndef ITTS_CALLBACK_H
#define ITTS_CALLBACK_H

#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <utils/String8.h>
#include <string>

using namespace android;
using namespace std;

enum{
	TRANSACTION_ONSTART = IBinder::FIRST_CALL_TRANSACTION + 0,
	TRANSACTION_ONCANCEL,
	TRANSACTION_ONCOMPLETE,
	TRANSACTION_ONERROR
};

class ITtsCallback : public IInterface{
public:
	DECLARE_META_INTERFACE(TtsCallback);
	virtual void onStart(int id)=0;

	virtual void onCancel(int id)=0;

	virtual void onComplete(int id)=0;

	virtual void onError(int id, int err)=0;
};

class BnTtsCallback : public BnInterface<ITtsCallback> {
	public:
		virtual status_t onTransact(uint32_t code, const Parcel &data, Parcel *reply, uint32_t flag = 0);
};

#endif // TTSCALLBACK_H
