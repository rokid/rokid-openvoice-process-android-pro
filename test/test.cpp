#define DESCRIPTOR "com.rokid.openvoice.native.RuntimeService"

#include <stdio.h>
#include <stdlib.h>
#include <binder/Parcel.h>
#include <binder/IServiceManager.h>  

using namespace android;

int main(int argc, char** argv){
    if(argc < 2){
        printf("%s\n", "needs two arguments");
        return 0;
    }
    sp<IBinder> proxy = defaultServiceManager()->getService(String16("runtime_native")); 
    Parcel data, reply;
    printf("%s\n", argv[1]);
    if(proxy != NULL && proxy->isBinderAlive()){
        if(strcmp(argv[1], "init") == 0){
            data.writeInterfaceToken(String16(DESCRIPTOR));
            proxy->transact(IBinder::FIRST_CALL_TRANSACTION, data, &reply); 
            reply.readExceptionCode();
        }else if(strcmp(argv[1], "start") == 0){
            if(argc < 3){
                printf("%s\n", "needs three arguments");
                return 0;
            }
            data.writeInterfaceToken(String16(DESCRIPTOR));
            data.writeInt32(atoi(argv[2]));
            proxy->transact(IBinder::FIRST_CALL_TRANSACTION + 1, data, &reply); 
            reply.readExceptionCode();
        }else if(strcmp(argv[1], "state") == 0){
            if(argc < 3){
                printf("%s\n", "needs three arguments");
                return 0;
            }
            data.writeInterfaceToken(String16(DESCRIPTOR));
            data.writeInt32(atoi(argv[2]));
            proxy->transact(IBinder::FIRST_CALL_TRANSACTION + 2, data, &reply); 
            reply.readExceptionCode();
        }else if(strcmp(argv[1], "netchg") == 0){
            if(argc < 3){
                printf("%s\n", "needs three arguments");
                return 0;
            }
            data.writeInterfaceToken(String16(DESCRIPTOR));
            data.writeInt32(atoi(argv[2]));
            proxy->transact(IBinder::FIRST_CALL_TRANSACTION + 3, data, &reply); 
            reply.readExceptionCode();
        }
    }else{
        printf("%s\n", "proxy is null");
    }
    return 0;
}
