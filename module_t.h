#ifndef MODULET_H
#define MODULET_H
#include "stdio.h"

/* Struct for symbol data */
typedef struct symbol {
  int symbolID;
  int freq;
  char *code;
} Symbol;

/**
 \brief Main function for this module. Generates a file containing the
 appropriate Shannon Fano coding for the provided file name.
 \param *char Frequency file's name.
 */
void generatecode (char *freqs_filename);

/**
 \brief Initializes the new .cod file and gets the total block number.
 \param *FILE Frequency file.
 \param *FILE Output code file.
 \return Number of blocks in the frequency file.
 */
int initialize_code_file (FILE *input, FILE *output);

/**
 \brief Prints the runtime stats in the terminal.
 \param int Number of blocks in the frequency file.
 \param *int Array of two elements containing the two possible block sizes.
 \param double Execution time of program.
 \param *char Output file name.
 */
void printStat(int nblocks, int *block_sizes, double time, char *file);

/**
 \brief Initializes a table containing the data of the 256 symbols in a block
 \param *Symbol An array of Symbols that represents our table of symbols to be
 initialized.
 \param int Number of symbols to initialize in the table (usually 256).
 */
void initialize_table (Symbol *symbol_table, int n);

/**
 \brief Reads frequencies to the pointed Symbol Table from the frequencies file
 and returns the block size.
 \param *FILE Frequency file.
 \param *Symbol Table of symbols to write frequencies on.
 \return Size of block being read.
 */
int read_block (FILE *freqs_file, Symbol *symbol_table);

/**
 \brief Calculates difference between the frequencies of two pointed symbols.
 This function is used later as the comparison function in a qsort usage.
 \param *const void First symbol being compared.
 \param *const void Second symbol being compared.
 \return Diference between the frequencies of the two symbols.
 */
int compare_freqs (const void *a, const void *b);

/**
 \brief Calculates difference between the numeric values of the ASCII value of
 two symbols. This function is used later as the comparison function in a qsort usage.
 \param *const void First symbol being compared.
 \param *const void Second symbol being compared.
 \return Diference between the symbol IDs of the two symbols.
 */
int compare_symbolID (const void *a, const void *b);

/**
 \brief Returns the number of symbols in the pointed symbol table with non 0
 frequency
 \param *Symbol Table of symbols being read for null frequencies.
 \return Total number of symbols with non 0 frequency in table.
 */
int get_used_symbols (Symbol *symbol_table);

/**
 \brief Writes Shannon Fano code to the symbols in the pointed symbols table
 from the index in start to the index in end, not including the last
 element (if end is 256, it won't calculate the code for the symbol of
 index 256).
 \param *Symbol Table containing all symbols in the block being coded that will
 be written on.
 \param int Index of first element of current group in the Shannon Fano algorithm.
 \param int Index of the first garbage element, not to be read/worked on.
 */
void create_shafa_code (Symbol *symbol_table, int start, int end);

/**
 \brief Calculates de index of the first element of the second sub-group in
 the Shannon Fano coding algorithm, splitting the table provided
 from start to end (not including the end index) into 2 sub-groups
 of simillar total frequency at the calculated index.
 \param *Symbol Table containing all symbols in the block being coded that will
 be read from.
 \param int Index of first element of current group in the Shannon Fano algorithm.
 \param int Index of the first garbage element, not to be read/worked on.
 \return Index of first element in the second subgroup we just calculated.
 */
int freq_split (Symbol *symbol_table, int start, int end);

/**
 \brief Appends a '0' to all symbol codes from start to p, not including p,
 and a '1' to all symbol codes from p to end, not including end. 
 \param *Symbol Table containing all symbols in the block being coded that
 will be written on.
 \param int Index of first element of second subgroup, in which we append '1'.
 \param int Index of first element of first subgroup, in which we append '0'.
 \param int Index of first garbage element, not to be written on. 
 */
void append_bits (Symbol *symbol_table, int p, int start, int end);

/**
 \brief Writes one Shannon Fano code block (and its size) from the code in the
 symbol_table to the file pointed in code_file, including the final '@'
 character.
 \param *FILE Output code file.
 \param int Size of block being written.
 \param *Symbol Table containing all symbols in block being written, from which
 we read the code recently calculated.
 */
void write_code_block (FILE * code_file, int block_size, Symbol *symbol_table);

#endif /* MODULOT_H */
