#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define uint8_max 255

typedef struct block {
    uint8_t  block_size;
    int sequence [uint8_max];
} Block;

typedef struct full_sequence {
   unsigned int number_blocks;
   unsigned int size_first_block;
   unsigned int size_last_block;
   Block block [] ;
}*FullSequence;

