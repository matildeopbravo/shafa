#ifndef DATA_H
#define DATA_H 

#include <stdint.h>  /* for uint8_t */
#define BUFFER_SIZE 1024

enum compression{N , R}type;

/* FILE */
	
/* Block Values */
typedef struct NODE {
    void *value;
    struct NODE *prox; 
} *BLOCK_LIST ;

/* struct for blocks */
typedef struct Block {
    BLOCK_LIST *blocklist;
    int block_size;
    struct Block *prox;
} *BLOCKS;

/* struct to put on file on blocks */
typedef struct {
    enum compression type; /* R == compression with RLE (1 -> true) ||  N == no compression (0 -> False) */
    int num_blocks;
    BLOCKS *Blocks;
} *Block_file;

/* FREQUENCY */

/* Struct for the blocks' frequency */
typedef struct freq_block {
    int freq[256];
    struct freq_block *prox;
} *Freq_blocks ;

#endif /* DATA_H */
