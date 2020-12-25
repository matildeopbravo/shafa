#include "module_c.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int count_numbers(char* c, FILE* fp_cod) {
    char tmp = *c;
    char buffer[20];
    int i = 0;
    do {
        buffer[i] = tmp;
        i++;
    } while ((tmp = fgetc(fp_cod)) != '@');

    buffer[i] = '\0';
    *c = tmp;
    return atoi(buffer);
}
void shift_piece (Block * block , Piece * previous, Piece * current){
    *current= { .code  =
                .next =
                .next_byte_index = }
}

void make_offset(Block * block,int offset) {
    Piece * pieces_current = block->matrix + block->number_symbols * offset;
    Piece * pieces_previous = block->matrix + block->number_symbols * (offset-1);

    for(int i = 0 ; i < block->number_symbols ; i++ ){
        shift_piece(block, pieces_previous+i, pieces_current+i);
    }


}

uint8_t* make_code(const char* str, size_t size, size_t CODE_MAX_SIZE) {

    uint8_t num = 0;
    uint8_t* arr = calloc(CODE_MAX_SIZE, sizeof(uint8_t));
    int entire_bytes = size / 8;
    int byte = 0;
    size_t i = 0;

    for (; byte < entire_bytes; byte++) {
        num = 0;
        for (int j = 7; j >= 0; i++, j--) {
            if (str[i] == '1') {
                num += 1 << j;  // num = num + 2^j
            }
        }
        arr[byte] = num;
    }
    if (size % 8 != 0) {
        num = 0;
        for (int j = 7; i < size; i++, j--) {
            if (str[i] == '1') {
                num += 1 << j;  // num = num + 2^j
            }
        }
        arr[byte] = num;
    }
    return arr;
}

const Piece* get_piece(
    Piece* matrix,
    uint8_t offset,
    uint8_t symbol_index,
    uint8_t number_symbols) {
    return matrix + number_symbols * offset + symbol_index;
}

void start_matrix (Block *  block, uint8_t * symbols) {
    Piece* matrix = malloc(sizeof(Piece) * block->number_symbols * 8);
    printf("%d symbols \n",block->number_symbols);
    block->matrix = matrix;
    // ( ( x - 1 ) | ( m - 1 ) ) + 1 multiple above current
    size_t CODE_MAX_SIZE = (((block->biggest_code_size - 1) | 7) + 9) / 8;

    // creates offset 0
    for (uint8_t i = 0 ; i < block->number_symbols ; i++ ) {
        SFTuple tuple = block->symbol_dictionary[symbols[i]];
        size_t size = strlen(tuple.sf_code);
        block->matrix[tuple.index] =(Piece){
            .code = make_code(tuple.sf_code, size, CODE_MAX_SIZE),
            .next = size * (size_t) block->number_symbols,
            .next_byte_index = size / 8
        };
    }
    printf("Code will have %zu bytes\n", CODE_MAX_SIZE);
}
void matrix_optimization(Block * block, uint8_t * symbols) {
    start_matrix(block,symbols);
    for (int i = 1; i < 8 ; i++) {
        make_offset(block,i);

    }
}

void print_dictionary(FullSequence* full_seq) {
    for (size_t i = 0; i < full_seq->number_blocks; i++) {
        printf("Bloco: %zu -> Size : %zu\n", i, full_seq->blocks[i].block_size);
        printf("Max code is : %zu\n", full_seq->blocks[i].biggest_code_size);
        for (int x = 0; x < DICT_SIZE; x++) {
            if (full_seq->blocks[i].symbol_dictionary[x].sf_code) {
                printf(
                    "<%s>", full_seq->blocks[i].symbol_dictionary[x].sf_code);
            }
            else {
                putchar('.');
            }
        }
        putchar('\n');
    }
}

void read_block(FILE* fp_cod, FullSequence* full_seq, char* c, int nblock) {
    uint8_t index = 0;
    uint8_t symbols [DICT_SIZE];
    int max_size = 0;
    SFTuple tuple;

    for (uint8_t ascii = 0; *c != '@';) {
        if (*c != ';') {
            char* sequence = malloc(sizeof(char) * 10);
            int x = 0;
            for (; *c != ';'; x++) {
                sequence[x] = *c;
                *c = fgetc(fp_cod);
            }
            sequence[x] = '\0';
            if (x > max_size) {
                max_size = x;
            }
            symbols[index] = ascii;
            tuple = (SFTuple) {.sf_code = sequence, .index = index++};
            full_seq->blocks[nblock].symbol_dictionary[ascii] =tuple;
        }
        else {
            *c = fgetc(fp_cod);
            ascii++;
        }
    }
    full_seq->blocks[nblock].number_symbols = index;
    full_seq->blocks[nblock].biggest_code_size = max_size;
    matrix_optimization(&full_seq->blocks[nblock], symbols);
}

int read_cod(char* cod_file, FullSequence* full_seq) {
    enum parameters param = START;
    int nblock = 0, not_finished = 1;
    size_t size = 0;
    int error = 1;

    FILE* fp_cod = fopen(cod_file, "r");
    char c = fgetc(fp_cod);

    for (int i = 0; not_finished; i++) {
        if (c == '@') {
            error = 0;
            switch (i) {
                case 0:
                    full_seq->compression_type = fgetc(fp_cod);
                    c = fgetc(fp_cod);
                    break;
                case 1:
                    c = fgetc(fp_cod);
                    full_seq->number_blocks = count_numbers(&c, fp_cod);
                    param = BLOCK_SIZE;
                    break;
                default:
                    c = fgetc(fp_cod);
                    not_finished = c - '0';
                    if (not_finished) {
                        if (param == BLOCK_SIZE) {
                            size = count_numbers(
                                &c,
                                fp_cod);  // o c já tem o primeiro digito
                            if (i == 2) {
                                full_seq->size_first_block = size;
                            }
                            full_seq->size_last_block = size;
                            full_seq->blocks[nblock].block_size = size;
                            param = SEQUENCE;
                        }
                        else {  // param == sequence
                            read_block(fp_cod,full_seq,&c,nblock++);
                            param = BLOCK_SIZE;
                        }
                    }
                    break;
            }
        }
    }
    return error;
}
void destructor (FullSequence * sequence) {

    for (size_t i = 0; i < sequence->number_blocks; i++) {
            free(sequence->blocks[i].matrix);
        for (int j = 0; j < DICT_SIZE; j++) {
            free(sequence->blocks[i].symbol_dictionary[j].sf_code);
        }
    }
    free(sequence);
}

int main() {
    // char const* symbol_file = "aaa.txt";
    // char* cod_file = malloc(strlen(symbol_file) + 5);  // (".cod" ++ '\0')
    // strcpy(cod_file, symbol_file);
    // strcat(cod_file, ".cod");  // codfile = "input.txt.rle.cod"
    FullSequence* my_sequence = calloc(sizeof(FullSequence), 1);
    read_cod("aaa.txt.rle.cod", my_sequence);
    print_dictionary(my_sequence);
    putchar('\n');
    for (int i = 0; i < my_sequence->number_blocks; i++) {
    size_t CODE_MAX_SIZE = (((my_sequence->blocks[i].biggest_code_size - 1) | 7) + 9) / 8;
        for (int j = 0;  j < my_sequence->blocks[i].number_symbols; j++) {
            printf("code begins \n");
            Piece x =*get_piece(my_sequence->blocks[i].matrix,0,j,my_sequence->blocks[i].number_symbols);
            for (int y = 0; y < CODE_MAX_SIZE ; y++ )
            printf("%d\n",x.code[y]);
        }
    }
    destructor(my_sequence);
    printf("%zu",sizeof(struct block));
    return 0;
}
