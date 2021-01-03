/**
 * @file module_c.c
 * @author Matilde Oliveira Pizarro Bravo
 * @date 3 Janeiro 2021
 * @brief Responsável por gerar um ficheiro comprimido (shaf), dado um ficheiro de texto e um ficheiro com os códigos Shannon-Fano.
 */

#include "module_c.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "dynamic_arrays.h"
/**
\brief Devolve uma peça(code+index+next) da matrz de bytes
*/
static Piece* get_piece(
    Piece* matrix,
    uint8_t offset,
    uint8_t symbol_index,
    uint16_t number_symbols) {
    return matrix + number_symbols * offset + symbol_index;
}
/**
\brief Escreve o resultado da compressão de um bloco no ficheiro shaf
*/
static int write_block(Block* block, FILE* fp_shaf, FILE* fp_input) {
    ByteVec* vec = byte_vec_new();
    int error = 0;
    int end = 1;
    Piece* piece = get_piece(block->matrix, 0, 0, block->number_symbols);
    for (size_t symbol = 0; symbol < block->block_size_before; symbol++) {
        int c = fgetc(fp_input);
        uint8_t index = block->symbol_dictionary[c].index;
        piece += index;
        for (size_t i = 0; i <= piece->byte_index; i++) {
            if (piece->next == 0 && piece->byte_index == i) {
                break;
            }
            else {
                if (end) {
                    byte_vec_push(vec, piece->code[i]);
                }
                else {
                    vec->vec[byte_vec_used(vec) - 1] += piece->code[i];
                    end = i < (piece->byte_index);
                }
            }
        }
        end = !piece->next;
        piece = block->matrix + piece->next;
    }
    block->block_size_after = byte_vec_used(vec);
    fprintf(fp_shaf, "@%zu@", byte_vec_used(vec));
    for (size_t i = 0; i < byte_vec_used(vec); i++) {
        fputc(byte_vec_index(vec, i), fp_shaf);
    }
    byte_vec_del(vec);
    return error;
}
/**
\brief Escreve o resultado da compressão de todos os blocos no ficheiro shaf, bem como informação sobre o número de blocos e o seu tamanho.
*/
static int write_file(
    FullSequence* full_seq, char const* shaf_file, char const* input_file) {

    int error = 0;
    FILE* fp_shaf = fopen(shaf_file, "w+b");
    FILE* fp_input = fopen(input_file, "r");
    if (!fp_input || !fp_shaf) return 1;

    fprintf(fp_shaf, "@%zu", full_seq->number_blocks);
    for (size_t i = 0; i < full_seq->number_blocks; i++) {
        write_block(&full_seq->blocks[i], fp_shaf, fp_input);
    }

    fclose(fp_shaf);
    fclose(fp_input);
    return error;
}
/**
\brief Lê carateres do ficheiro cod e converte os carateres lidos para o número correspondente.
*/

static size_t count_numbers(char* c, FILE* fp_cod) {
    char tmp = *c;
    CharVec buffer = char_vec_new();
    do {
        char_vec_push(&buffer, tmp);

    } while ((tmp = fgetc(fp_cod)) != '@');

    char_vec_push(&buffer, '\0');
    *c = tmp;
    int n = atoi(buffer.vec);
    char_vec_del(&buffer);
    return n;
}
/**
\brief Cria uma nova peça a partir da peça correspondente do offset anterior. Faz os shifts necessários no code da peça nova.
*/

static void shift_piece(Piece* previous, Piece* current, uint16_t number_symbols) {
    for (size_t i = previous->byte_index;; i--) {
        current->code[i] = (previous->code[i] >> 1);
        if (i == 0) break;
        current->code[i] += (previous->code[i - 1] & 1) << 7;
    }
    current->next = previous->next == 7 * number_symbols
                        ? 0
                        : previous->next + number_symbols;
    current->byte_index =
        previous->byte_index + (previous->next == 7 * number_symbols);
}
/**
\brief Cria um offset, do 1 ao 7, da matriz de bytes
*/

static void make_offset(Block* block, int offset) {
    Piece* pieces_current = block->matrix + block->number_symbols * offset;
    Piece* pieces_previous =
        block->matrix + block->number_symbols * (offset - 1);
    uint16_t number_symbols = block->number_symbols;

    for (uint16_t i = 0; i < block->number_symbols; i++) {
        shift_piece(pieces_previous + i, pieces_current + i, number_symbols);
    }
}
/**
\brief Cria um code, array de inteiros de 8 bits, com tamanho máximo CODE_MAX_SIZE*/

static uint8_t* make_code(const char* str, size_t size, size_t CODE_MAX_SIZE) {

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
/**
\brief Inicializa a matriz de bytes e cria o primeiro offset e as respetivas peças.
*/
static void start_matrix(Block* block, uint8_t* symbols) {
    Piece* matrix = calloc(sizeof(Piece), block->number_symbols * 8);
    block->matrix = matrix;
    size_t CODE_MAX_SIZE = (((block->biggest_code_size - 1) | 7) + 9) / 8;

    // creates offset 0
    for (uint16_t i = 0; i < block->number_symbols; i++) {
        SFTuple tuple =
            block->symbol_dictionary[symbols[i]];  // symbols has value of
                                                   // existing symbols
        size_t size = strlen(tuple.sf_code);
        block->matrix[tuple.index] = (Piece){
            .code = make_code(tuple.sf_code, size, CODE_MAX_SIZE),
            .next = (size % 8) * (size_t) block->number_symbols,
            .byte_index = size / 8};
    }
    for (uint8_t i = 1; i < 8; i++) {
        for (uint16_t j = 0; j < block->number_symbols; j++) {
            (block->matrix + block->number_symbols * i + j)->code =
                calloc(sizeof(uint8_t),  CODE_MAX_SIZE);
        }
    }
}
/**
\brief Cria a matriz de bytes que será utilizada para otimizar a compressão.
*/
static void matrix_optimization(Block* block, uint8_t* symbols) {
    start_matrix(block, symbols);
    for (int i = 1; i < 8; i++) {
        make_offset(block, i);
    }
}
/**
\brief Lê um bloco do ficheiro cod, imediatamente a seguir a ter sido guardado o seu tamanho.
*/

static void read_block(
    FILE* fp_cod, FullSequence* full_seq, char* c, size_t nblock) {
    uint16_t index = 0;
    uint8_t symbols[DICT_SIZE];
    size_t max_size = 0;

    for (uint8_t ascii = 0; *c != '@';) {
        if (*c != ';') {
            CharVec sequence = char_vec_new();
            while (*c != ';' && *c != '@') {
                char_vec_push(&sequence,*c);
                *c = fgetc(fp_cod);
            }
            char_vec_push(&sequence,'\0');
            if (sequence.used - 1 > max_size) {
                max_size = sequence.used - 1;
            }
            symbols[index] = ascii;
            SFTuple tuple = (SFTuple){.sf_code = sequence.vec, .index = index++};
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
/**
\brief Lê o ficheiro cod e guarda a informação relativa a cada um dos blocos nas respetivas estruturaas de dados.
*/


static int read_cod(char* cod_file, FullSequence* full_seq) {
    size_t nblock = 0 , size = 0;
    int error = 0;
    FILE* fp_cod = fopen(cod_file, "r");
    if (!fp_cod) return 1;
    fseek(fp_cod,3,SEEK_SET); // jumps to number o of  blocks
    char c = fgetc(fp_cod);
    full_seq->number_blocks = count_numbers(&c, fp_cod);
    full_seq->blocks = calloc(sizeof(Block),full_seq->number_blocks);
    enum parameters param = BLOCK_SIZE;

    for (size_t i = 0; nblock < full_seq->number_blocks && !error; i++) {
        if (c == '@') {
                c = fgetc(fp_cod);
                if (param == BLOCK_SIZE) {
                        size = count_numbers( &c, fp_cod);  // o c já tem o primeiro digito
                        full_seq->blocks[nblock].block_size_before = size;
                        param = SEQUENCE;
                    }
                 else {  // param == sequence
                      read_block(fp_cod, full_seq, &c, nblock++);
                      param = BLOCK_SIZE;
                 }
        }
        else {
            error = 1;
        }
    }
    fclose(fp_cod);
    return error;
}
/**
\brief Liberta toda a memória que o programa alocou na Heap.
*/

static void destructor(FullSequence* sequence,char*shaf_file,char*cod_file) {
    for (size_t i = 0; i < sequence->number_blocks; i++) {
        for (size_t j = 0; j < (sequence->blocks[i].number_symbols) * 8; j++) {
            free((sequence->blocks[i].matrix + j)->code);
        }
        free(sequence->blocks[i].matrix);
        for (size_t x = 0; x < DICT_SIZE; x++) {
            free(sequence->blocks[i].symbol_dictionary[x].sf_code);
        }
    }
    free(sequence->blocks);
    free(sequence);
    free(cod_file);
    free(shaf_file);
}
/**
\brief Função de debugging que imprime os valores guardados na estrutura dicionário.
*/
static void print_dictionary(FullSequence* full_seq) {
    for (size_t i = 0; i < full_seq->number_blocks; i++) {
        printf(
            "Bloco: %zu -> Size : %zu\n",
            i,
            full_seq->blocks[i].block_size_before);
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
/**
\brief Envia para o stdout informação relevante relativamente à execução do programa.
*/
static void print_console(FullSequence* full_seq, double time, char* filename) {
    printf("Matilde Bravo, a93246, MIEI/CD, 1-jan-2021\n");
    printf("Módulo: c (codificação de um ficheiro de símbolos)\n");
    printf("Número de blocos: %zu\n", full_seq->number_blocks);
    printf("Tempo de execução do módulo (milissegundos): %f\n", time);
    size_t after_sum = 0;
    size_t before_sum = 0;
    for (size_t i = 0; i < full_seq->number_blocks; i++) {
        size_t before = full_seq->blocks[i].block_size_before;
        size_t after = full_seq->blocks[i].block_size_after;
        after_sum += after;
        before_sum += before;
        printf("Tamanho antes/depois & taxa de compressão (bloco %zu): %zu/%zu \n",
            i + 1,
            before,
            after);
    }
    size_t compression = 100 - ((after_sum / (double) before_sum) * 100);
    printf("Taxa de compressão global: %zu%%\n", compression);
    printf("Ficheiro gerado: %s.shaf\n", filename);
}
/**
\brief Função de debugging que imprime no ecrã o conteúdo da matriz de bytes utilizada na otimização.
*/

static void print_matrix(FullSequence* full_seq) {
    int z = 0;
    for (size_t i = 0; i < full_seq->number_blocks; i++) {
        size_t CODE_MAX_SIZE =
            (((full_seq->blocks[i].biggest_code_size - 1) | 7) + 9) / 8;
        for (int offset = 0; offset < 8; offset++) {
            for (int j = 0; j < full_seq->blocks[i].number_symbols; z++, j++) {
                Piece* pc = get_piece(
                    full_seq->blocks[i].matrix,
                    offset,
                    j,
                    full_seq->blocks[i].number_symbols);
                printf(
                    "%d : Bloco : %zu , offset: %d, symbol: %d, next: %zu * "
                    "%d ",
                    z,
                    i,
                    offset,
                    j,
                    pc->next / full_seq->blocks[i].number_symbols,
                    full_seq->blocks[i].number_symbols);
                for (size_t i = 0; i < CODE_MAX_SIZE; i++) {
                    printf("%d ", pc->code[i]);
                }
                putchar('\n');
            }
        }
    }
}
/**
\brief Devolve um apontador para um array de chars que contem o nome do ficheiro
*/
static char * get_filename(char*symbol_file,char * extension) {
    char* cod_file = malloc(strlen(symbol_file) + strlen(extension) + 1);
    strcpy(cod_file, symbol_file);
    strcat(cod_file, extension);
    return cod_file;
}
/**
\brief Função principal do módulo c cujo objetivo é converter um ficheiro de texto para um ficheiro shaf comprimido.
*/

void module_c(char* symbol_file) {

    char * shaf_file = get_filename(symbol_file,".shaf");
    char * cod_file = get_filename(symbol_file,".cod");
    clock_t start = clock();
    FullSequence* my_sequence = calloc(sizeof(FullSequence), 1);
    int x = read_cod(cod_file, my_sequence);
    if (x) printf("Couldn't open file %s", cod_file);
    write_file(my_sequence, shaf_file, symbol_file);
    print_console(my_sequence, ((double) (clock() - start) / CLOCKS_PER_SEC) * 1000, symbol_file);
    destructor(my_sequence,cod_file,shaf_file);
}
