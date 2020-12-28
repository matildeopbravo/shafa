#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "dynamic_arrays.h"
#include "data.h"
#include "module_c.h"

Piece* get_piece(
    Piece* matrix,
    uint8_t offset,
    uint8_t symbol_index,
    uint8_t number_symbols) {
    return matrix + number_symbols * offset + symbol_index;
}

int write_block (Block * block, FILE * fp_shaf, FILE * fp_input) {
    ByteVec * vec = byte_vec_new();
    int error = 0;
    int end = 1;
    Piece * piece = get_piece(block->matrix,0,0,block->number_symbols);

    for (size_t symbol = 0; symbol < block->block_size_before ; symbol++) {
        uint8_t index = block->symbol_dictionary[fgetc(fp_input)].index;
        piece += index;
        for (size_t i = 0; i <= piece->byte_index ; i++) {
            if (piece->next == 0 && piece->byte_index == i) {
                break;
            }
            else {
                if (end){
                    byte_vec_push(vec, piece->code[i]);
                }
                else {
                    vec->vec[byte_vec_used(vec)-1] += piece->code[i];
                    end = i == (piece->byte_index - 1);
                }
            }
        }
       end = !piece->next;
       piece = block->matrix + piece->next;
    }
    block->block_size_after = byte_vec_used(vec);
    fprintf(fp_shaf,"@%zu@",byte_vec_used(vec));

   for (size_t i = 0; i < byte_vec_used(vec); i++) {
       fputc(byte_vec_index(vec,i),fp_shaf);
   }
    return error;
}


int write_file (FullSequence * full_seq, char const * shaf_file, char const * input_file) {

    int error = 0;
    FILE * fp_shaf = fopen(shaf_file,"w+b");
    FILE * fp_input = fopen(input_file,"r");
    if (!fp_input || !fp_shaf) return 1;

    fprintf(fp_shaf,"@%zu",full_seq->number_blocks);
    for (size_t i = 0 ; i < full_seq->number_blocks ; i++) {
        write_block(&full_seq->blocks[i], fp_shaf,fp_input);
    }

    fclose(fp_shaf);
    fclose(fp_input);
    return error;
}

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
    for (size_t i = previous->byte_index; ; i--) {
        current->code[i] = (previous->code[i] >> 1);
        if (i == 0) break;
        current->code[i] += (previous->code[i - 1] & 1) << 7;
    }
    current->next = previous->next == 7 * number_symbols ? 0 : previous->next + number_symbols;
    current->byte_index = previous->byte_index + (previous->next == 7 * number_symbols);
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


void start_matrix(Block* block, uint8_t* symbols) {
    Piece* matrix = malloc(sizeof(Piece) * block->number_symbols * 8);
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
            .byte_index = size / 8};
    }
    for (uint8_t i = 1 ; i < 8 ; i++) {
        for (int j = 0 ; j < block->number_symbols; j++){
            (block->matrix + block->number_symbols * i + j)  ->code = malloc(sizeof(uint8_t)*CODE_MAX_SIZE);
        }
    }
}
void matrix_optimization(Block* block, uint8_t* symbols) {
    start_matrix(block, symbols);
    for (int i = 1; i < 8; i++) {
        make_offset(block, i);
    }
}

static void read_block(FILE* fp_cod, FullSequence* full_seq, char* c, int nblock) {
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
    int error = 0;

    FILE* fp_cod = fopen(cod_file, "r");
    if (!fp_cod) return 1;
    char c = fgetc(fp_cod);

    for (int i = 0; not_finished && !error; i++) {
        if (c == '@') {
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
                            full_seq->blocks[nblock].block_size_before = size;
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
        else {
            error = 1;
        }
    }
    fclose(fp_cod);
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
        printf("Bloco: %zu -> Size : %zu\n", i, full_seq->blocks[i].block_size_before);
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

void print_console(FullSequence* full_seq,double time ,char* filename) {
    printf("Matilde Bravo, a93246, MIEI/CD, 1-jan-2021\n");
    printf("Módulo: c (codificação de um ficheiro de símbolos\n");
    printf("Número de blocos: %zu\n", full_seq->number_blocks);
    printf("Tempo de execução do módulo (milissegundos): %f\n",time);
    size_t after_sum = 0;
    size_t before_sum = 0;
    for(size_t i = 0; i < full_seq->number_blocks; i++) {
        size_t before = full_seq->blocks[i].block_size_before;
        size_t after = full_seq->blocks[i].block_size_after;
        after_sum += after;
        before_sum += before;
        printf("Tamanho antes/depois & taxa de compressão (bloco %zu): %zu/%zu \n",i+1,before,after);
    }
    size_t compression = 100 - ((after_sum / (double) before_sum) * 100);
    printf("Taxa de compressão global: %zu%%\n",compression);
    printf("Ficheiro gerado: %s.shaf\n", filename);
}
void print_matrix(FullSequence * full_seq) {
    int z = 0;
    for (size_t i = 0; i < full_seq->number_blocks; i++) {
     size_t CODE_MAX_SIZE = (((full_seq->blocks[i].biggest_code_size - 1) | 7) + 9) / 8;
        for (int offset = 0; offset < 8 ; offset++){
            for (int j = 0; j < full_seq->blocks[i].number_symbols ; z++,j++) {
                Piece * pc = get_piece(full_seq->blocks[i].matrix,offset,j,full_seq->blocks[i].number_symbols);
                printf("%d : Bloco : %zu , offset: %d, symbol: %d, next: %zu * %d  ",z,i,offset, j,pc->next/full_seq->blocks[i].number_symbols, full_seq->blocks[i].number_symbols);
                for (size_t i = 0; i < CODE_MAX_SIZE ; i++) {
                    printf("%d ",pc->code[i]);
                }
                putchar('\n');
            }
        }
    }
}

void module_c(char  * symbol_file) {

    clock_t start = clock();
    char* cod_file = malloc(strlen(symbol_file) + strlen(".cod") + 1 );
    strcpy(cod_file, symbol_file);
    strcat(cod_file, ".cod");  // codfile = "input.txt.rle.cod"
    FullSequence* my_sequence = calloc(sizeof(FullSequence), 1);
    int x = read_cod(cod_file, my_sequence);
    if (x)  printf("Couldn't open file %s",cod_file);
    //print_dictionary(my_sequence);
    //printf("There are %zu blocks\n",my_sequence->number_blocks);
    char * shaf_file = malloc(strlen(symbol_file) + strlen(".shaf") + 1) ;
    strcpy(shaf_file,symbol_file);
    strcat(shaf_file, ".shaf");
    //print_matrix(my_sequence);
    write_file(my_sequence,shaf_file,symbol_file);
    free(cod_file);
    free(shaf_file);
    clock_t end = clock();
    double time = (double) (end - start) / CLOCKS_PER_SEC;
    print_console(my_sequence,time*1000, symbol_file);
    destructor(my_sequence);

}
