#ifndef IRUNTIME_SERVICE_H
#define IRUNTIME_SERVICE_H

#include <binder/IInterface.h>
#include <binder/Parcel.h>

using namespace android;

enum{
	TRANSACTION_SPEECH_NLP = 0,
	TRANSACTION_SIREN_NLP = 1
};

class IRuntimeService : public IInterface {
	public:	
		DECLARE_META_INTERFACE(RuntimeService);
};

class BnRuntimeService : public BnInterface<IRuntimeService> {
	public:	
		virtual status_t onTransact(uint32_t code, const Parcel &data, Parcel *reply, uint32_t flag = 0);
};

#endif // IRUNTIME_SERVICE_H
