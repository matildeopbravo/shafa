#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data.h"
#include "module_t.h"
#include "dynamic_arrays.h"

void generatecode (char *freqs_filename) {
    // Read frequencies files
    // FILE *freqs_file = fopen(freqs_filename, "r");


    // Create Shannon Fano code file
    // char *output_name = ;
    // FILE *code_file = fopen(output_name, "w");
    // n_blocks = initialize_code_file(code_file);      //Initialize cod file
                                                        //and get n_blocks    


    /* Main loop for each block */
    int i;
    for (i = 0; i < n_blocks; i++) {
        write_block_size (freqs_file, code_file);

        Symbol *symbol_table = (Symbol*) malloc(256*sizeof(Symbol));

        initialize_table (symbol_table, 256);
        
        read_block (freqs_file, symbol_table);

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

int initialize_code_file (FILE *input, FILE *output) {
      int i = 0; // indice do array
      uint8_t x = 0; // valor que esta no index do array
      int nb = 0; // numero de blocos
      int a = 0; // contador de arrobas
      ByteVec *vector = loadArray(input, M); 
      while (a < 2 && i < vector->used) {
        x = byte_vec_index(vector, i);
          if (x == '@' && a<2) {
            a++;
            fputc(x, output);
            i++;
            x = byte_vec_index(vector, i);
            nb = 0;
            while (x!='@') {
              nb = (10*nb) + (x-48);
              fputc(x, output);
              i++;
              x = byte_vec_index(vector, i);
            }
          }
      }
      return nb;
}   

void write_block_size (FILE *freqs_file, FILE *code_file) {
    int block_size;
    fscanf (freqs_file, "%d", &block_size);
    fprintf (code_file, "%d", block_size);
    fputc('@', code_file);
}

void initialize_table (Symbol *symbol_table, int n) {
    int i;
    for (i = 0; i < n; i++) {
        symbol_table[i].symbolID = i;
        symbol_table[i].code = (char *) malloc(256);
    }
}

void read_block (FILE *freqs_file, Symbol *symbol_table) {
    int i, freq, prev_freq;
    for (i = 0; i < 256; i++) {
        if (fscanf (freqs_file, "%d", &freq))
            symbol_table[i].freq = freq;
        else
            symbol_table[i].freq = prev_freq;
        prev_freq = freq;
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

void write_block_code (FILE * code_file, int block_size, Symbol *symbol_table) {
    int i;
    for (i = 0; i < 256; i++) {
        if (symbol_table[i].code)
            fprintf(code_file, "%s", symbol_table[i].code);
        fputc(';', code_file);
    }

    fseek(code_file, -1, SEEK_CUR);
    fputc('@', code_file);
}
