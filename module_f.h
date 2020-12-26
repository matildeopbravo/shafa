#ifndef __MODULE_F_H_
#define __MODULE_F_H_

#include "dynamic_arrays.h"

#define k 64000
#define K 640000
#define m 8000000
#define M 64000000

/* Codigo ASCII */
#define uint_Arroba 64
#define uint_PontoVirgula 59
#define uint_percentagem 37
#define uint_range 256

#define sucess 1
/* Errors related to function WriteFreq */
#define WriteFreq_ERROR_IN_FILE -1       /* ERROR: Opening file */
#define WriteFreq_ERROR_IN_FREQ -2       /* ERROR: Reading the frequency */
#define WriteFreq_ERROR_IN_BLOCKFILES -3 /* ERROR: Reading the block Files */

/* Errors related to function building */
#define BUILDING_ERROR_IN_FILE -1 /* ERROR: Opening file */

enum compression { NOT_COMPRESSED, COMPRESSED } check;

/* FILE */

/* struct for blocks */
typedef struct block {
  ByteVec *blocklist;
  struct block *prox;
  unsigned int block_size;
} Blocks;

/* struct for blocks_compressed */
typedef struct block_c {
  TuppleVec *tBList;
  struct block_c *prox;
  unsigned int block_size;
} Blocks_C;

/* struct to put on file on blocks */
typedef struct block_file {
  enum compression compression_type;
  Blocks *blocks;     // if compression_type == NOT_COMPRESSED -> NULL
  Blocks_C *blocks_c; // if compression_type == COMPRESSED -> NULL
  unsigned int num_blocks;
} BlockFiles;

/* FREQUENCY */
/* Struct for the blocks' frequency */
typedef struct freq_block {
  int freq[uint_range];
  struct freq_block *prox;
} FreqBlock;

#endif // __MODULE_F_H_
