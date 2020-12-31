#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "dynamic_arrays.h"
#define uint8_max 255
#define MAX_NUMBER_BLOCKS 100      // 2^32
#define DICT_SIZE 256

enum parameters { START,BLOCK_SIZE, SEQUENCE };
enum rle_compression { NOT_RLE_COMPRESSED = 'N', RLE_COMPRESSED = 'R' };

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
    enum rle_compression compression_type;
    Block blocks[MAX_NUMBER_BLOCKS];
} FullSequence;

int write_block (Block * block, FILE * fp_shaf, FILE * fp_input);
int write_file (FullSequence * full_seq, char const * shaf_file, char const * input_file);
int count_numbers(char* c, FILE* fp_cod);
void shift_piece(Piece* previous, Piece* current, uint16_t number_symbols);
void make_offset(Block* block, int offset);
uint8_t* make_code(const char* str, size_t size, size_t CODE_MAX_SIZE);
void start_matrix(Block* block, uint8_t* symbols);
void matrix_optimization(Block* block, uint8_t* symbols);
int read_cod(char* cod_file, FullSequence* full_seq);
void destructor(FullSequence* sequence,char*,char*);
void print_dictionary(FullSequence* full_seq);
void print_console(FullSequence* , double time,char* filename);
void print_matrix(FullSequence * full_seq);
void module_c(char  * symbol_file);
