#ifndef IRUNTIME_SERVICE_H
#define IRUNTIME_SERVICE_H

#include <binder/IInterface.h>
#include <binder/Parcel.h>

using namespace android;

enum{
	TRANSACTION_SET_SIREN_STATUS = 0x00,
	TRANSACTION_GET_SIREN_STATUS,
};

class IRuntimeService : public IInterface {
	public:	
		DECLARE_META_INTERFACE(RuntimeService);
		virtual void set_siren_state(const int&) = 0;
		virtual int get_siren_state() = 0;
};

class BnRuntimeService : public BnInterface<IRuntimeService> {
	public:	
		virtual status_t onTransact(uint32_t code, const Parcel &data, Parcel *reply, uint32_t flag = 0);
};

#endif // IRUNTIME_SERVICE_H
