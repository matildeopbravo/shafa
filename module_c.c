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

void shift_piece(Piece* previous, Piece* current, size_t number_symbols) {
    for (size_t i = previous->next_byte_index; ; i--) {
        current->code[i] = (previous->code[i] >> 1);
        if (i == 0) break;
        current->code[i] += (previous->code[i - 1] & 1) << 7;
    }
    current->next = previous->next == 7 * number_symbols ? 0 : previous->next + number_symbols;
    current->next_byte_index = previous->next_byte_index;
    current->next_byte_index += previous->next == 7 * number_symbols ? 1 : 0;
}

void make_offset(Block* block, int offset) {
    Piece* pieces_current = block->matrix + block->number_symbols * offset;
    Piece* pieces_previous =
        block->matrix + block->number_symbols * (offset - 1);
    size_t number_symbols = block->number_symbols;

    for (int i = 0; i < block->number_symbols; i++) {
        shift_piece(pieces_previous + i, pieces_current + i, number_symbols);
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

Piece* get_piece(
    Piece* matrix,
    uint8_t offset,
    uint8_t symbol_index,
    uint8_t number_symbols) {
    return matrix + number_symbols * offset + symbol_index;
}

void start_matrix(Block* block, uint8_t* symbols) {
    Piece* matrix = malloc(sizeof(Piece) * block->number_symbols * 8);
    printf("%d symbols \n", block->number_symbols);
    block->matrix = matrix;
    // ( ( x - 1 ) | ( m - 1 ) ) + 1 multiple above current
    size_t CODE_MAX_SIZE = (((block->biggest_code_size - 1) | 7) + 9) / 8;

    // creates offset 0
    for (uint8_t i = 0; i < block->number_symbols; i++) {
        SFTuple tuple = block->symbol_dictionary[symbols[i]]; // symbols has value of existing symbols
        size_t size = strlen(tuple.sf_code);
        block->matrix[tuple.index] = (Piece){
            .code = make_code(tuple.sf_code, size, CODE_MAX_SIZE),
            .next = (size % 8) * (size_t) block->number_symbols,
            .next_byte_index = size / 8};
    }
    for (uint8_t i = 1 ; i < 8 ; i++) {
        for (int j = 0 ; j < block->number_symbols; j++){
            (block->matrix + block->number_symbols * i + j)  ->code = malloc(sizeof(uint8_t)*CODE_MAX_SIZE);
        }
    }
    printf("Code will have %zu bytes\n", CODE_MAX_SIZE);
}
void matrix_optimization(Block* block, uint8_t* symbols) {
    start_matrix(block, symbols);
    for (int i = 1; i < 8; i++) {
        make_offset(block, i);
    }
}

void read_block(FILE* fp_cod, FullSequence* full_seq, char* c, int nblock) {
    uint8_t index = 0;
    uint8_t symbols[DICT_SIZE];
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
            tuple = (SFTuple){.sf_code = sequence, .index = index++};
            full_seq->blocks[nblock].symbol_dictionary[ascii] = tuple;
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
    if (!fp_cod) return error;;
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
                            read_block(fp_cod, full_seq, &c, nblock++);
                            param = BLOCK_SIZE;
                        }
                    }
                    break;
            }
        }
    }
    return error;
}
void destructor(FullSequence* sequence) {

    for (size_t i = 0; i < sequence->number_blocks; i++) {
        free(sequence->blocks[i].matrix);
        for (int j = 0; j < DICT_SIZE; j++) {
            free(sequence->blocks[i].symbol_dictionary[j].sf_code);
        }
    }
    free(sequence);
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

void print_console(FullSequence* full_seq, char* filename) {
    printf("Matilde Bravo, a93246, MIEI/CD, 1-jan-2021\n");
    printf("Módulo: c (codificação de um ficheiro de símbolos\n");
    printf("Número de blocos: %zu\n", full_seq->number_blocks);
    printf("Ficheiro gerado: %s.shaf\n", filename);
}
void print_matrix(FullSequence * full_seq) {
    for (size_t i = 0; i < full_seq->number_blocks; i++) {
     size_t CODE_MAX_SIZE = (((full_seq->blocks[i].biggest_code_size - 1) | 7) + 9) / 8;
        for (int offset = 0; offset < 8 ; offset++){
            for (int j = 0; j < full_seq->blocks[i].number_symbols ; j++) {
                Piece * pc = get_piece(full_seq->blocks[i].matrix,offset,j,full_seq->blocks[i].number_symbols);
                printf("Bloco : %zu , offset: %d, symbol %d : ",i,offset, j);
                for (size_t i = 0; i < CODE_MAX_SIZE ; i++) {
                    printf("%d ",pc->code[i]);
                }
                putchar('\n');
            }
        }
    }
}

int main() {
    char const* symbol_file = "aaa.txt.rle";
    char* cod_file = malloc(strlen(symbol_file) + 5);  // (".cod" ++ '\0')
    strcpy(cod_file, symbol_file);
    strcat(cod_file, ".cod");  // codfile = "input.txt.rle.cod"
    FullSequence* my_sequence = calloc(sizeof(FullSequence), 1);
    int x = read_cod(cod_file, my_sequence);
    if (x)  printf("Couldn't open file %s",cod_file);
    print_dictionary(my_sequence);
    //print_matrix(my_sequence);
    putchar('\n');
    print_console(my_sequence,"aaa.txt");
    free(cod_file);
    destructor(my_sequence);
    return 0;
}
