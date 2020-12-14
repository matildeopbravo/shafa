#include "module_c.h"
/* #include "fsize.h" */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int count_numbers(char *c, FILE *fp_cod) {

  char tmp = *c;
  char buffer[20]; // to be determined se sabemos o tamanho ou se usamos array
                   // dinamico
  int i = 0;

  do {
    buffer[i] = tmp;
    i++;
  } while ((tmp = fgetc(fp_cod)) != '@');

  buffer[i] = '\0';
  *c = tmp;
  return atoi(buffer);
}

void read_cod(char *cod_file, FullSequence *full_seq, int *dic[127]) {

  char sequence[uint8_max];
  FILE *fp_cod = fopen(cod_file, "r");
  enum parameters param = START;
  int ascii = -1, j = 0, x = 0, n_block = 0; // block we're in
  char buffer[BUFF_SIZE];
  int not_finished = 1; // este valor será alterado para 0 quando encontrarmos
                        // @0 e o ciclo para
  int count = 0;
  int size = 0;

  char c = (char)fgetc(fp_cod);

  for (int i = 0; not_finished; i++) {
    if (c == '@') {
      ascii = 0;
      if (i == 0) {
        full_seq->compression_type = fgetc(fp_cod);
        c = fgetc(fp_cod);
      } else if (i == 1) {
        c = fgetc(fp_cod);
        full_seq->number_blocks =
            count_numbers(&c, fp_cod); // O c já está igual ao primeiro digito
        // a count numbers vai deixar o c = @ e é para onde o file pointer
        // aponta e vai devolver o valor correspondente ao numero de blocos (já
        // em long)
        param = BLOCK_SIZE;
      } else {
        c = fgetc(fp_cod);
        if ((not_finished = c)) {
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
                  buffer[x] = c;
                  c = fgetc(fp_cod);
                }
                buffer[x] = '\0';
                x++;
                memcpy(buffer, dic[ascii], x); // Ainda tenho que testar isto
              } else
                dic[ascii] = NULL;
            }
          }
        }
      }
    }
  }
}

void show_dic(int *dic[DIC_SIZE]) {
  for (int i = 0; i < DIC_SIZE; i++) {
    if (dic[i] == NULL)
      printf("%i: NULL", i);
    else
      for (int n = 0; dic[i][n] != '\0'; n++)
        printf("%i: %c", i, dic[i][n]);
    putc('\n', stdout);
  }
}

int main() {
  // input file, in this example it's rle but could be just txt
  const char *symbol_file = "input.txt.rle";
  char *cod_file;
  FullSequence sequence = {0}; // initialize struct
  int *dic[DIC_SIZE];
  strcpy(cod_file, symbol_file);
  strcat(cod_file, ".cod"); // codfile = "input.txt.rle.cod"
  read_cod(cod_file, &sequence, dic);

  return 0;
}
