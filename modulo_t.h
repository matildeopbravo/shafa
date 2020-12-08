#ifndef MODULOT_H
#define MODULOT_H
#include "data.h"

// Main function for this module. Generates a file containing
// the appropriate Shannon Fano coding for the provided file name.
void generatecode (char *freqs_filename);

// Initializes a table containing the data of the 256 symbols in a block
void initialize_table (symbol *symbol_table, int n);

// Calculates difference between the frequencies of two pointed symbols
int compare_freqs (void *a, void *b);

// Writes Shannon Fano code to the symbols in the pointed symbols table
// from the index in start to the index in end, not including the last
// element (if end is 256, it won't calculate the code for the symbol of
// index 256).
void create_shafa_code (symbol *symbol_table, int start, int end);

// Calculates de index of the first element of the second sub-group in
// the Shannon Fano coding algorithm, splitting the table provided
// from start to end (not including the end index) into 2 sub-groups
// of simillar total frequency at the calculated index.
int freq_split (symbol *symbol_table, int start, int end);

// Appends a '0' to all symbol codes from start to p, not including p,
// and a '1' to all symbol codes from p to end, not including end. 
void append_bits (symbol *symbol_table, int p, int start, int end);

// Appends a character to a string
void append_char(char c, char* string);

#endif /* MODULOT_H */
