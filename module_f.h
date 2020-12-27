/**
 * @file module_f.h
 * @author Mariana Dinis Rodrigues e Mike
 * @date 27 Dezembro 2020
 * @brief File containing the data structures
 */
#ifndef __MODULE_F_H_
#define __MODULE_F_H_

#include "dynamic_arrays.h"

#define k 64000    /**< k */
#define K 640000   /**< K */
#define m 8000000  /**< m */
#define M 64000000 /**< M */

/* Codigo ASCII */
#define uint_Arroba 64       /**< Uint do @ */
#define uint_PontoVirgula 59 /**< Uint do ; */
#define uint_percentagem 37  /**< Uint da % */
#define uint_range 256       /**< Podemos ter valores de 0 a 255 */

#define sucess 1
/* Errors related to function WriteFreq */
#define WriteFreq_ERROR_IN_FILE -1 /**< ERROR: Opening file */
#define WriteFreq_ERROR_IN_FREQ -2 /**< ERROR: Reading the frequency */
#define WriteFreq_ERROR_IN_BLOCKFILES                                          \
  -3 /**< ERROR: Reading the block Files                                       \
      */

/* Errors related to function building */
#define BUILDING_ERROR_IN_FILE -1 /**< ERROR: Opening file */

/* Errors related to function module_f */
#define Module_f_ERROR_IN_FILE -1       /**< ERROR: Opening file */
#define Module_f_ERROR_IN_FREQ -2       /**< ERROR: Reading the frequency */
#define Module_f_ERROR_IN_BLOCKFILES -3 /**< ERROR: Reading the block Files */

enum compression { NOT_COMPRESSED, COMPRESSED } check; /**< Compressao */

/* FILE */

/** @brief Struct for blocks
 * Ficheiro por blocos
 */
typedef struct block {
  ByteVec *blocklist;      /**< um bloco */
  unsigned int block_size; /**< tamanho do bloco */
  struct block *prox;      /**< Apontador para o proximo bloco*/
} Blocks;

/** @brief Struct for blocks_compressed */
typedef struct block_c {
  TuppleVec *tBList;       /**< um bloco */
  unsigned int block_size; /**< tamanho do bloco */
  struct block_c *prox;    /**< Apontador para o proximo bloco*/
} Blocks_C;

/** @brief struct to put on file on blocks */
typedef struct block_file {
  enum compression
      compression_type;    /**< if compression_type == NOT_COMPRESSED -> NULL
                            * if compression_type == COMPRESSED -> NULL*/
  Blocks *blocks;          /**< Blocos sem compressao */
  Blocks_C *blocks_c;      /**< Blocos com compressao */
  unsigned int num_blocks; /**< numero de blocos */
} BlockFiles;

/* FREQUENCY */
/** @brief Struct for the blocks' frequency */
typedef struct freq_block {
  int freq[uint_range]; /**< Frequencias dos simbolos de um bloco */
  /* unsigned int block_size; */
  struct freq_block *prox; /**< Apontador para o proximo bloco */
} FreqBlock;

/* Functions */
/**
 * @brief
 *
 * @param
 * @return
 */

/**
 * @brief Inicializa a nossa estrutura das frequencias
 * Esta funcao recebe um array de inteiros, inicializa a nossa estrutura
 * de dados de Frequencias e coloca o aray na mesma.
 * @param Array de inteiros
 * @return FreqBlock -> com o array dado
 */
FreqBlock *initializeFreq(int array[uint_range]);

/**
 * @brief Liberta o espaco da nossa estutura de dados de Freqeuncias
 *
 * @param FreqBlocks
 * @return void
 */
void free_Freq(FreqBlock *);

FreqBlock *initializeFreq(int array[uint_range]);

#endif /* __MODULE_F_H_ */
