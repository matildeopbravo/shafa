#include "module_c.h"
/* #include "fsize.h" */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int count_numbers(char *c, FILE *fp_cod) {

  char tmp = *c;
  char buffer[20]; // to be determined se usamos array dinamico
  int i = 0;

  do {
    buffer[i] = tmp;
    i++;
  } while ((tmp = fgetc(fp_cod)) != '@');

  buffer[i] = '\0';
  *c = tmp;
  return atoi(buffer);
}

size_t strToNum(const char *str, size_t size) {
  size_t num = 0;
  for (int i = size - 1; str[i]; i--) {
    if (str[i] == '1') {
      num += str[i] << (size - 1 - i);
    }
  }
  return num;
}

void read_cod(char *cod_file, FullSequence *full_seq) {

  char sequence[uint8_max]; // should it be uint8 ??
  enum parameters param = START;
  int ascii = 0, j = 0, x = 0, nblock = 0, size = 0; // block we're in
  int not_finished = 1; // este valor será alterado para 0 quando encontrarmos
                        // @0 e o ciclo para

  FILE *fp_cod = fopen(cod_file, "r");
  char c = (char)fgetc(fp_cod);

  for (int i = 0; not_finished; i++) {

    if (c == '@') {
      ascii = 0;
      if (i == 0) {
        full_seq->compression_type = fgetc(fp_cod);
        c = fgetc(fp_cod);
      } else if (i == 1) {
        c = fgetc(fp_cod);
        full_seq->number_blocks = count_numbers(&c, fp_cod);
        // O c já está igual ao primeiro digito
        // a count numbers vai deixar o c = @ e é para onde o file pointer
        // aponta e vai devolver o valor correspondente ao numero de blocos (já
        // em long)
        param = BLOCK_SIZE;
      } else {
        c = fgetc(fp_cod);
        not_finished = c;
        if (not_finished) {
          if (param == BLOCK_SIZE) {
            size = count_numbers(&c, fp_cod); // o c já tem o primeiro digito
            if (i == 3) {
              full_seq->size_first_block = size;
            }
            full_seq->size_last_block = size;
            param = SEQUENCE;
          } else { // param == sequence
                   // vai incrementando o ascii para saber em que posicao vai e
                   // vai guardar os codigos sf que encontrar e o codigo ascii
                   // correspondente começamos a ler um codigo ascii
            for (; c != '@'; ascii++) {
              if (c != ';') {
                for (x = 0; c != ';'; x++) {
                  sequence[x] = c;
                  c = fgetc(fp_cod);
                }
                sequence[x] = '\0';
                strcpy(full_seq->blocks[nblock].symbol_dictionary[ascii],
                       sequence);
              } else
                full_seq->blocks[nblock].symbol_dictionary[ascii] = NULL;
            }
            nblock++;
          }
        }
      }
    }
  }
}

int main() {
  // input file, in this example it's rle but could be just txt
  char const *symbol_file = "input.txt.rle";
  char *cod_file = malloc(strlen(symbol_file) + 5);      // (".cod" ++ '\0')
  FullSequence *sequence = malloc(sizeof(FullSequence)); // initialize struct
  strcpy(cod_file, symbol_file);
  strcat(cod_file, ".cod"); // codfile = "input.txt.rle.cod"
  read_cod(cod_file, sequence);

  return 0;
}
