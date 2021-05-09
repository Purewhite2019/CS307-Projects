#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void swap(int *a, int *b);
void quicksort(int a[], int l, int r);
void *sort(void *param);
void *merge(void *param);

typedef struct {
    int *a;
    int l;
    int r;
} sort_param;

typedef struct {
    int *src;
    int *dest;
    int lbegin;
    int rbegin;
    int size;
} merge_param;

int *src = NULL, *dest = NULL;

int main(int argc, char *argv[]){
    pthread_t tid1, tid2, tid3; pthread_attr_t attr1, attr2, attr3;
    src = (int*)malloc((argc-1)*sizeof(int));
    dest = (int*)malloc((argc-1)*sizeof(int));
    sort_param s1p = {.a = src, .l = 0, .r = (argc-1)/2-1}, s2p = {.a = src, .l = (argc-1)/2, .r = argc-2};
    merge_param mp = {.src = src, .dest = dest, .lbegin = 0, .rbegin = (argc-1)/2, .size = argc-1};

    for(int i = 1; i < argc; ++i){
        src[i-1] = atoi(argv[i]);
    }
    
    pthread_attr_init(&attr1);
    pthread_attr_init(&attr2);
    pthread_attr_init(&attr3);

    pthread_create(&tid1, &attr1, sort, &s1p);
    pthread_create(&tid2, &attr2, sort, &s2p);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    pthread_create(&tid3, &attr3, merge, &mp);
    pthread_join(tid3, NULL);
    
    for(int i = 0; i < argc-1; ++i)
        printf("%d ", dest[i]);
    putchar('\n');
    return 0;
}

void swap(int *a, int *b){
    int c = *a;
    *a = *b;
    *b = c;
}

void quicksort(int a[], int l, int r){
    int pivot = a[(l+r)/2];
    int i = l, j = r;
    do{
        while(a[i] < pivot) ++i;
        while(a[j] > pivot) --j;
        if(i <= j){
            swap(a+i, a+j);
            ++i;
            --j;
        }
    } while(i <= j);
    if(l < j) quicksort(a, l, j);
    if(i < r) quicksort(a, i, r);
}

void *sort(void *param){
    quicksort(((sort_param*)param)->a, ((sort_param*)param)->l, ((sort_param*)param)->r);
    pthread_exit(0);
}

void *merge(void *param){
    merge_param *p = (merge_param*)param;
    int i = p->lbegin, j = p->rbegin, k = 0;
    while(i < p->rbegin && j < p->size){
        if(p->src[i] < p->src[j])
            p->dest[k++] = p->src[i++];
        else
            p->dest[k++] = p->src[j++];
    }
    while(i < p->rbegin){
        p->dest[k++] = p->src[i++];
    }
    while(j < p->size){
        p->dest[k++] = p->src[j++];
    }
    pthread_exit(0);
}