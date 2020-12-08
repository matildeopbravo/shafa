#include "data.h"
#include "module_t.h"
#include "stdio.h"
#include "stdlib.h"

void generatecode (char *freqs_filename) {
    // Read frequencies files
    // FILE *freqs_file = fopen(freqs_filename, "r");

    // Create Shannon Fano code file
    // char *output_name = ;
    // FILE *code_file = fopen(output_name, "w");

    // Obtain number of blocks in file
    // int n_blocks = getblocks(freqs_file);

    /* Main loop for each block */
    int i;
    for (i = 0; i < n_blocks; i++) {
        Symbol *symbol_table = (Symbol*) malloc(256*sizeof(Symbol));

        initialize_table (symbol_table, 256);

<<<<<<< HEAD
        qsort(symbol_table, 256, sizeof(Symbol), compare_freqs);

        create_shafa_code(symbol_table, 0, 256);
        
        write_block(code_file, symbol_table);
        free(symbol_table);
    }
   
=======
    //     createshafacode(symbol_table);
    //
    //     writeblock(code_file, symbol_table);
    //     free(symbol_table);
    // }
    //
>>>>>>> 9f65a5d8edbffdb025d18365ea328f044d460291
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

void create_shafa_code (Symbol *symbol_table, int start, int end) { 
    int p = freq_split(symbol_table, start, end);
    append_bits(symbol_table, p, start, end);

    create_shafa_code(symbol_table, start, p);
    create_shafa_code(symbol_table, p, end);
}

int freq_split(Symbol *symbol_table, int start, int end) {
    int p, i;
    int freq_1 = 0;
    int freq_2 = 0;
    int dif_freq, min_dif_freq = -1; 
     
    for (p = start + 1; p < end; p++) {
        for (i = start; i < end; i++) {
            if (i < p) {
                freq_1 += symbol_table[i].freq;
            }
            else {
                freq_2 += symbol_table[i].freq;
            }
        }
        
        dif_freq = abs(freq_1 - freq_2);
        
        if (min_dif_freq == -1)
            min_dif_freq = dif_freq;

        else {
            if (dif_freq < min_dif_freq)
                min_dif_freq = dif_freq;
            else {
                break;
            }
        }
    }

    return (p-1);
}

void append_bits (Symbol *symbol_table, int p, int start, int end) {
    int i;
    for (i = start; i < end; i++) {
        if (i < p)
            append_char('0', symbol_table[i].code);
        else
            append_char('1', symbol_table[i].code);
    }
}

void append_char(char c, char* string) {
    int i;
    while (string[i])
        i++;
    string[i] = c;
}








