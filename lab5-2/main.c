#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include "buffer.h"

sem_t empty, full;
int isRunning;

void *producer(void *param);
void *consumer(void *param);

int main(int argc, char *argv[]){
    int run_time = 0, num_producer = 0, num_consumer = 0;
    pthread_t *producer_tid, *consumer_tid;
    pthread_attr_t *producer_attr, *consumer_attr;
    if(argc != 4){
        printf("Usage: %s <time to sleep before terminating> <# of producer threads> <# of consumer threads>.\n", argv[0]);
        return 0;
    }
    if((run_time = atoi(argv[1])) <= 0){
        printf("<time to sleep before terminating> should be positive integer.\n");
        return 0;
    }
    if((num_producer = atoi(argv[2])) <= 0){
        printf("<# of producer threads> should be positive integer.\n");
        return 0;
    }
    if((num_consumer = atoi(argv[3])) <= 0){
        printf("<# of consumer threads> should be positive integer.\n");
        return 0;
    }
    
    isRunning = 1;
    producer_tid = (pthread_t*)malloc(sizeof(pthread_t) * num_producer);
    consumer_tid = (pthread_t*)malloc(sizeof(pthread_t) * num_consumer);
    producer_attr = (pthread_attr_t*)malloc(sizeof(pthread_attr_t) * num_producer);
    consumer_attr = (pthread_attr_t*)malloc(sizeof(pthread_attr_t) * num_consumer);

    sem_init(&empty, 0, BUFFER_SIZE-1);
    sem_init(&full, 0, 0);

    for(int i = 0; i < num_producer; ++i){
        assert(pthread_attr_init(&producer_attr[i]) == 0);
    }
    for(int i = 0; i < num_consumer; ++i){
        assert(pthread_attr_init(&consumer_attr[i]) == 0);
    }
    for(long i = 0; i < num_producer; ++i){
        assert(pthread_create(&producer_tid[i], &producer_attr[i], producer, (void*)i) == 0);
    }
    for(long i = 0; i < num_consumer; ++i){
        assert(pthread_create(&consumer_tid[i], &consumer_attr[i], consumer, (void*)i) == 0);
    }

    sleep(run_time);

    isRunning = 0;
    for(int i = 0; i < num_producer; ++i) sem_post(&empty);
    for(int i = 0; i < num_consumer; ++i) sem_post(&full);

    for(int i = 0; i < num_producer; ++i){
        assert(pthread_join(producer_tid[i], NULL) == 0);
    }
    for(int i = 0; i < num_consumer; ++i){
        assert(pthread_join(consumer_tid[i], NULL) == 0);
    }
    return 0;
}

void *producer(void *param){
    long number = (long)param;
    buffer_item item;

    while(isRunning){
        sleep((unsigned long long)rand() * 3 / RAND_MAX);
        item = rand();
        sem_wait(&empty);
        if(!isRunning) break;
        if(insert_item(item)){
            printf("producer #%ld report error condition\n", number);
            sem_post(&empty);
        }
        else{
            printf("producer #%ld produced %d\n", number, item);
            sem_post(&full);
        }
            
    }
    pthread_exit(0);
}
void *consumer(void *param){
    long number = (long)param;
    buffer_item item;

    while(isRunning){
        sleep((unsigned long long)rand() * 3 / RAND_MAX);
        sem_wait(&full);
        if(!isRunning) break;
        if(remove_item(&item)){
            printf("consumer #%ld report error condition\n", number);
            sem_post(&full);
        }
        else{
            printf("consumer #%ld consumed %d\n", number, item);
            sem_post(&empty);
        }
    }
    pthread_exit(0);
}