//#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#define uint8_max 255
#define MAX_BLOCK_SIZE 67108864      // 64MB
#define MAX_NUMBER_BLOCKS 5 // 2^32
#define DIC_SIZE 256

// not sure about data types yet (eg: long long) and the value of maxblock etc
enum parameters { START, BLOCK_SIZE, SEQUENCE };
enum rle_compression { NOT_RLE_COMPRESSED = 'N', RLE_COMPRESSED = 'R' };

// typedef struct {
//  bool is_garbage;
//  char * sf_code; // codigo shannon-fano atribuido ao carater nesse bloco
//} SFTuple;

typedef struct block {
  size_t block_size;
  char *symbol_dictionary[DIC_SIZE]; // O array tem comprimento 256 (ascii
                                     // extended?)
} Block;

typedef struct {
  size_t number_blocks;
  size_t size_first_block;
  size_t size_last_block;
  enum rle_compression compression_type;
  Block blocks[MAX_NUMBER_BLOCKS];
} FullSequence;
