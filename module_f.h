/**
 * @file module_f.h
 * @author Mariana Dinis Rodrigues e Mike
 * @date 28 Dezembro 2020
 * Responsible for compressing RLE and calculating frequencies.
 * @brief Responsavel pela compressao RLE e pelo calculo das frequencias.
 \brief Responsavel pela compressao RLE e pelo calculo das frequencias.
 */
#ifndef __MODULE_F_H_
#define __MODULE_F_H_

#include "data.h"
#include "dynamic_arrays.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* Codigo ASCII */
#define uint_Arroba 64
#define uint_PontoVirgula 59
#define uint_percentagem 37
#define uint_range 256

#define sucess 1
/* Errors related to function WriteFreq */
#define WriteFreq_ERROR_IN_FILE -1       /** ERROR: Opening File */
#define WriteFreq_ERROR_IN_FREQ -2       /** ERROR: Reading the Frequency */
#define WriteFreq_ERROR_IN_BLOCKFILES -3 /** ERROR: Reading the BlockFiles */

/* Errors related to function building */
#define BUILDING_ERROR_IN_FILE -1 /** ERROR: Opening File */

/* Errors related to function module_f */
#define Module_f_ERROR_IN_FILE -1        /** ERROR: Opening File */
#define Module_f_ERROR_IN_FREQ -2        /** ERROR: Reading the Frequency */
#define Module_f_ERROR_IN_BLOCKFILES -3  /** ERROR: Reading the BlockFiles */
#define Module_f_ERROR_IN_COMPRESSION -4 /** ERROR: Compression. */

/**
 \brief Calcula a compressão de um determinado ByteTupple.
 */
long double calcCompress(TuppleVec const *self);

/**
 \brief Faz a conversão RLE de um determinado ByteVec, returnando um TuppleVec.
 */
TuppleVec *compress(ByteVec const *self);

/**
 \brief Escreve a compressâo RLE de um BlockFile num determinado file.
 */
int write_compressed(FILE *file, BlockFiles const *self);

/**
 \brief Verifica se a compressão de um determinado bloco é superior ou igual
 a 5.

 \return 1 se a compressão obtida for superior ou igual 5, 0 caso contrário.
 */
int checkSum(ByteVec *self);

/**
 \brief Esta função recebe um file e coloca-o por blocos na nossa estrutura de
 dados.
 \param FILE File para leitura.
 \param *self Iremos guardar a nossa estrutura de dados aqui.
 \param size_last_block Tamanho do último bloco.
 \param block_size Tamanho dos restantes blocos.
 \return 1 em caso de sucesso.
*/
int building_blocks(FILE *file, BlockFiles *self, size_t n_blocks,
                    size_t size_last_block, size_t block_size);

/**
 \brief Executa a compressão RLE dos nossos blocos.
 * Esta função so executa a compressao por blocos se, ou a compressão do
 * primeiro bloco for superior ou igual a cinco, ou se a nossa FORCE_FLAG for 1.
 \return 1 caso se tenha comprimido os blocos, 0 caso contrário.
 */
int compress_blocks(BlockFiles *self, int FORCE_FLAG);

/**
 \brief Calcula a compressão média das compressao dos blocos.
 */
long double calcCompress_blocks(BlockFiles const *self);

/**
 \brief Recebe um determinado BlockFiles e retorna as frequências de cada bloco.
 \param *file Blockfiles Não comprimido.
 \return FreqBlock
 */
FreqBlock *calFreq(BlockFiles const *file);

/**
 \brief Recebe um TuppleVec e calcula a frequencias dos simbolos
 \param *block_size_ No final da função aoresenta o tamanho real do bloco.
 \return 1 em caso de sucesso
 */
int TuppleVec_freq(TuppleVec *vec, size_t *block_size_, int array[uint_range]);

/**
 \brief Recebe um determinado BlockFiles e retorna as frequências de cada bloco.
 \param *file Blockfiles Comprimido.
 \return FreqBlock
 */
FreqBlock *calFreq_RLE(BlockFiles *file);

/**
 \brief Escreve as frequências dos blocos num determinado file dado.
 \return 1 em caso de sucesso.
 */
int writeFreq(FILE *fp_in, const char *filename, BlockFiles *BlockFile,
              FreqBlock *freq);

/**
 \brief Imprime os resultados obtidos do MODULE_F.
 */
void print_module_f(const char *filename, BlockFiles const *self,
                    long double percentage_compression, double const time);

/**
 \brief Função que executa todo o MODULE_F.
 \param *filename Ficheiro a ser analisado.
 \param the_block_size Tamanho do bloco a ser analisado.
 \param FORCE_FLAG Flag que determina se a compressão RLE é forçada.
 \return 1 em caso de sucesso.
 */
int module_f(char const *filename, size_t const the_block_size, int FORCE_FLAG);

/**
 \brief Avisa que argumentos dados não são válidos.
 */
void argumentos_invalidos();

/**
 \brief Função que converte o char recebido para o tamanho correto do bloco.
 */
size_t convert_block_size(char letter);

/**
  \brief Função que analisa os argumentos dados e chama o módule f.
  \return 1 em caso de sucesso
 */
int call_module_f(char *filename, char *options[]);

#endif // __MODULE_F_H_
