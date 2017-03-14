#ifndef RKRUNTIME_SERVICE_H
#define RKRUNTIME_SERVICE_H

#include "IRKRuntimeService.h"

using namespace android;

class RKRuntimeService : public BnRKRuntimeService{
	public:
		static char const* getServiceName(){
			return "RKRuntimeService";
		}
};

#endif // RKRUNTIME_SERVICE_H
