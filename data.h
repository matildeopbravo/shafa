/**
 * @file data.h
 * @date 28 Dezembro 2020
 */
#ifndef DATA_H
#define DATA_H

#include "dynamic_arrays.h"
#include <stdint.h> /* for uint8_t */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024
#define uint_range 256

/* Tamanho dos blocos */
#define k 65536   /**<  */
#define K 655360  /**<  */
#define m 8388608 /**<  */

#define M 67108864 /**<  */
enum compression { NOT_COMPRESSED, COMPRESSED };

/* FILE */
/**
\brief Struct for Blocks NOT_COMPRESSED
*/
typedef struct block {
  ByteVec *blocklist;
  unsigned int block_size; /**< Tamanho do bloco */
  struct block *prox;
} Blocks;

/**
/brief Struct for blocks_compressed
*/
typedef struct block_c {
  TuppleVec *tBList;
  unsigned int block_size; /**< Tamanho do bloco */
  struct block_c *prox;
} Blocks_C;

/**
\brief Struct to put on file on blocks
*/
typedef struct block_file {
  enum compression compression_type; /**< Compression_type */
  unsigned int num_blocks;           /**< Número de blocos. */
  Blocks *blocks;     /**< If compression_type == NOT_COMPRESSED -> NULL */
  Blocks_C *blocks_c; /**< If compression_type == COMPRESSED -> NULL */
} BlockFiles;

/* FREQUENCY */
/**
\brief Struct for the blocks' frequency
*/
typedef struct freq_block {
  int freq[uint_range]; /**< Freqência relativa ao bloco */
  struct freq_block *prox;
} FreqBlock;

char *replace_str(char *str, char *orig, char *rep);

ByteVec *loadArray(FILE *file, size_t block_size);

/**
\brief Função que recebe um array e inicializa a estrutura das Frequências.
\param array Array de inteiros
\return FreqBlock.
*/
FreqBlock *initializeFreq(size_t array[uint_range]);

/**
\brief Função que liberta a memória alocada dos FreqBlock.
*/
void free_Freq(FreqBlock *e);

/**
\brief Função que alloca a memória necessária do Blocks.
*/
Blocks *initializeBlocks();

/**
\brief Função que liberta a memória alocada dos Blocks.
*/
void free_Blocks(Blocks *e);

/**
\brief Função que alloca a memória necessária do Blocks_C.
*/
Blocks_C *initializeBlocks_C();

/**
\brief Função que liberta a memória alocada dos Blocks_C.
*/
void free_Blocks_C(Blocks_C *e);

/**
\brief Função que alloca a memória necessária do BlockFiles.
*/
BlockFiles *initializeBlockFiles();

/**
\brief Função que liberta a memória alocada dos BlocksFiles.
*/
void free_Blocks_file(BlockFiles *e);

/**
\brief Função que adiciona um dado Blocks a um determinado BlockFiles.
*/
void addedBlockTOBloc_file(BlockFiles *e, Blocks *block);

/**
\brief Função que adiciona um dado Blocks_C a um determinado BlockFiles.
*/
void addedBlock_CTOBloc_file(BlockFiles *e, Blocks_C *self);

/**
\brief Função que adiciona um dado array de inteiros a FreqBlock.
*/
void arrayToFreqBlock(size_t array[uint_range], FreqBlock *e);

/**
\brief Função que imprime um FreqBlock.
*/
void print_freq(FreqBlock *freq);

/**
\brief Função que imprime um ByteVec.
*/
void printByteVec(ByteVec const *self);

void printEqual(TuppleVec const *vec);

/**
\brief Função que returna o tamanho do ultimo bloco.
*/
size_t size_last_block_C_rle(Blocks_C *self);

#endif /* DATA_H */
