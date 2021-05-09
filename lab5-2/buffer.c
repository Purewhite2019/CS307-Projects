#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <pthread.h>
#include "buffer.h"

static buffer_item buffer[BUFFER_SIZE];
static int head, tail;

static pthread_mutex_t mutex;

void init(void){
    pthread_mutex_init(&mutex, NULL);
    head = tail = 0;
    memset(buffer, 0, sizeof(buffer));
}

int insert_item(buffer_item item){
    pthread_mutex_lock(&mutex);
    if((tail + 1) % BUFFER_SIZE == head){
        pthread_mutex_unlock(&mutex);
        return -1;
    }
    memcpy(&buffer[tail], &item, sizeof(buffer_item));
    tail = (tail + 1) % BUFFER_SIZE;
    pthread_mutex_unlock(&mutex);
    return 0;
}
int remove_item(buffer_item *item){
    pthread_mutex_lock(&mutex);
    if(tail == head){
        pthread_mutex_unlock(&mutex);
        return -1;
    }
    memcpy(item, &buffer[head], sizeof(buffer_item));
    head = (head + 1) % BUFFER_SIZE;
    pthread_mutex_unlock(&mutex);
    return 0;
}