#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#define uint8_max 255
#define MAX_BLOCK_SIZE 67108864  // 64MB
#define MAX_NUMBER_BLOCKS 5      // 2^32
#define DICT_SIZE 256

enum parameters { START, BLOCK_SIZE, SEQUENCE };
enum rle_compression { NOT_RLE_COMPRESSED = 'N', RLE_COMPRESSED = 'R' };

typedef struct {
    char* sf_code;  // codigo shannon-fano atribuido ao carater nesse bloco
    uint8_t index;  // index atribuido ao simbolo na em cada "peca" do offset
} SFTuple;

typedef struct block {
    size_t block_size;
    uint8_t number_symbols;
    size_t biggest_code_size;
    SFTuple symbol_dictionary[DICT_SIZE];  // O array tem comprimento 256 (ascii
                                           // extended?)
} Block;

typedef struct sequence {
    size_t number_blocks;
    size_t size_first_block;
    size_t size_last_block;
    enum rle_compression compression_type;
    Block blocks[MAX_NUMBER_BLOCKS];
} FullSequence;

// Otimização Matriz
typedef struct piece {
    uint8_t* code;
    size_t next;             // Next bit index * Nsimbolos
    size_t index_next_byte;  // Index do byte até ao qual se faz shift
} Piece;
