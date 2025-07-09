#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct block {
  int length;
  struct block *next;
  struct block *prev;
  char data[];
} block;

typedef struct {
  block *head;
  block *tail;
  void *base;
  int total_size;
  int free_size;
} poolBoss;

typedef struct {
  void *start[100];
  int sizes[100];
  int listEnd;
} freeList;

void initPool(int poolSize, poolBoss *boss, freeList *flist) {
  boss->base = malloc(poolSize);
  if (!boss->base) {
    fprintf(stderr, "Failed to allocate memory pool\n");
    exit(1);
  }
  boss->head = NULL;
  boss->tail = NULL;
  boss->total_size = poolSize;
  boss->free_size = poolSize;

  flist->listEnd = 0;
  memset(flist->start, 0, sizeof(flist->start));
  memset(flist->sizes, 0, sizeof(flist->sizes));
}

void* palloc(poolBoss *boss, freeList *flist, int dataSize) {
  if (dataSize <= 0) return NULL;
  int needed = sizeof(block) + dataSize;

  if (boss->free_size < needed) return NULL;

  void *freePtr = NULL;
  int freeIndex = -1;
  for (int i = 0; i < flist->listEnd; i++) {
    if (flist->sizes[i] >= dataSize) {
      freePtr = flist->start[i];
      freeIndex = i;
      break;
    }
  }

  block *newMem = NULL;
  if (freePtr != NULL) {
    newMem = (block *)freePtr;
    flist->start[freeIndex] = NULL;
    flist->sizes[freeIndex] = 0;
    boss->free_size -= needed;
    newMem->length = dataSize;
  } else {
    if (boss->head == NULL) {
      newMem = (block *)boss->base;
      newMem->prev = NULL;
      newMem->next = NULL;
      boss->head = newMem;
      boss->tail = newMem;
    } else {
      char *endOfTail = (char *)boss->tail + sizeof(block) + boss->tail->length;
      newMem = (block *)endOfTail;
      newMem->prev = boss->tail;
      newMem->next = NULL;
      boss->tail->next = newMem;
      boss->tail = newMem;
    }
    newMem->length = dataSize;
    boss->free_size -= needed;
  }

  return newMem->data;
}

void pfree(void* memptr, poolBoss *boss, freeList *flist) {
  if (!memptr) return;

  block *current = (block *)((char *)memptr - sizeof(block));

  if (current->prev) {
    current->prev->next = current->next;
  } else {
    boss->head = current->next;
  }
  if (current->next) {
    current->next->prev = current->prev;
  } else {
    boss->tail = current->prev;
  }

  if (flist->listEnd < 100) {
    flist->start[flist->listEnd] = current;
    flist->sizes[flist->listEnd] = current->length;
    flist->listEnd++;
  } else {
    fprintf(stderr, "Free list full! Cannot add freed block.\n");
  }

  boss->free_size += sizeof(block) + current->length;

  memset(current, 0, sizeof(block) + current->length);
}

void printBlocks(poolBoss *boss, freeList *flist) {
  printf("Memory Pool: total=%d, free=%d\n", boss->total_size, boss->free_size);
  printf("Allocated blocks:\n");
  block *current = boss->head;
  while (current) {
    printf("  block at %p length=%d prev=%p next=%p\n",
           (void *)current, current->length,
           (void *)current->prev, (void *)current->next);
    current = current->next;
  }
  printf("Freelist:\n");
  for (int i = 0; i < flist->listEnd; i++) {
    if (flist->start[i]) {
      printf("  free block at %p size=%d\n", flist->start[i], flist->sizes[i]);
    }
  }
  printf("\n");
}

int main() {
  int poolSize;
  printf("Enter pool size (bytes): ");
  scanf("%d", &poolSize);

  poolBoss boss;
  freeList flist;
  initPool(poolSize, &boss, &flist);

  int allocSize;
  printf("Enter allocation size: ");
  scanf("%d", &allocSize);

  void *mem1 = palloc(&boss, &flist, allocSize);
  if (mem1) {
    printf("Allocated memory at %p\n", mem1);
    memset(mem1, 0xAB, allocSize);
  } else {
    printf("Allocation failed\n");
  }

  printBlocks(&boss, &flist);

  printf("Freeing allocated memory...\n");
  pfree(mem1, &boss, &flist);

  printBlocks(&boss, &flist);

  free(boss.base);  // Now calls standard free function correctly
  return 0;
}
