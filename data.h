#ifndef DATA_H
#define DATA_H

#include <stdint.h>  /* for uint8_t */
#define BUFFER_SIZE 1024
#define uint_range 256

enum compression {NOT_COMPRESSED, COMPRESSED};

/* FILE */

/* Block Values */
typedef struct node {
    uint8_t value;
    struct node *prox;
} *BlockList;

/* struct for blocks */
typedef struct block {
    BlockList blocklist;
    struct block *prox;
    unsigned int block_size;
} *Blocks;

/* struct to put on file on blocks */
typedef struct block_file{
   enum compression compression_type ;
   Blocks blocks;
   unsigned int num_blocks;
} *BlockFiles;

/* FREQUENCY */
/* Struct for the blocks' frequency */
typedef struct freq_block {
   int freq[uint_range];
   struct FreqBlock *prox;
} *FreqBlock ;

/* SHANNON FANO CODING */

/* Struct for symbol data */
typedef struct symbol {
    char symbolID;
    int freq;
    char *code;
} Symbol;

#endif /* DATA_H */
