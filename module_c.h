#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "dynamic_arrays.h"
#define DICT_SIZE 256

enum parameters { BLOCK_SIZE, SEQUENCE };

typedef struct {
    char* sf_code;  // codigo shannon-fano atribuido ao carater nesse bloco
    uint8_t index;  // index atribuido ao simbolo na em cada "peca" do offset
} SFTuple;

// Otimização Matriz
typedef struct piece {
    uint8_t* code;
    size_t next;             // Next bit index * Nsimbolos
    size_t byte_index;  // Index do byte até ao qual se faz shift
} Piece;

typedef struct {
    size_t block_size_before;
    size_t block_size_after;
    uint16_t number_symbols;
    size_t biggest_code_size;
    Piece * matrix;
    SFTuple symbol_dictionary[DICT_SIZE];
} Block;

typedef struct sequence {
    size_t number_blocks;
    size_t size_first_block;
    size_t size_last_block;
    Block * blocks;
} FullSequence;

void module_c(char  * symbol_file);
