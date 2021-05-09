/**
 * Implementation of thread pool.
 */
/*: Question: Why "initialization from incompatible pointer type" when using LinkNode implementation? */
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include "threadpool.h"

#define QUEUE_SIZE 10
#define NUMBER_OF_THREADS 3

#define TRUE 1

// this represents work that has to be 
// completed by a thread in the pool
typedef struct 
{
    void (*function)(void *p);
    void *data;
}
task;

#ifndef USE_ARRAY_QUEUE
typedef struct {
    task *content;
    struct LinkNode *next;
} LinkNode;
LinkNode head = {.content = NULL, .next = NULL};
LinkNode *tail = &head;
#endif
#ifdef USE_ARRAY_QUEUE
task queue[QUEUE_SIZE+1] = {};
int head = 0, tail = 0;
#endif
pthread_mutex_t queue_mutex;
sem_t queue_cnt;

// the worker bee
pthread_t bee[NUMBER_OF_THREADS];
pthread_attr_t bee_attr[NUMBER_OF_THREADS];

int isWorking;

// insert a task into the queue
// returns 0 if successful or 1 otherwise, 
int enqueue(task t) 
{
    pthread_mutex_lock(&queue_mutex);
#ifndef USE_ARRAY_QUEUE
    if((tail = (tail->next = (LinkNode*)malloc(sizeof(LinkNode)))) == NULL){
        pthread_mutex_unlock(&queue_mutex);
        return 1;
    }
    if((tail->content = (task*)malloc(sizeof(task))) == NULL){
        pthread_mutex_unlock(&queue_mutex);
        return 1;
    }
    tail->content->data = t.data;
    tail->content->function = t.function;
#endif

#ifdef USE_ARRAY_QUEUE
    if((tail + 1) % (QUEUE_SIZE + 1) == head){
        pthread_mutex_unlock(&queue_mutex);
        return 1;
    }
    queue[tail].data = t.data;
    queue[tail].function = t.function;
    tail = (tail + 1) % (QUEUE_SIZE + 1);
#endif
    pthread_mutex_unlock(&queue_mutex);
    return 0;
}

// remove a task from the queue
task dequeue() 
{
    pthread_mutex_lock(&queue_mutex);
    task worktodo = {.data = NULL, .function = NULL};
#ifndef USE_ARRAY_QUEUE
    if(head.next != NULL){
        LinkNode *tmp = head.next;
        worktodo.data = tmp->content->data;
        worktodo.function = tmp->content->function;
        head.next = tmp->next;
        free(tmp->content);
        free(tmp);
    }
#endif

#ifdef USE_ARRAY_QUEUE
    if(tail != head){
        worktodo.data = queue[head].data;
        worktodo.function = queue[head].function;
        head = (head + 1)  % (QUEUE_SIZE + 1);
    }
#endif
    pthread_mutex_unlock(&queue_mutex);
    return worktodo;
}

// the worker thread in the thread pool
void *worker(void *param)
{
    // execute the task
    while(isWorking){
        sem_wait(&queue_cnt);
        if(isWorking == 0) break;
        task worktodo = dequeue();
        if(worktodo.function != NULL || worktodo.data != NULL)
            execute(worktodo.function, worktodo.data);
        // else{
            // printf("Error: NULL work.\n");
            // fflush(stdout);
        // }
    }
    pthread_exit(0);
}

/**
 * Executes the task provided to the thread pool
 */
void execute(void (*somefunction)(void *p), void *p)
{
    (*somefunction)(p);
}

/**
 * Submits work to the pool.
 */
int pool_submit(void (*somefunction)(void *p), void *p)
{
    if(enqueue((task){.data = p, .function = somefunction}) == 0){
        sem_post(&queue_cnt);
        return 0;
    }
    else
        return 1;
}

// initialize the thread pool
void pool_init(void)
{
    isWorking = 1;
    if(pthread_mutex_init(&queue_mutex, NULL) != 0){
        printf("Error: pthread_mutex_init() failed.\n");
        exit(1);
    }
    if(sem_init(&queue_cnt, 0, 0) != 0){
        printf("Error: sem_init() failed.\n");
        exit(1);
    }
    for(int i = 0; i < NUMBER_OF_THREADS; ++i){
        if(pthread_attr_init(bee_attr+i) != 0){
            printf("Error: pthread_attr_init() failed.\n");
            exit(1);
        }
        if(pthread_create(bee+i,NULL,worker,NULL) != 0){
            printf("Error: pthread_create() failed.\n");
            exit(1);
        }
    }
}

// shutdown the thread pool
void pool_shutdown(void)
{
    // printf("Shutting down...\n");
    isWorking = 0;
    for(int i = 0; i < NUMBER_OF_THREADS; ++i)
        sem_post(&queue_cnt);

    for(int i = 0; i < NUMBER_OF_THREADS; ++i){
        if(pthread_join(bee[i],NULL) != 0){
            printf("Error: pthread_join() failed.\n");
            exit(1);
        }
    }
}
