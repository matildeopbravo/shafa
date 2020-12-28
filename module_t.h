#ifndef MODULET_H
#define MODULET_H
#include "data.h"
#include "stdio.h"

// Main function for this module. Generates a file containing
// the appropriate Shannon Fano coding for the provided file name.
void generatecode (char *freqs_filename);

// Initializes the new .cod file and gets the block number.
int initialize_code_file (FILE *input, FILE *output);

// Prints the runtime stats in the terminal.
void printStat(int nblocks, int *block_sizes, double time, char *file);

// Writes the size of the next block in the frequency file to the code file,
// including the appending '@' character.
// int write_block_size (FILE *freqs_filename, FILE *code_file);

// Initializes a table containing the data of the 256 symbols in a block
void initialize_table (Symbol *symbol_table, int n);

// Reads frequencies to the pointed Symbol Table from the frequencies file and
// returns the block size.
int read_block (FILE *freqs_file, Symbol *symbol_table);

// Calculates difference between the frequencies of two pointed symbols
int compare_freqs (const void *a, const void *b);

// Calculates difference between the numeric values of the ASCII value of
// two symbols.
int compare_symbolID (const void *a, const void *b);

// Returns the number of symbols in the pointed symbol table with non 0
// frequency
int get_used_symbols (Symbol *symbol_table);

// Writes Shannon Fano code to the symbols in the pointed symbols table
// from the index in start to the index in end, not including the last
// element (if end is 256, it won't calculate the code for the symbol of
// index 256).
void create_shafa_code (Symbol *symbol_table, int start, int end);

// Calculates de index of the first element of the second sub-group in
// the Shannon Fano coding algorithm, splitting the table provided
// from start to end (not including the end index) into 2 sub-groups
// of simillar total frequency at the calculated index.
int freq_split (Symbol *symbol_table, int start, int end);

// Appends a '0' to all symbol codes from start to p, not including p,
// and a '1' to all symbol codes from p to end, not including end. 
void append_bits (Symbol *symbol_table, int p, int start, int end);

// Writes one Shannon Fano code block (and its size) from the code in the
// symbol_table to the file pointed in code_file, including the final '@'
// character.
void write_block (FILE * code_file, int block_size, Symbol *symbol_table);

/* DEBUGGING */
void print_table(Symbol *symbol_table);
#endif /* MODULOT_H */
