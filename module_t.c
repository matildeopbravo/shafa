#include "data.h"
#include "module_t.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

void generatecode (char *freqs_filename) {
    // Read frequencies files
    // FILE *freqs_file = fopen(freqs_filename, "r");

    // Create Shannon Fano code file
    // char *output_name = ;
    // FILE *code_file = fopen(output_name, "w");
    // initialize_code_file(code_file);

    // Obtain number of blocks in file
    // int n_blocks = getblocks(freqs_file);

    /* Main loop for each block */
    int i;
    for (i = 0; i < n_blocks; i++) {
        Symbol *symbol_table = (Symbol*) malloc(256*sizeof(Symbol));

        initialize_table (symbol_table, 256);

        qsort(symbol_table, 256, sizeof(Symbol), compare_freqs);

        create_shafa_code(symbol_table, 0, 256);
        
        qsort(symbol_table, 256, sizeof(Symbol), compare_symbolID);

        //int block_size = get_block_size(freqs_file);
        write_block(code_file, block_size, symbol_table);
        free(symbol_table);
    }
   
    // Append the null block at the end of the loop
    //
    // Print run stats
}

void initialize_table (Symbol *symbol_table, int n) {
    int i;
    for (i = 0; i < n; i++) {
        symbol_table[i].symbolID = i;
        symbol_table[i].code = (char *) malloc(256);
    }
}

int compare_freqs (void *a, void *b) {
    return (((Symbol *) b)->freq - ((Symbol *) a)->freq);
}

int compare_symbolID (void *a, void*b) {
    return (((Symbol *) a)->symbolID - ((Symbol *) b)->symbolID);
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
    int p, i, found_pivot = 0;
    int freqs[2] = {0, 0};
    int dif_freq, min_dif_freq = -1;
     
    for (p = start + 1; !found_pivot && p < end; p++) {
        for (i = start; i < end; i++) 
            freqs[i >= p] += symbol_table[i].freq;
        
        dif_freq = abs(freqs[0] - freqs[1]);
        
        if (dif_freq < min_dif_freq || min_dif_freq == -1)
            min_dif_freq = dif_freq;
        else found_pivot = 1;
    }

    return (p-1);
}

void append_bits (Symbol *symbol_table, int p, int start, int end) {
    int i;
    char *bit_char;
    for (i = start; i < end; i++) {
        *bit_char = (i >= p) + 48;
        strcat(symbol_table[i].code, bit_char);
    }
}

void write_block (FILE * code_file, int block_size, Symbol *symbol_table) {
    int i;
    for (i = 0; i < 256; i++) {
        if (symbol_table[i].code)
            fprintf(code_file, "%s", symbol_table[i].code);
        fputc(';', code_file);
    }

    fseek(code_file, -1, SEEK_CUR);
    fputc('@', code_file);
}
