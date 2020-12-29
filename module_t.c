#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data.h"
#include "module_t.h"
#include "dynamic_arrays.h"
#include <time.h>
#define M 64000000

int main(int argc, char *argv[]) {
    if (!argc) {
        printf("Please input file name.\n");
        return 0;
    }
    else {
        generatecode(argv[1]);
        return 1;
    }
}

void generatecode (char *freqs_filename) {
    /* Initialize clock for run time calculation */
    clock_t Ticks[2];
    Ticks[0] = clock();

    /* Read frequencies file and open code file */
    FILE *freqs_file = fopen(freqs_filename, "r");
    FILE *code_file;
    char *codfilename = replace_str(freqs_filename, "freq", "cod");
    code_file = fopen(codfilename, "w");

    /* Initialize code file and get total number of blocks */
    int n_blocks = initialize_code_file(freqs_file, code_file);

    /* Main loop for code generation */
    int i;
    int block_sizes[2] = {0,0};

    for (i = 0; i < n_blocks; i++) {
        Symbol *symbol_table = (Symbol*) malloc(256*sizeof(Symbol));
        initialize_table (symbol_table, 256);
        
        printf("Initialized symbol table %d.\n", i);

        int size = read_block (freqs_file, symbol_table);
        printf("Read the block!\n");
        qsort(symbol_table, 256, sizeof(Symbol), compare_freqs);
        printf("Sorted frequencies and...\n");
        int symbols = get_used_symbols(symbol_table);
        create_shafa_code(symbol_table, 0, symbols);
        printf("generated the code and finally...\n");
        qsort(symbol_table, 256, sizeof(Symbol), compare_symbolID);
        printf("sorted symbol IDs!\n");
        
        write_block(code_file, size, symbol_table);
        free(symbol_table);
        
        block_sizes[i == n_blocks - 1] = size;

        printf("Finished writing block\n");
    }
    printf("----------\nDone!\n----------\n");
    /* Append null block '0' character */
    fputc('0', code_file);

    /* Get run time */
    Ticks[1] = clock();
    double time = (Ticks[1] - Ticks[0]) * 1000.0 / CLOCKS_PER_SEC;

    /* Print stats */
    printStat(n_blocks, block_sizes, time, codfilename); // falta definir o tamanho do bloco

    /* Close files */
    fclose(freqs_file);
    fclose(code_file);
}

void printStat(int nblocks, int *block_sizes, double time, char *file) {
    printf ("Alexandre Flores, a93220, Rita Lino, a93196, MIEI/CD, 5-jan-2021 \n");
    printf ("Módulo: t (cálculo dos códigos dos símbolos\n");
    printf ("Número de blocos: %d \n", nblocks);
    printf ("Tamanho dos blocos analisados no ficheiro de símbolos: %d", block_sizes[0]);
    if (block_sizes[1])
        printf("/%d", block_sizes[1]);
    printf ("\nTempo de execução do módulo (milissegundos): %g \n", time);
    printf ("Ficheiro gerado: %s \n", file);
}

int initialize_code_file (FILE *input, FILE *output) {
    int i = 0; // indice do array
    uint8_t x = 0; // valor que esta no index do array
    int blocks = 0; // numero de blocos
    // int ats = 0; // contador de arrobas
    //ByteVec *vector = loadArray(input, M); 
    /* while (ats < 2 && i < vector->used ) {
        //x = byte_vec_index(vector, i);
        x = fgetc (input);
            if (x == '@' && ats < 2) {
                ats++;
                fputc(x, output);
                // i++;
                // x = byte_vec_index(vector, i);
                x = fgetc (input);
                blocks = 0;
                while (x!='@') {
                    blocks = (10*blocks) + (x-48);
                    fputc(x, output);
                    // i++;
                    // x = byte_vec_index(vector, i);
                    x = fgetc(input);
                }
            }
    } */
    while (i++ < 3) {
        x = fgetc(input);
        fputc(x, output);
    }
    
    x = fgetc(input);
    while (x!='@') {
        blocks = (10 * blocks) + (x - 48);
        fputc(x, output);
        x = fgetc(input);
    }

    fputc(x, output);

    printf ("Got %d blocks.\n", blocks);
    return blocks;
}   

/* int write_block_size (FILE *freqs_file, FILE *code_file) {
    int block_size;
    fscanf (freqs_file, "%d", &block_size);
    fprintf (code_file, "%d", block_size);
    fputc('@', code_file);

    return block_size;
} */

void initialize_table (Symbol *symbol_table, int n) {
    int i;
    for (i = 0; i < n; i++) {
        symbol_table[i].symbolID = i;
        symbol_table[i].code = (char *) malloc(256);
        symbol_table[i].code[0] = 0;
    }
}

int read_block (FILE *freqs_file, Symbol *symbol_table) {
    int i, freq, prev_freq, size;
    fscanf (freqs_file, "%d", &size);
    printf("Finished reading block_size (%d) in read_block.\n", size);
    fgetc(freqs_file);
    for (i = 0; i < 256; i++) {
        if (fscanf (freqs_file, "%d", &freq))
            symbol_table[i].freq = freq;
        else
            symbol_table[i].freq = prev_freq;
        prev_freq = freq;
        fgetc (freqs_file);
    }

    return size;
}

int compare_freqs (const void *a, const void *b) {
    return (((Symbol *) b)->freq - ((Symbol *) a)->freq);
}

int compare_symbolID (const void *a, const void *b) {
    return (((Symbol *) a)->symbolID - ((Symbol *) b)->symbolID);
}

int get_used_symbols (Symbol *symbol_table) {
    int i = 0;
    while (symbol_table[i++].freq);
    return (i-1);
}

void create_shafa_code (Symbol *symbol_table, int start, int end) { 
    int p = freq_split(symbol_table, start, end);
    append_bits(symbol_table, p, start, end);

    if (p - start > 1)
        create_shafa_code(symbol_table, start, p);
    if (end - p > 1)
        create_shafa_code(symbol_table, p, end);
}

int freq_split(Symbol *symbol_table, int start, int end) {
    int p, i;
    int freqs[2] = {0, 0};
    int dif_freq, min_dif_freq = -1;
     
    for (p = start + 1; p < end; p++) {
        freqs[0] = freqs[1] = 0;
        for (i = start; i < end; i++) 
            freqs[i >= p] += symbol_table[i].freq;
        
        dif_freq = abs(freqs[0] - freqs[1]);
        printf("%d; ", dif_freq);
        
        if (dif_freq < min_dif_freq || min_dif_freq == -1)
            min_dif_freq = dif_freq;
        else break;
    }
    printf("Found this pivot from %d to %d: %d\n", start, end, p-1);
    return (p-1);
}

void append_bits (Symbol *symbol_table, int p, int start, int end) {
    int i;
    char bit_char[2];
    for (i = start; i < end; i++) {
        bit_char[0] = (i >= p) + 48;
        bit_char[1] = 0;
        strcat(symbol_table[i].code, bit_char);
    }
}

void write_block (FILE * code_file, int block_size, Symbol *symbol_table) {
    int i;
    fprintf (code_file, "%d", block_size);
    fputc('@', code_file);

    for (i = 0; i < 256; i++) {
        if (symbol_table[i].code)
            fprintf(code_file, "%s", symbol_table[i].code);
        fputc(';', code_file);
    }

    fseek(code_file, -1, SEEK_CUR);
    fputc('@', code_file);
}

/* DEBUGGING */

void print_table (Symbol *symbol_table) {
    for (int i = 0; i < 256; i++) {
        printf("%d    ", symbol_table[i].symbolID);
        printf("%d    ", symbol_table[i].freq);
        printf("%s\n", symbol_table[i].code);
    }
}
