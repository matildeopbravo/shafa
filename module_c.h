#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#define uint8_max 255
#define MAX_BLOCK_SIZE 67108864
#define MAX_NUMBER_BLOCKS 4294967296

// not sure about data types yet (eg: long long) and the value of maxblock etc

enum parameters { START, BLOCK_SIZE, SEQUENCE };
enum rle_compression { NOT_RLE_COMPRESSED = 'N', RLE_COMPRESSED = 'R' };

typedef struct {
  unsigned char c;
  unsigned char
      sf_code[]; // codigo shannon-fano atribuido ao carater nesse bloco

} SFTuple;

typedef struct block {
  long long block_size;
  uint8_t sequence[MAX_BLOCK_SIZE];
  SFTuple symbol_dictionary[];

} Block;

typedef struct {
  unsigned long number_blocks;
  unsigned long
      size_first_block; // not sure se vai ser necessário, depois vemos
  unsigned long size_last_block;
  enum rle_compression compression_type;
  Block block[MAX_NUMBER_BLOCKS];
} FullSequence;
