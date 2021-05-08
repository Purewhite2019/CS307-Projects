#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define MAX_COMM_LEN 100

enum {Free, Used};

typedef struct mem_region_node{
    size_t size;
    int type;
    char *name;
    struct mem_region_node *last, *next;
}MemRegionNode;

void init(MemRegionNode *head, int _memsize);
void request(MemRegionNode *head, const char *name, char strategy, int need);
void release(MemRegionNode *head, const char *name);
void compact(MemRegionNode *head);
void report(MemRegionNode *head);

int main(int argc, char **argv){
    MemRegionNode head;
    int memsize;
    char comm[MAX_COMM_LEN], name[MAX_COMM_LEN], c;

    if(argc != 2){
        printf("Usage: %s $MemSize.\n", argv[0]);
        return 1;
    }
    memsize = atol(argv[1]);
    init(&head, memsize);
    
    while(1){
        printf("allocator>");
        scanf("%s", comm);
        if(strcmp(comm, "RQ") == 0){
            scanf("%s", name);
            scanf("%d", &memsize);
            while(!isalpha(c = getchar()) && c != EOF);
            request(&head, name, c, memsize);
        }
        else if(strcmp(comm, "RL") == 0){
            scanf("%s", name);
            release(&head, name);
        }
        else if(strcmp(comm, "C") == 0){
            compact(&head);
        }
        else if(strcmp(comm, "STAT") == 0){
            report(&head);
        }
        else if(strcmp(comm, "X") == 0){
            break;
        }
    }
    return 0;
}

void init(MemRegionNode *head, int _memsize){
    if(_memsize <= 0){
        printf("Error: invalid memory size.\n");
        fflush(stdout);
        exit(1);
    }
    memset(head, 0, sizeof(MemRegionNode));
    head->size = _memsize;
}

void request(MemRegionNode *head, const char *name, char strategy, int need){
    if(need <= 0){
        printf("Error: invalid memory size.\n");
        fflush(stdout);
        exit(1);
    }
    MemRegionNode *choice = NULL;
    switch(strategy){
        case 'F':
            for(choice = head; choice != NULL && (choice->type == Used || choice->size < need); choice = choice->next);
            break;
        case 'B':
            for(MemRegionNode *iter = head; iter != NULL; iter = iter->next){
                if(iter->type == Free && iter->size >= need){
                    if(choice == NULL || choice->size < iter->size)
                        choice = iter;
                }
            }
            break;
        case 'W':
            for(MemRegionNode *iter = head; iter != NULL; iter = iter->next){
                if(iter->type == Free && iter->size >= need){
                    if(choice == NULL || choice->size > iter->size)
                        choice = iter;
                }
            }
            break;
        default:
            printf("Error: invalid allocation strategy: <%c>\n", strategy);
            fflush(stdout);
            exit(1);
    }
    
    if(choice == NULL){
        printf("Allocation failed: no enough free space.\n");
        return;
    }
    else{
        choice->type = Used;
        choice->name = strdup(name);
        if(choice->size > need){
            MemRegionNode *np = (MemRegionNode*)malloc(sizeof(MemRegionNode));
            np->size = choice->size - need;
            choice->size = need;
            np->type = Free;
            np->name = NULL;
            np->last = choice;
            np->next = choice->next;
            choice->next = np;
            if(np->next) np->next->last = np;
        }
    }
}

void release(MemRegionNode *head, const char *name){
    MemRegionNode *tmp;
    for(MemRegionNode *iter = head; iter != NULL; iter = iter->next){
        if(iter->type == Used && strcmp(iter->name, name) == 0) {
            iter->type = Free;
            free(iter->name);
            iter->name = NULL;
            if(iter->next && iter->next->type == Free){
                tmp = iter->next;
                iter->size += tmp->size;
                iter->next = tmp->next;
                if(tmp->next)
                    tmp->next->last = iter;
                free(tmp);
            }
            if(iter->last && iter->last->type == Free){
                if(iter->last == head) continue;
                tmp = iter->last;
                iter->size += tmp->size;
                iter->last = tmp->last;
                if(tmp->last)
                    tmp->last->next = iter;
                free(tmp);
            }
        }
    }
}

void compact(MemRegionNode *head){
    size_t accumulation = 0;
    for(MemRegionNode *iter = head->next; iter != NULL; iter = iter->next){
        if(iter->type == Free){
            accumulation += iter->size;
            if(iter->last) iter->last->next = iter->next;
            if(iter->next) iter->next->last = iter->last;
            free(iter);
        }
    }
    if(head->type == Free)
        head->size += accumulation;
    else{
        MemRegionNode *np = (MemRegionNode*)malloc(sizeof(MemRegionNode));
        np->size = accumulation;
        np->type = Free;
        np->last = head;
        np->next = head->next;
        head->next = np;
        if(np->next) np->next->last = np;
    }
}

void report(MemRegionNode *head){
    size_t curaddr = 0;
    for(MemRegionNode *iter = head; iter != NULL; iter = iter->next){
        printf("Addresses [%lu, %lu] ", curaddr, curaddr+iter->size-1);
        if(iter->type == Free) printf("Unused\n");
        else printf("Process %s\n", iter->name);
        curaddr += iter->size;
    }
}