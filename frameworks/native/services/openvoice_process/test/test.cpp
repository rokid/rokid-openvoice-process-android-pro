#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <binder/IServiceManager.h>

#include "IVoiceService.h"

using namespace std;
using namespace android;
sp<IVoiceService> proxy = interface_cast<IVoiceService>(defaultServiceManager()->getService(String16("openvoice_process")));

void automatic_test(int count) {
    if(count < 0) return;
    while(count--) {
        int index = rand() % 6;
        printf("=========%d========%d=======\n", count, index);
        switch(index) {
        case 0:
            proxy->start_siren(false);
            break;
        case 1:
            proxy->start_siren(true);
            break;
        case 2:
            proxy->set_siren_state(0);
            break;
        case 3:
            proxy->set_siren_state(1);
            break;
        case 4:
            proxy->network_state_change(true);
            break;
        case 5:
            proxy->network_state_change(false);
            break;
        case 6:
            proxy->setup();
            break;
        }
        sleep(2);
    }
    proxy->start_siren(true);
    proxy->network_state_change(true);
}

int main(int argc, char** argv) {
    if(argc < 2) {
        printf("Needs two arguments");
        return 0;
    }
    Parcel data, reply;
    printf("%s\n", argv[1]);
    if(proxy != NULL) {
        if(strcmp(argv[1], "init") == 0) {
            proxy->setup();
        } else if(strcmp(argv[1], "start") == 0) {
            if(argc < 3) {
                printf("Needs three arguments");
                return 0;
            }
            proxy->start_siren(atoi(argv[2]));
        } else if(strcmp(argv[1], "state") == 0) {
            if(argc < 3) {
                printf("Needs three arguments");
                return 0;
            }
            printf("dfasdfadsfa\n");
            proxy->set_siren_state(atoi(argv[2]));
        } else if(strcmp(argv[1], "netchg") == 0) {
            if(argc < 3) {
                printf("Needs three arguments");
                return 0;
            }
            proxy->network_state_change(atoi(argv[2]));
        } else if(strcmp(argv[1], "auto") == 0) {
            if(argc < 3) {
                printf("Please enter the number of times");
                return 0;
            }
            automatic_test(atoi(argv[2]));
        }
    } else {
        printf("%s\n", "proxy is null");
    }
    return 0;
}
