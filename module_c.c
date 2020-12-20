#include "module_c.h"
/* #include "fsize.h" */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int count_numbers(char* c, FILE* fp_cod) {
    char tmp = *c;
    char buffer[20];  // to be determined se usamos array dinamico
    int i = 0;

    do {
        buffer[i] = tmp;
        i++;
    } while ((tmp = fgetc(fp_cod)) != '@');

    buffer[i] = '\0';
    *c = tmp;
    return atoi(buffer);
}

size_t strToNum(const char* str, size_t size) {
    size_t num = 0;
    for (int i = size - 1; str[i]; i--) {
        if (str[i] == '1') {
            num += str[i] << (size - 1 - i);
        }
    }
    return num;
}

void print_dictionary(FullSequence* full_seq) {

    for (size_t i = 0; i < full_seq->number_blocks; i++) {
        printf(
            "Bloco : %zu -> Size : %zu\n", i, full_seq->blocks[i].block_size);
        for (int x = 0; x < DIC_SIZE; x++) {
            if (full_seq->blocks[i].symbol_dictionary[x]) {
                printf("<%s>", full_seq->blocks[i].symbol_dictionary[x]);
            }
            else {
                putchar('.');
            }
        }
        putchar('\n');
    }
}

int read_cod(char* cod_file, FullSequence* full_seq) {
    char* sequence;
    enum parameters param = START;
    int nblock = 0, error = 1, not_finished = 1;
    size_t size = 0;
    // este valor será alterado para 0 quando encontrarmos
    // @0 e o ciclo para

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
                    //                     // O c já está igual ao primeiro
                    //                     digito
                    //                     // a count numbers vai deixar o c
                    //                     = @ e é para onde o file
                    //                     // pointer aponta e vai devolver
                    //                     o valor correspondente ao
                    //                     // numero de blocos (já em long)
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
                            for (int ascii = 0; c != '@';) {
                                if (c != ';') {
                                    sequence = malloc(sizeof(char) * 5);
                                    int x = 0;
                                    for (; c != ';'; x++) {
                                        sequence[x] = c;
                                        c = fgetc(fp_cod);
                                    }
                                    sequence[x] = '\0';
                                    full_seq->blocks[nblock]
                                        .symbol_dictionary[ascii] = sequence;
                                }
                                else {

                                    c = fgetc(fp_cod);
                                    ascii++;
                                }
                            }
                            param = BLOCK_SIZE;
                            nblock++;
                        }
                    }
                    break;
            }
        }
    }
    return error;
}

int main() {
    // input file, in this example it's rle but could be just txt
    // char const* symbol_file = "aaa.txt";
    // char* cod_file = malloc(strlen(symbol_file) + 5);  // (".cod" ++
    // '\0')
    FullSequence* my_sequence = calloc(sizeof(FullSequence), 1);
    // strcpy(cod_file, symbol_file);
    // strcat(cod_file, ".cod");  // codfile = "input.txt.rle.cod"
    read_cod("bbb.txt.cod", my_sequence);
    print_dictionary(my_sequence);

    for (size_t i = 0; i < my_sequence->number_blocks; i++) {
        for (int j = 0; j < 256; j++) {
            free(my_sequence->blocks[i].symbol_dictionary[j]);
        }
    }
    free(my_sequence);

    return 0;
}
