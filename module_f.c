/**
 @file module_f.c
 Responsible for compressing RLE and calculating frequencies.
 Responsavel pela compressao RLE e pelo calculo das frequencias.
*/

#include "module_f.h"
#include "fsize.h"

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
FreqBlock *initializeFreq() {
  FreqBlock *e = (FreqBlock *)calloc(1, sizeof(FreqBlock));
  (*e).prox = NULL;
  for (int i = 0; i < uint_range; i++)
    (*e).freq[i] = 0;
  return e;
}

/* BLOCKS */
/* Allocate memory for blocks */
BlockList *initializeBlockList() {
  BlockList *e = (BlockList *)calloc(1, sizeof(BlockList));
  (*e).value = 0;
  (*e).prox = NULL;
  return e;
}

Blocks *initializeBlocks() {
  Blocks *e = (Blocks *)calloc(1, sizeof(Blocks));
  (*e).prox = NULL;
  (*e).block_size = 0;
  return e;
}

BlockFiles *initializeBlockFiles() {
  BlockFiles *e = (BlockFiles *)calloc(1, sizeof(BlockFiles));
  (*e).blocks = NULL;
  (*e).compression_type = NOT_COMPRESSED;
  (*e).num_blocks = 0;
  return e;
}
/* Add an element to a block list */
void add(BlockList *e, uint8_t value) {
  BlockList *new, *aux;
  aux = e;
  initializeBlockList(new);
  (*new).value = value;
  for (; (*aux).prox != NULL; aux = ((*e).prox))
    ;
  (*aux).prox = new;
}

/* Write the frequencies in a file */
int writeFreq(FILE *fp_in, unsigned char *filename, BlockFiles *BlockFile,
              FreqBlock *freq) {
  FILE *fp;
  long long n_blocks;

  if (BlockFile != NULL)
    n_blocks = (*BlockFile).num_blocks;
  else
    return WriteFreq_ERROR_IN_BLOCKFILES;
  /* unsigned int n_blocks; n_blocks = (*BlockFile).num_blocks ; */
  int i, j;
  i = 1;

  /* Compression_type */
  char compression_type;
  if ((*BlockFile).compression_type == NOT_COMPRESSED)
    compression_type = 'N';
  else
    compression_type = 'R';

  Blocks *aux_Blocks;
  aux_Blocks = &(*BlockFile).blocks;
  FreqBlock *aux_Freq;
  aux_Freq = freq;
  /* unsigned int block_size; */

  /* Check if the frequency data is not empty */
  if (freq == NULL)
    return WriteFreq_ERROR_IN_FILE;

  /* Open the file in binary write mode */
  /* if ( filename==NULL || *filename == 0 ) fp = fp_in; */
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
  while (/* aux_Blocks && */ aux_Freq && i <= n_blocks) {
    /* block_size = (*aux_Blocks)->block_size; */
    fprintf(fp, "%c", uint_Arroba);
    j = 0;
    /* Write the frequencies up to the symbol 254 */
    while (j < 255) { /* tentar melhorar a eficacia... tem muitos coisos , so
                         para chamar atencao e n esquecer */
      fprintf(fp, "%d;", (*aux_Freq).freq[j]);
      fprintf(fp, "%d;", (*aux_Freq).freq[j + 1]);
      fprintf(fp, "%d;", (*aux_Freq).freq[j + 2]);
      fprintf(fp, "%d;", (*aux_Freq).freq[j + 3]);
      fprintf(fp, "%d;", (*aux_Freq).freq[j + 4]);
      j = j + 5;
    }
    /* frequency of symbol 255 */
    fprintf(fp, "%d", (*aux_Freq).freq[j]);

    i = i + 1;
    aux_Freq = &(*aux_Freq).prox;
    /* aux_Blocks = &(*aux_Blocks).prox; */
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
  printf("Tava a dar🤔 \n");

  fclose(fp);
  printf("humm.estranho\n");
  return sucess;
}

void print_modulo_f(unsigned char *filename, unsigned int n_blocks,
                    enum compression compression, unsigned int time) {
  unsigned int percentage_compression = 100; /* mudar variavel!!!! */

  /* FALTAM COISAAAASSS !!! */
  /* so onde estao os pontos de interrogacao */
  fprintf(stdout, "\n");
  fprintf(stdout, "Mariana Rodrigues, a93329 && Mick, a \n");
  fprintf(stdout, "MIEI/CD -Dezembro-2020 \n");
  fprintf(stdout, "Módulo: f (Cálculo das frequências dos símbolos) \n");
  fprintf(stdout, "Número de blocos: %d\n", n_blocks);
  fprintf(stdout, "Tamanho dos blocos analisados: \n"); /* ?!!? */
  if (compression == COMPRESSED) {
    fprintf(stdout, "Compressao RLE: %s.rle (%d (percentagem) compressão)\n",
            filename, percentage_compression);
    fprintf(stdout,
            "Tamanho dos blocos analisados no ficheiro RLE: \n"); /* ?!!? */
  }
  fprintf(stdout, "Tempo de execução do módulo (milissegundos): %d\n", time);
  fprintf(stdout, "Ficheiros gerados: %s.freq", filename);
  if (compression == COMPRESSED)
    fprintf(stdout, ", %s.rle.freq\n", filename);
  fprintf(stdout, "\n");
}

int modulo_f(unsigned char *filename, unsigned long *the_block_size,
             enum compression compression) {

  /* ler o ficheiro */
  /* sugestao: ao colocar o file por blocos fazer ja a contagem dos simbolos por
   * bloco */

  /* fazer compressao ou nao */
  /* se hover compressao fazer contagem dos simbolos ao mesmo tempo q se faz a
   * compressao */

  /* escrever as frequencias */
  /* apresentar menu final */
  return sucess;
}

int 👑() {
  int a = 5;
  return a;
}

int main() {

  /* experiencias: */

  unsigned char *name1 = "Aola.txt";
  /* FreqBlock *e; */
  FreqBlock *e = initializeFreq();

  /* printf("aqui tou"); */
  for (int i = 0; i < uint_range; i++) {
    (*e).freq[i] = i * 1;
  }
  FreqBlock *aux = initializeFreq();
  for (int i = 0; i < uint_range; i++) {
    (*aux).freq[i] = i * 100;
  }
  (*e).prox = &aux;

  printf("Tava a dar🤔 \n");
  printf("ate aqui nice -> %d\n", (*e).freq[4]);

  BlockFiles *BlockFile = initializeBlockFiles();
  (*BlockFile).num_blocks = 2;
  FILE *fp = fopen("try.txt", "w");

  writeFreq(fp, name1, BlockFile, e);
  /* o ficheiro foi fechado dentro da funcao writeFreq
   * n deveria ser */
  printf("Write Freq done\n");

  printf("\n");
  /* ------------------------------------------------------ */
  /* testar print_modulo_f */
  unsigned int n_blocks = 2;
  unsigned int time = 123;
  enum compression argh = COMPRESSED;
  /* enum compression argh=NOT_COMPRESSED; */
  unsigned char *filename = "Aola.txt";
  print_modulo_f(filename, n_blocks, argh, time);
  /* --------------------------------------------------- */
  printf("\n");

  return sucess;
}
