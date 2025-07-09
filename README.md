# Custom Memory Pool in C

A simple, lightweight memory pool implementation in C designed for manual memory management without relying on `malloc`/`free` for every allocation.

## Overview

This project implements a memory pool that allocates and frees memory blocks within a fixed-size pre-allocated buffer. It uses a linked list of memory blocks with flexible array members to store variable-sized data and maintains a free list for reusing freed blocks.

The pool avoids merging free blocks by discarding nodes entirely upon free and reusing freed spaces for future allocations, helping reduce fragmentation in a simple way.

## Features

- Fixed-size memory pool with manual management  
- Linked list of memory blocks with `length` and flexible array member for data  
- Free list to track reusable freed memory  
- Allocation (`palloc`) and deallocation (`pfree`) functions  
- Avoids complex block merging by design  
- Suitable for embedded or constrained environments  

## Usage

```c
#include "mempool.h"

int main() {
  poolBoss boss;
  freeList free;
  int poolSize = 1024 * 1024; // 1MB pool
  int allocSize = 256;

  initPool(poolSize, &boss, &free);

  char *ptr = (char *)palloc(&boss, &free, allocSize);
  if(ptr) {
    // Use allocated memory
  }

  pfree(ptr, &boss, &free);

  free(boss.base);  // Clean up pool memory at end
  return 0;
}
