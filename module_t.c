#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "module_t.h"
#include "data.h"
#include <time.h>

void generatecode (char *freqs_filename) {
    /* Initialize clock for execution time calculation */
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
        
        int size = read_block (freqs_file, symbol_table);
        qsort(symbol_table, 256, sizeof(Symbol), compare_freqs);
        int symbols = get_used_symbols(symbol_table);
        create_shafa_code(symbol_table, 0, symbols);
        qsort(symbol_table, 256, sizeof(Symbol), compare_symbolID);
        
        write_code_block(code_file, size, symbol_table); 
        
        block_sizes[i == n_blocks - 1] = size;

        int j;
        for (j = 0; j < 256; j++) {
            free(symbol_table[j].code);
        }
        free(symbol_table);
    }
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
    printf ("Módulo: t (cálculo dos códigos dos símbolos)\n");
    printf ("Número de blocos: %d \n", nblocks);
    printf ("Tamanho dos blocos analisados no ficheiro de símbolos: %d", block_sizes[0]);
    if (block_sizes[1])
        printf("/%d", block_sizes[1]);
    printf ("\nTempo de execução do módulo (milissegundos): %g \n", time);
    printf ("Ficheiro gerado: %s \n", file);
}

int initialize_code_file (FILE *input, FILE *output) {
    int i = 0; // Indice do array
    uint8_t x = 0; // Valor que esta no index do array
    int blocks = 0; // Numero de blocos

    /* Copy until the second '@' character, including it */
    while (i++ < 3) {
        x = fgetc(input);
        fputc(x, output);
    }
    
    /* Copy and get total block number */
    x = fgetc(input);
    while (x!='@') {
        blocks = (10 * blocks) + (x - 48);
        fputc(x, output);
        x = fgetc(input);
    }
    
    /* Print final '@' character after block size */
    fputc(x, output);
    return blocks;
}   

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
    prev_freq = size = 0;

    /* Get block size and consume following '@' character */
    char c = fgetc(freqs_file);
    while (c != '@') {
        size = (10 * size) + (c - 48);
        c = fgetc(freqs_file);
    }

    /* Loop for reading all frequency values */
    for (i = 0; i < 256; i++) {
        if (fscanf (freqs_file, "%d", &freq)) // Value read as expected
            symbol_table[i].freq = freq;
        else                                  // Value omitted through ';'
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
    /* Check if dimension is only 2 elements */
    if (end - start == 2)
        append_bits(symbol_table, start + 1, start, end);
    else {
        /* Find the index of the first element of the second subgroup and append
         * the '0' and '1' to the subgroups */
        int p = freq_split(symbol_table, start, end);
        append_bits(symbol_table, p, start, end);
        
        /* Recursively code subgroups for the subgroups we just calculated */
        if (p - start > 1)
            create_shafa_code(symbol_table, start, p);

        if (end - p > 1)
            create_shafa_code(symbol_table, p, end);
    }
}

int freq_split(Symbol *symbol_table, int start, int end) {
    int p, i;
    int freqs[2] = {0, 0};
    int dif_freq, min_dif_freq = -1;
     
    for (p = start + 1; p < end; p++) {
        freqs[0] = freqs[1] = 0;    // Reset frequency additions 
        for (i = start; i < end; i++) 
            freqs[i >= p] += symbol_table[i].freq; // Add frequencies of the two subgroups with p pivot
        
        dif_freq = abs(freqs[0] - freqs[1]);    // Calc diference between frequencies of 2 subgroups
        
        if (dif_freq < min_dif_freq || min_dif_freq == -1) // Frequency dif still decreasing/uninitialized, continue looping
            min_dif_freq = dif_freq;
        else break; // Frequency dif worsened, best pivot already found
    }
    return (p-1);
}

void append_bits (Symbol *symbol_table, int p, int start, int end) {
    int i;
    char bit_char[2];
    for (i = start; i < end; i++) {
        bit_char[0] = (i >= p) + 48; // '0' if in first subgroup, '1' otherwise
        bit_char[1] = 0;             // Null character so as to use strcat sucessfully 
        strcat(symbol_table[i].code, bit_char);
    }
}

void write_code_block (FILE * code_file, int block_size, Symbol *symbol_table) {
    int i;
    /* Print block size and appending '@' character */
    fprintf (code_file, "%d", block_size);
    fputc('@', code_file);


    for (i = 0; i < 256; i++) {
        if (symbol_table[i].code)   // Character has a code, so we print it
            fprintf(code_file, "%s", symbol_table[i].code);
        fputc(';', code_file);
    }
    
    /* Replace last ';' with a '@' */
    fseek(code_file, -1, SEEK_CUR);
    fputc('@', code_file);
}
