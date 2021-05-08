#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "list.h"
#include "task.h"
#include "cpu.h"

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

    while(iter->next != NULL && iter->next->task->priority > priority) iter = iter->next;
    if(iter->next == NULL)
        iter->next = np;
    else{
        np->next = iter->next;
        iter->next = np;
    }
    // printf("Added %s, %d, %d\n", name, priority, burst);
    fflush(stdout);
}

// invoke the scheduler
void schedule(){
    int n = 0;
    double turnaround_time = 0, waiting_time = 0, response_time = 0, current_time = 0;
    for(struct node *iter = head.next; iter != NULL; iter = iter->next){
        response_time += current_time;
        waiting_time += current_time;
        run(iter->task, iter->task->burst);
        current_time += iter->task->burst;
        turnaround_time += current_time;
        ++n;
    }
    response_time /= n;
    waiting_time /= n;
    turnaround_time /= n;
    printf("Average Response Time: %lf\nAverage Waiting Time: %lf\nAverage Turnaround Time: %lf\n", response_time, waiting_time, turnaround_time);
}