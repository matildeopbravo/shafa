#ifndef MODULE_F_H
#define MODULE_F_H

#include <stdint.h>  /* for uint8_t */

#define BUFFER_SIZE 1024
#define uint_range 256

/* Codigo ASCII */
#define uintArroba 64
#define uint_PontoVirgula 59

/* Errors related to function WriteFreq */
#define WriteFreq_ERROR_IN_FILE -1 /* ERROR: Opening file */
#define WriteFreq_ERROR_IN_FREQ -2 /* ERROR: Reading the frequency */

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
   struct freq_block *prox;
} *FreqBlock ;

#endif /* MODULE_F_H */
