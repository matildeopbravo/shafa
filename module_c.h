/**
 * @file module_c.h
 * @author Matilde Oliveira Pizarro Bravo
 * @date 3 Janeiro 2021
 * @brief Definições das structs e enums utilizados no módulo C.
*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "dynamic_arrays.h"
#define DICT_SIZE 256

/**
\brief Enum que indica se está a ser lido um tamanho do bloco ou uma sequência no ficheiro cod
*/
enum parameters { BLOCK_SIZE, SEQUENCE };

/**
\brief Estrutura de dados que pretende simular um tuplo em que o primeiro elemento é uma string que contém um código Shannon-Fano e o segundo elemento contém o índice do símbolo.
*/
typedef struct {
    char* sf_code; /** codigo shannon-fano atribuido ao carater nesse bloco */
    uint8_t index;  /** index atribuido ao simbolo em cada "peca" do offset */
} SFTuple;

/**
\brief Estrutura de dados que representa a "peça" a ser utilizada para contruir a codificação final, recorrendo à otimização com matriz de bytes. Cada offset terá tantas peças quanto o número de símbolos presentes.
*/
typedef struct piece {
    uint8_t* code; /** Array de inteiros de 8 bits que permitirá codificar um simbolo*/
    size_t next; /**Posição da peça seguinte a ser usada na codificação de um bloco */
    size_t byte_index;  /** Índice do byte até ao qual se faz shift */
} Piece;

/**
\brief Estrutura de dados que contém informação relevante sobre um dado bloco retirada do ficheiro .cod.
*/
typedef struct {
    size_t block_size_before; /**Tamanho do bloco original */
    size_t block_size_after; /** Tamanho do bloco após a codificação com matriz de bytes */
    uint16_t number_symbols; /**Número total de símbolos diferentes presentes no ficheiro de texto original */
    size_t biggest_code_size; /**Tamanho do maior código Shannon-Fano gerado*/
    Piece * matrix; /**Matriz bidimensional que contem as peças para cada um dos 8 offsets */
    SFTuple symbol_dictionary[DICT_SIZE]; /** Dicionário de símbolos indexado pelo valor que o simbolo tem. Se um dado simbolo não estiver presente neste ficheiro, na posição igual ao valor desse simbolo tera um sf_code nulo.  */
} Block;
/**
 \brief Estrutura de dados que contém toda a informação retirada do ficheiro .cod e informação sobre cada um dos blocos.
*/
typedef struct sequence {
    size_t number_blocks; /**Número de blocos do ficheiro inicial */
    Block * blocks; /**Array de estruturas corrrespondentes a cada bloco */
} FullSequence;

void module_c(char  * symbol_file);
