#include "banker.h"

int available[NUMBER_OF_RESOURCES] = {};
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES] = {};
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES] = {};
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES] = {};

int request_resources(int customer_num, int request[]){
    int isFed[NUMBER_OF_CUSTOMERS] = {};
    int Available[NUMBER_OF_RESOURCES];
    int Allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES] = {};
    int Need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
    memcpy(Available, available, sizeof(available));
    memcpy(Allocation, allocation, sizeof(allocation));
    memcpy(Need, need, sizeof(need));

    for(int k = 0; k < NUMBER_OF_RESOURCES; ++k){   // If request is out of Need bound || is out of Available bound
        if(request[k] > Need[customer_num][k] || request[k] > Available[k]){
            return -1;
        }
        Available[k] -= request[k];
        Need[customer_num][k] -= request[k];
        Allocation[customer_num][k] += request[k];
    }

    int isRunning = 1;   // If customer j can be fed
    while(isRunning){
        isRunning = 0;
        for(int j = 0; j < NUMBER_OF_CUSTOMERS; ++j){
            if(isFed[j])
                continue;
            int canBeFed = 1;
            for(int k = 0; k < NUMBER_OF_RESOURCES; ++k){
                if(Need[j][k] > Available[k]){
                    canBeFed = 0;
                    break;
                }
            }
            if(canBeFed){
                isFed[j] = 1;
                isRunning = 1;
                for(int k = 0; k < NUMBER_OF_RESOURCES; ++k)
                    Available[k] += Allocation[j][k];
            }
        }
    }
    int isOK = 1;
    for(int i = 0; i < NUMBER_OF_CUSTOMERS; ++i)
        if(!isFed[i]){
            isOK = 0;
            break;
        }
    if(isOK){
        for(int k = 0; k < NUMBER_OF_RESOURCES; ++k){   // If request is out of Need bound || is out of Available bound
            available[k] -= request[k];
            need[customer_num][k] -= request[k];
            allocation[customer_num][k] += request[k];
        }
        return 0;
    }
    else
        return -1;
}

void release_resources(int customer_num, int release[]){
    for(int i = 0; i < NUMBER_OF_RESOURCES; ++i)
        if(allocation[customer_num][i] < release[i]){
            fprintf(stderr, "Customer %d: releasing request %d is greater than allocated resources %d.\n", customer_num, release[i], allocation[customer_num][i]);
            return;
        }
    for(int i = 0; i < NUMBER_OF_RESOURCES; ++i){
        available[i] += release[i];
        allocation[customer_num][i] -= release[i];
        need[customer_num][i] += release[i];
    }
}