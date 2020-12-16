/**
 @file module_f.c
 Responsible for compressing RLE and calculating frequencies.
 Responsavel pela compressao RLE e pelo calculo das frequencias.
*/
#include "module_f.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
/* #include "fsize.h" */

/* FILE */
/* Open a file */
FILE *openFile(const char *file, const char *mode) {
  FILE *new_file = NULL;
  new_file = fopen(file, mode);
  return new_file;
}

/* Close file */
void closeFile(FILE **file) {
  if (!file || !(*file))
    return;

  fclose(*file);
  *file = NULL;
}

/* FREQUENCY */
/* Allocate memory for frequencies */
FreqBlock *initializeFreq(int array[uint_range]) {
  FreqBlock *e = (FreqBlock *)calloc(1, sizeof(FreqBlock));
  e->prox = NULL;
  int i;
  i = 0;
  for (; i < uint_range; i = i + 1)
    e->freq[i] = array[i];
  return e;
}

/* BLOCKS */
Blocks *initializeBlocks() {
  Blocks *e = (Blocks *)calloc(1, sizeof(Blocks));
  e->prox = NULL;
  e->block_size = 0;
  e->blocklist = NULL;
  return e;
}

BlockFiles *initializeBlockFiles() {
  BlockFiles *e = (BlockFiles *)calloc(1, sizeof(BlockFiles));
  e->blocks = NULL;
  e->compression_type = NOT_COMPRESSED;
  e->num_blocks = 0;
  return e;
}

/* Adiciona um bloco (Blocks) no nosso BlockFiles */
void addedBlockTOBloc_file(BlockFiles *e, Blocks *block) {
  Blocks *aux = e->blocks;
  if (aux) {
    while (aux->prox)
      aux = aux->prox;
    aux->prox = block;
  } else {
    e->blocks = block;
  }
  e->num_blocks = e->num_blocks + 1;
}

/* Array of ints to a FreqBlock */
void arrayToFreqBlock(int array[uint_range], FreqBlock *e) {
  FreqBlock *aux, *new;
  aux = e;
  new = initializeFreq(array);

  new->prox = NULL;
  if (aux) {
    while (aux->prox) {
      aux = (aux->prox);
    }
    aux->prox = new;
  } else
    e = new;
}

/* Write the frequencies in a file */
int writeFreq(FILE *fp_in, const char *filename, BlockFiles *BlockFile,
              FreqBlock *freq) {

  long long n_blocks;

  /* Check if the BlockFile is not empty */
  if (BlockFile != NULL)
    n_blocks = BlockFile->num_blocks;
  else
    return WriteFreq_ERROR_IN_BLOCKFILES;

  /* Check if the frequency data is not empty */
  if (freq == NULL)
    return WriteFreq_ERROR_IN_FILE;

  int i, j;

  /* Compression_type */
  char compression_type;
  if (BlockFile->compression_type == NOT_COMPRESSED)
    compression_type = 'N';
  else
    compression_type = 'R';

  Blocks *aux_Blocks;
  aux_Blocks = BlockFile->blocks;
  FreqBlock *aux_Freq;
  aux_Freq = freq;
  unsigned int block_size;

  FILE *fp;
  if (fp_in != NULL)
    fp = fp_in;
  else {
    fp = fopen(filename, "r"); /*  fp = fopen(filename, "rb");*/
    if (fp == NULL)
      return WriteFreq_ERROR_IN_FILE;
  }

  /* ESCREVER:  */
  /* dados inicias */
  /* @<R|N>@NumerodeBlocos */
  fprintf(fp, "%c%c%c%lld", uint_Arroba, compression_type, uint_Arroba,
          n_blocks);
  /* write the frequency of a block */
  i = 0;
  while (aux_Blocks && aux_Freq && i < n_blocks) {
    block_size = aux_Blocks->block_size;
    /* @[tamanho_do_bloco]@ */
    fprintf(fp, "%c%d%c", uint_Arroba, block_size, uint_Arroba);
    j = 0;
    /* Write the frequencies up to the symbol 254 */
    while (j < (uint_range - 1)) { /* tentar melhorar a eficacia...*/
      fprintf(fp, "%d;", aux_Freq->freq[j]);
      j = j + 1;
    }
    /* frequency of symbol 255 */
    fprintf(fp, "%d", aux_Freq->freq[j]);
    i = i + 1;
    aux_Freq = aux_Freq->prox;
    aux_Blocks = aux_Blocks->prox;
  }
  fprintf(fp, "%c0", uint_Arroba);
  if (aux_Freq == NULL && i != n_blocks)
    return WriteFreq_ERROR_IN_FREQ;

  /* aux_Blocks = NULL; */
  /* aux_Freq = NULL; */
  /* free allocated space from auxiliary variables */
  /* ????????????????????????????????????????????? */
  /* free(aux_Blocks); */
  /* free(aux_Freq); */

  fclose(fp);
  return sucess;
}

void print_modulo_f(const char *filename, unsigned int n_blocks,
                    enum compression compression, unsigned int time) {
  unsigned int percentage_compression = 100; /* mudar variavel!!!! */

  /* FALTAM COISAAAASSS !!! */
  /* so onde estao os pontos de interrogacao */
  fprintf(stdout, "\n");
  fprintf(stdout, "Mariana Rodrigues, a93329 && Mike, a \n");
  fprintf(stdout, "MIEI/CD -Dezembro-2020 \n");
  fprintf(stdout, "Módulo: f (Cálculo das frequências dos símbolos) \n");
  fprintf(stdout, "Número de blocos: %d\n", n_blocks);
  fprintf(stdout, "Tamanho dos blocos analisados: \n"); /* ?!!? */
  if (compression == COMPRESSED) {
    fprintf(stdout, "Compressao RLE: %s.rle (%d %c compressão)\n", filename,
            percentage_compression, uint_percentagem);
    fprintf(stdout,
            "Tamanho dos blocos analisados no ficheiro RLE: \n"); /* ?!!? */
  }
  fprintf(stdout, "Tempo de execução do módulo (milissegundos): %d\n", time);
  fprintf(stdout, "Ficheiros gerados: %s.freq", filename);
  if (compression == COMPRESSED)
    fprintf(stdout, ", %s.rle.freq\n", filename);
  fprintf(stdout, "\n");
}

int modulo_f(const char *filename, unsigned long *the_block_size,
             enum compression compression) {

  /* ler o ficheiro */
  /* sugestao: ao colocar o file por blocos fazer ja a contagem dos simbolos
   * por bloco */

  /* fazer compressao ou nao */
  /* se hover compressao fazer contagem dos simbolos ao mesmo tempo q se faz a
   * compressao */

  /* escrever as frequencias */
  /* apresentar menu final */
  return sucess;
}

int 👑() {
  printf("\n\nI AMMMMMMMM 👑");
  for (int i = 0; i < 6; i++)
    printf("👑");
  printf("\n    👻😄\n\n\n");
  return 1;
}

int main() {
  👑();
  /* experiencias: */
  const char *name1 = "Aola.txt";
  /*--------------------------------------------------------*/
  /* testar writeFreq */
  /* testado com 1->10 blocos */
  /* a dar 👌🏻 */
  int a[uint_range];
  for (int i = 0; i < uint_range; i++) {
    a[i] = 12;
  }

  int a1[uint_range], a2[uint_range];
  for (int i = 0; i < uint_range; i++) {
    a1[i] = 50;
    a2[i] = 111;
  }

  FreqBlock *e = initializeFreq(a);
  arrayToFreqBlock(a1, e);
  arrayToFreqBlock(a2, e);
  arrayToFreqBlock(a1, e);
  arrayToFreqBlock(a2, e);
  arrayToFreqBlock(a1, e);
  arrayToFreqBlock(a, e);
  arrayToFreqBlock(a2, e);
  arrayToFreqBlock(a1, e);
  arrayToFreqBlock(a, e);

  /* iniciar a estrutura do file em blocos */
  BlockFiles *BlockFile = initializeBlockFiles();

  /*bloco*/
  Blocks *bloco1 = initializeBlocks();
  (*bloco1).block_size = 1234;
  addedBlockTOBloc_file(BlockFile, bloco1);
  printf("ate ao 1 \n");
  /*bloco*/
  Blocks *bloco2 = initializeBlocks();
  (*bloco2).block_size = 5678;
  addedBlockTOBloc_file(BlockFile, bloco2);
  printf("ate ao 2 \n");
  /*bloco*/
  Blocks *bloco3 = initializeBlocks();
  (*bloco3).block_size = 999;
  addedBlockTOBloc_file(BlockFile, bloco3);
  printf("ate ao 3 \n");
  /*bloco*/
  Blocks *bloco4 = initializeBlocks();
  (*bloco4).block_size = 333;
  addedBlockTOBloc_file(BlockFile, bloco4);
  printf("ate ao 4 \n");
  /*bloco*/
  Blocks *bloco5 = initializeBlocks();
  (*bloco5).block_size = 999;
  addedBlockTOBloc_file(BlockFile, bloco5);
  printf("ate ao 5 \n");
  /*bloco*/
  Blocks *bloco6 = initializeBlocks();
  (*bloco6).block_size = 13;
  addedBlockTOBloc_file(BlockFile, bloco6);
  printf("ate ao 6 \n");
  /*bloco*/
  Blocks *bloco7 = initializeBlocks();
  (*bloco7).block_size = 20;
  addedBlockTOBloc_file(BlockFile, bloco7);
  printf("ate ao 7 \n");
  /*bloco*/
  Blocks *bloco8 = initializeBlocks();
  (*bloco8).block_size = 777;
  addedBlockTOBloc_file(BlockFile, bloco8);
  printf("ate ao 8 \n");

  Blocks *bloco9 = initializeBlocks();
  (*bloco9).block_size = 777;
  addedBlockTOBloc_file(BlockFile, bloco9);
  printf("ate ao 9 \n");
  Blocks *bloco10 = initializeBlocks();
  (*bloco10).block_size = 777;
  addedBlockTOBloc_file(BlockFile, bloco10);
  printf("ate ao 10 \n");

  FILE *fp = fopen("try.txt.freq", "w");
  int resul = writeFreq(fp, name1, BlockFile, e);
  /* int resul = writeFreq(NULL, name1, BlockFile, e); */
  /* o ficheiro foi fechado dentro da funcao writeFreq
   * n deveria ser */
  printf("Write Freq done :%d \n", resul);
  printf("\n");
  free(BlockFile);
  free(e);
  /*--------------------------------------------------------*/

  /* ------------------------------------------------------ */
  /* testar print_modulo_f */
  unsigned int n_blocks = 5;
  unsigned int time = 123;
  enum compression argh = COMPRESSED;
  /* enum compression argh=NOT_COMPRESSED; */
  const char *filename = "Aola.txt";
  print_modulo_f(filename, n_blocks, argh, time);
  /* --------------------------------------------------- */

  printf("\n");

  printf("🤔Tava a dar🤔 \n");
  printf("🤔Agora da sem erros?!🤔 \n");
  printf("🤔Hummmm.. era suposto n dar🤔 \n\n");

  return sucess;
}
