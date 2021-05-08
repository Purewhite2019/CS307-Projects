#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "list.h"
#include "task.h"
#include "cpu.h"

#define RR_TIME_SLICE 10

static struct node head = {.task = NULL, .next = NULL};
static int tid_cur = 0;

// add a task to the list 
void add(char *name, int priority, int burst){
    // printf("Adding %s, %d, %d\n", name, priority, burst);
    fflush(stdout);
    struct node *np = (struct node*)malloc(sizeof(struct node)), *iter = &head;

    np->task = (Task*)malloc(sizeof(Task));
    np->task->name = (char*)malloc(strlen(name) * sizeof(char));
    np->task->tid = __sync_fetch_and_add(&tid_cur, 1);
    np->task->priority = priority;
    np->task->burst = burst;
    np->next = NULL;
    strcpy(np->task->name, name);

    while(iter->next != NULL) iter = iter->next;
    iter->next = np;
    // printf("Added %s, %d, %d\n", name, priority, burst);
    fflush(stdout);
}

// invoke the scheduler
void schedule(){
    int isResponsed[tid_cur];
    double turnaround_time = 0, waiting_time = 0, response_time = 0, current_time = 0;
    struct node *last = head.next;
    memset(isResponsed, 0, tid_cur * sizeof(int));
    while(last->next != NULL)
        last = last->next;
    last->next = head.next;

    for(struct node *iter = head.next; iter != NULL; iter = iter->next){
        if(isResponsed[iter->task->tid] == 0){
            response_time += current_time;
            waiting_time -= iter->task->burst;
            isResponsed[iter->task->tid] = 1;
        }
        if(iter->task->burst > RR_TIME_SLICE){
            current_time += RR_TIME_SLICE;
            run(iter->task, RR_TIME_SLICE);
            iter->task->burst -= RR_TIME_SLICE;
            last = iter;
        }
        else{
            current_time += iter->task->burst;
            turnaround_time += current_time;
            waiting_time += current_time;
            run(iter->task, iter->task->burst);
            iter->task->burst = 0;
            if(last == iter){
                turnaround_time += current_time;
                break;
            }
            last->next = iter->next;
        }
    }
    response_time /= tid_cur;
    waiting_time /= tid_cur;
    turnaround_time /= tid_cur;
    printf("Average Response Time: %lf\nAverage Waiting Time: %lf\nAverage Turnaround Time: %lf\n", response_time, waiting_time, turnaround_time);
}