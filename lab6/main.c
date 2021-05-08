#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "banker.h"

#define MAX_COMM_LEN 100

int getint(FILE *in){
    int ret = 0;
    char c;
    while(!isdigit((c = fgetc(in))) && c != EOF);
    do{
        ret *= 10;
        ret += c - '0';
    } while(isdigit(c = fgetc(in)) && c != EOF);
    return ret;
}

int main(int argc, char **argv){
    char command[MAX_COMM_LEN] = {};
    char c;
    FILE *in = fopen("banker_in.txt", "r");
    for(int i = 0; i < NUMBER_OF_RESOURCES; ++i){
        available[i] = atoi(argv[i+1]);
    }
    for(int i = 0; i < NUMBER_OF_CUSTOMERS; ++i){
        for(int j = 0; j < NUMBER_OF_RESOURCES; ++j)
            need[i][j] = maximum[i][j] = getint(in);
    }
    while(1){
        scanf("%s", command);
        switch(command[0]){
            case 'R':
                if(command[1] == 'Q'){
                    int num, request[NUMBER_OF_RESOURCES];
                    scanf("%d", &num);
                    for(int i = 0; i < NUMBER_OF_RESOURCES; ++i)
                        scanf("%d", request+i);
                    if(request_resources(num, request) == 0)
                        printf("Allocation succeeded.\n");
                    else
                        printf("Allocation failed.\n");
                }
                else if(command[1] == 'L'){
                    int num, request[NUMBER_OF_RESOURCES];
                    scanf("%d", &num);
                    for(int i = 0; i < NUMBER_OF_CUSTOMERS; ++i)
                        scanf("%d", request+i);
                    release_resources(num, request);
                }
                else
                    printf("Invalid command: %s\n", command);
                break;
            case '*':
                if(strcmp(command, "*") == 0){
                    printf("available:\n");
                    for(int i = 0; i < NUMBER_OF_RESOURCES; ++i)
                        printf("%d ", available[i]);
                    putchar('\n');

                    printf("maximum:\n");
                    for(int i = 0; i < NUMBER_OF_CUSTOMERS; ++i){
                        for(int j = 0; j < NUMBER_OF_RESOURCES; ++j)
                            printf("%d ", maximum[i][j]);
                        putchar('\n');
                    }

                    printf("allocation:\n");
                    for(int i = 0; i < NUMBER_OF_CUSTOMERS; ++i){
                        for(int j = 0; j < NUMBER_OF_RESOURCES; ++j)
                            printf("%d ", allocation[i][j]);
                        putchar('\n');
                    }

                    printf("need:\n");
                    for(int i = 0; i < NUMBER_OF_CUSTOMERS; ++i){
                        for(int j = 0; j < NUMBER_OF_RESOURCES; ++j)
                            printf("%d ", need[i][j]);
                        putchar('\n');
                    }
                }
                else
                    printf("Invalid command: %s\n", command);
                break;
            case 'E':
                if(strcmp(command, "EXIT") == 0)
                    return 0;
                else
                    printf("Invalid command: %s\n", command);
        }
    }
    return 0;
}