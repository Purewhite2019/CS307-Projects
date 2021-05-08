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
        if(request[k] > Need[customer_num][k] || request[k] > Available[k])
            return -1;
        Available[k] -= request[k];
        Need[customer_num][k] -= request[k];
        Allocation[customer_num][k] += request[k];
    }
    for(int i = 0; i < NUMBER_OF_CUSTOMERS; ++i){
        for(int j = 0; j < NUMBER_OF_CUSTOMERS; ++j){
            while(isFed[j] && j < NUMBER_OF_CUSTOMERS) ++j; // Omit customers that are fed
            if(j == NUMBER_OF_CUSTOMERS)
                return -1;

            int canBeFed = 1;   // If customer j can be fed
            for(int k = 0; k < NUMBER_OF_RESOURCES; ++k){
                if(Need[j][k] > Available[k]){
                    canBeFed = 0;
                    break;
                }
            }
            if(canBeFed){
                isFed[j] = 1;
                for(int k = 0; k < NUMBER_OF_RESOURCES; ++k)
                    Available[k] += Allocation[j][k];
            }
        }
    }
    for(int k = 0; k < NUMBER_OF_RESOURCES; ++k){   // If request is out of Need bound || is out of Available bound
        available[k] -= request[k];
        need[customer_num][k] -= request[k];
        allocation[customer_num][k] += request[k];
    }
    return 0;
}

void release_resources(int customer_num, int release[]){
    for(int i = 0; i < NUMBER_OF_RESOURCES; ++i){
        if(allocation[customer_num][i] < release[i]){
            error(-1, 1, "Customer %d: releasing Request %d is greater than allocated resources %d.", customer_num, release[i], allocation[customer_num][i]);
            exit(1);
        }
        available[i] += release[i];
        allocation[customer_num][i] -= release[i];
        need[customer_num][i] += release[i];
    }
}