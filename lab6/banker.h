#include <string.h>
#include <stdio.h>
#include <errno.h>

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 4

extern int available[];
extern int maximum[][NUMBER_OF_RESOURCES];
extern int allocation[][NUMBER_OF_RESOURCES];
extern int need[][NUMBER_OF_RESOURCES];

int request_resources(int customer_num, int request[]);
void release_resources(int customer_num, int release[]);
