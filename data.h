#ifndef DATA_H
#define DATA_H

#include <stdint.h> /* for uint8_t */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dynamic_arrays.h"

#define BUFFER_SIZE 1024
#define uint_range 256

#define k 64000
#define K 640000
#define m 8000000
#define M 64000000

enum compression { NOT_COMPRESSED, COMPRESSED };

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
  /* unsigned int block_size; */
  struct freq_block *prox;
} FreqBlock;

/* SHANNON FANO CODING */

/* Struct for symbol data */
typedef struct symbol {
  char symbolID;
  int freq;
  char *code;
} Symbol;

char *replace_str(char *str, char *orig, char *rep);

ByteVec *loadArray(FILE *file, size_t block_size);

FreqBlock *initializeFreq(int array[uint_range]);

void free_Freq(FreqBlock *e);

Blocks *initializeBlocks();

void free_Blocks(Blocks *e);

Blocks_C *initializeBlocks_C();

void free_Blocks_C(Blocks_C *e);

BlockFiles *initializeBlockFiles();

void free_Blocks_file(BlockFiles *e);

void addedBlockTOBloc_file(BlockFiles *e, Blocks *block);

void addedBlock_CTOBloc_file(BlockFiles *e, Blocks_C *self);

void arrayToFreqBlock(int array[uint_range], FreqBlock *e);

void print_freq(FreqBlock *freq);

void printByteVec(ByteVec const *self);

void printEqual(TuppleVec const *vec);

#endif /* DATA_H */
