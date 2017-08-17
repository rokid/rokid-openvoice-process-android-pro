#ifndef ITTS_SERVICE_H
#define ITTS_SERVICE_H

#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <utils/String8.h>
#include <string>

using namespace android;
using namespace std;

enum{
	TRANSACTION_PREPARE = IBinder::FIRST_CALL_TRANSACTION + 0,
	TRANSACTION_SPEAK,
	TRANSACTION_CANCEL,
	TRANSACTION_IS_SPEAKING,
	TRANSACTION_SET_VOLUME,
};

class ITtsService : public IInterface{
	public:
		DECLARE_META_INTERFACE(TtsService);
		virtual bool prepare() = 0;
		virtual int speak(const string&, sp<IBinder>&) = 0;
		virtual void cancel(int id) = 0;
		virtual bool is_speaking(int id) = 0;
		virtual void set_volume(int volume) = 0;
};

class BnTtsService : public BnInterface<ITtsService> {
	public:
		virtual status_t onTransact(uint32_t code, const Parcel &data, Parcel *reply, uint32_t flag = 0);
};

#endif // ITTS_SERVICE_H
