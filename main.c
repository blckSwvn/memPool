#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct block {
int size;
struct block *next;
struct block *prev;
char data[];
} block;

typedef struct {
  void *head;
  int size;
  block *tail;
} poolBoss;

typedef struct {
  void *start[100];
  int sizes[100];
  void *end[100];
  int listEnd;
} freeList;

void initPool(int poolSize, poolBoss *boss, freeList *free){
  boss->head = malloc(poolSize);
  free = malloc(sizeof(*free));
  boss->tail = NULL;
  boss->size = (poolSize-sizeof(poolBoss));
}

void relinkBlocks(block *mem){
  block *current = mem;
  block *previous = current->prev;
  block *ahead = current->next;
}

void printBlocks(block *mem, poolBoss *boss, freeList *free){
  block *current = mem;
  printf("Boss: adress:%p poolAvailable:%d next:%p\n", boss, boss->size, mem);
  printf("memory");
  while(current){
    int size = sizeof(*current->data);
    printf(" adress:%p size:%d next:%p\n", current, size, current->next);
    current = current->next;
  }
  printf("freeList");
  for(int i = 0; i < 100;){
    printf("startPtr:%p size:%d endPtr:%p\n", free->start[i], free->sizes[i], free->end[i]);
    i++;
  }
}

void* palloc(poolBoss *boss, int data){
  if(!boss->size){
    return NULL;
  }

  block *blck = (block *)((char *)boss->head + boss->size);
  boss->size = boss->size-(data+sizeof(block));
  blck->size = data;
  block *current = blck;
  block *previous = current->prev;
  previous->next = blck;
  current->next = blck;
  blck->prev = current;
  blck->next = NULL;
  return blck;
}

void pfree(void* memptr, poolBoss *boss, freeList *free){
  if(!memptr){
    return NULL;
  }
  block *current = (block *)memptr;
  block *previous = current->prev;
  block *ahead = current->next;
  previous->next = ahead;
  if(ahead){
    ahead->prev = previous;
  } 
  free->start[free->listEnd] = current;
  free->sizes[free->listEnd] = current->size;
  free->end[free->listEnd] = current->data+1; 
  free->listEnd++;
  boss->size += sizeof(block) + current->size;
  memset(current, 0, sizeof(block) + current->size);
}

int main(){
  int poolSize;
  int data;
  //char data[5] = "12345";
  printf("poolsize?\n");
  scanf("%d",&poolSize);
  freeList free;
  poolBoss boss;
  initPool(poolSize, &boss &free);
  block mem;
  mem.prev = NULL;
  mem.next = NULL;
  mem.size = 0;

  printf("usrData?\n");
  scanf("%i", data);
  palloc(&boss, data);
  printBlocks(&mem, &boss, &free);

  return 0;
}
