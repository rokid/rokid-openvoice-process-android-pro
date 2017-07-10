#define DESCRIPTOR "com.rokid.openvoice.openvoice_process"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <binder/Parcel.h>
#include <binder/IServiceManager.h>  

using namespace std;
using namespace android;
sp<IBinder> proxy = defaultServiceManager()->getService(String16("openvoice_proc")); 

void init(){
    Parcel data, reply;
    data.writeInterfaceToken(String16(DESCRIPTOR));
    proxy->transact(IBinder::FIRST_CALL_TRANSACTION, data, &reply); 
    reply.readExceptionCode();
}

void start_siren(int flag){
    Parcel data, reply;
    data.writeInterfaceToken(String16(DESCRIPTOR));
    data.writeInt32(flag);
    proxy->transact(IBinder::FIRST_CALL_TRANSACTION + 1, data, &reply); 
    reply.readExceptionCode();
}

void set_siren_state(int flag){
    Parcel data, reply;
    data.writeInterfaceToken(String16(DESCRIPTOR));
    data.writeInt32(flag);
    proxy->transact(IBinder::FIRST_CALL_TRANSACTION + 2, data, &reply); 
    reply.readExceptionCode();
}

void network_state_change(int flag){
    Parcel data, reply;
    data.writeInterfaceToken(String16(DESCRIPTOR));
    data.writeInt32(flag);
    proxy->transact(IBinder::FIRST_CALL_TRANSACTION + 3, data, &reply); 
    reply.readExceptionCode();
}

void automatic_test(int count){
    if(count < 0) return;
    while(count--){
        int index = rand() % 6;
        printf("============%d============\n", index);
        switch(index) {
            case 0:
                start_siren(0);
                break;
            case 1:
                start_siren(1);
                break;
            case 2:
                set_siren_state(0);
                break;
            case 3:
                set_siren_state(1);
                break;
            case 4:
                network_state_change(0);
                break;
            case 5:
                network_state_change(1);
                break;
            case 6:
                init();
                break;
        }
        sleep(2);
    }
    start_siren(1);
    network_state_change(1);
}

int main(int argc, char** argv){
    if(argc < 2){
        printf("Needs two arguments");
        return 0;
    }
    Parcel data, reply;
    printf("%s\n", argv[1]);
    if(proxy != NULL && proxy->isBinderAlive()){
        if(strcmp(argv[1], "init") == 0){
            init();
        }else if(strcmp(argv[1], "start") == 0){
            if(argc < 3){
                printf("Needs three arguments");
                return 0;
            }
            start_siren(atoi(argv[2]));
        }else if(strcmp(argv[1], "state") == 0){
            if(argc < 3){
                printf("Needs three arguments");
                return 0;
            }
            set_siren_state(atoi(argv[2]));
        }else if(strcmp(argv[1], "netchg") == 0){
            if(argc < 3){
                printf("Needs three arguments");
                return 0;
            }
            network_state_change(atoi(argv[2]));
        }else if(strcmp(argv[1], "auto") == 0){
            if(argc < 3){
                printf("Please enter the number of times");
                return 0;
            }
            automatic_test(atoi(argv[2]));
        }
    }else{
        printf("%s\n", "proxy is null");
    }
    return 0;
}
