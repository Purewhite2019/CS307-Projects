#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#define VM_SIZE 65536
#define PAGE_NUM 128

// #define TLB_LRU                 // Comment this to use FIFO in TLB replacement.
// #define PR_LRU                  // Comment this to use FIFO in page replacement.

typedef int8_t page_t[256];

typedef union{
    int addr_int;
    uint32_t addr_uint;
    uint8_t addr[4];            // Little endian: addr[3], addr[2], addr[1], addr[0] ~ [31:24][23:16][15:8][7:0]
} addr_t;

struct{
    uint8_t page_num;
    uint8_t frame_num;
    int tick;
} TLB[16];

int page_table[256];
page_t *memory[PAGE_NUM];
int memref_tick[PAGE_NUM];

int cur_tick = 0;
int page_fault_cnt = 0, TLB_hit_cnt = 0;

uint8_t page_num(int addr){
    return ((addr_t)addr).addr[1];
}

uint8_t offset(int addr){
    return ((addr_t)addr).addr[0];
}

void error(const char* msg, int code){
    printf("%s\n", msg);
    fflush(stdout);
    exit(code);
}

page_t* swap_in(FILE *fd, int page_num){
    page_t *ret = (page_t*)malloc(sizeof(page_t));

    if(page_num < 0 || page_num > 255)
        error("Error: address out of bound.", 1);
    if(fseek(fd, page_num * sizeof(page_t), SEEK_SET) != 0){
        perror("Error: fseek() failed.");
        exit(1);
    }
    if(fread(ret, sizeof(page_t), 1, fd) != 1){
        perror("Error: fread() failed.");
        exit(1);
    }
    return ret;
}

int8_t maccess(FILE *fd, int addr){
    uint8_t pn = page_num(addr), off = offset(addr), frame_num = -1;
    for(int i = 0; i < 16; ++i){
        if(TLB[i].tick >= 0 && TLB[i].page_num == pn){  // TLB hit
            frame_num = TLB[i].frame_num;
            #ifdef TLB_LRU
            TLB[i].tick = cur_tick;
            #endif
            ++TLB_hit_cnt;
            ++cur_tick;
            return frame_num*sizeof(page_t)+off;
        }
    }
    // TLB miss
    if(page_table[pn] == -1){   // page fault
        int replace_num = 0;
        for(int i = 1; i < PAGE_NUM; ++i){
            if(memref_tick[replace_num] > memref_tick[i])
                replace_num = i;
        }
        for(int i = 0; i < 256; ++i){
            if(page_table[i] == replace_num){
                page_table[i] = -1;
                break;
            }
        }
        page_table[pn] = replace_num;
        memory[replace_num] = swap_in(fd, pn);
        memref_tick[replace_num] = cur_tick;
        ++page_fault_cnt;
    }

    frame_num = page_table[pn]; // Refer to page table
    #ifdef PR_LRU
    memref_tick[frame_num] = cur_tick;
    #endif

    int replace_num = 0;        // TLB replacement using LRU
    for(int i = 1; i < 16; ++i){
        if(TLB[replace_num].tick > TLB[i].tick)
            replace_num = i;
    }
    TLB[replace_num].page_num = pn;
    TLB[replace_num].frame_num = frame_num;
    TLB[replace_num].tick = cur_tick;

    ++cur_tick;
    return (*(memory[frame_num]))[off];
}

int paddr(FILE *fd, int addr){
    uint8_t pn = page_num(addr), off = offset(addr), frame_num = -1;
    for(int i = 0; i < 16; ++i){
        if(TLB[i].tick >= 0 && TLB[i].page_num == pn){  // TLB hit
            frame_num = TLB[i].frame_num;
            return frame_num*sizeof(page_t)+off;
        }
    }
    if(page_table[pn] == -1){
        error("Error: page fault.", 1);
    }
    frame_num = page_table[pn];
    return frame_num*sizeof(page_t)+off;
}

int main(int argc, char **argv){
    if(argc != 2){
        printf("Usage: %s <input file>.\n", argv[0]);
    }
    FILE *fd, *in;
    int taddr, val;
    memset(page_table, -1, sizeof(page_table));
    memset(TLB, -1, sizeof(TLB));
    memset(memref_tick, -1, sizeof(memref_tick));
    if((fd = fopen("BACKING_STORE.bin", "r")) == NULL){
        perror("Error: fopen() failed.");
        exit(1);
    }
    if((in = fopen(argv[1], "r")) == NULL){
        perror("Error: fopen() failed.");
        exit(1);
    }
    while(1){
        fscanf(in, "%d", &taddr);
        if(feof(in)) break;
        val = maccess(fd, taddr);
        printf("Virtual address: %d Physical address: %d Value: %d\n", taddr, paddr(fd, taddr), val);
    }
    printf("In totally %d memory accesses, page fault rate: %lf, TLB hit rate: %lf\n", cur_tick, 1e2*page_fault_cnt/cur_tick, 1e2*TLB_hit_cnt/cur_tick);
    return 0;
}