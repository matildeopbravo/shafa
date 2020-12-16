#ifndef __MODULE_F_H_
#define __MODULE_F_H_

#include "dynamic_arrays.h"

/* Codigo ASCII */
#define uint_Arroba 64
#define uint_PontoVirgula 59
#define uint_percentagem 37
#define uint_range 255

#define sucess 1
/* Errors related to function WriteFreq */
#define WriteFreq_ERROR_IN_FILE -1       /* ERROR: Opening file */
#define WriteFreq_ERROR_IN_FREQ -2       /* ERROR: Reading the frequency */
#define WriteFreq_ERROR_IN_BLOCKFILES -3 /* ERROR: Reading the block Files */

enum compression { NOT_COMPRESSED, COMPRESSED } check;

/* FILE */

/* struct for blocks */
typedef struct block {
  ByteVec *blocklist;
  struct block *prox;
  unsigned int block_size;
} Blocks;

/* struct to put on file on blocks */
typedef struct block_file {
  enum compression compression_type;
  Blocks *blocks;
  unsigned int num_blocks;
} BlockFiles;

/* FREQUENCY */
/* Struct for the blocks' frequency */
typedef struct freq_block {
  int freq[uint_range];
  struct freq_block *prox;
} FreqBlock;

#endif // __MODULE_F_H_
