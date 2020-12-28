#ifndef __MODULE_F_H_
#define __MODULE_F_H_

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "data.h"
#include "dynamic_arrays.h"
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

/* Errors related to function module_f */
#define Module_f_ERROR_IN_FILE -1       /* ERROR: Opening file */
#define Module_f_ERROR_IN_FREQ -2       /* ERROR: Reading the frequency */
#define Module_f_ERROR_IN_BLOCKFILES -3 /* ERROR: Reading the block Files */

FILE *openFile(const char *file, const char *mode);

void closeFile(FILE **file);

long double calcCompress(TuppleVec const *self);

TuppleVec *compress(ByteVec const *self);

int write_compressed(FILE *file, BlockFiles const *self);

int checkSum(ByteVec *self);

int building_blocks(FILE *file, BlockFiles *self, size_t n_blocks,size_t size_last_block, size_t block_size);

int compress_blocks(BlockFiles *self, int FORCE_FLAG);

long double calcCompress_blocks(BlockFiles const *self);

FreqBlock *calFreq(BlockFiles const *file);

FreqBlock *calFreq_RLE(BlockFiles *file);

int writeFreq(FILE *fp_in, const char *filename, BlockFiles *BlockFile,FreqBlock *freq);

void print_module_f(const char *filename, BlockFiles const *self,long double percentage_compression, double const time);

void determine_size(char const *filename, size_t *n_blocks,size_t *size_last_blocks, size_t *block_size);

int module_f(char const *filename, size_t const the_block_size,int FORCE_FLAG);

#endif // __MODULE_F_H_
