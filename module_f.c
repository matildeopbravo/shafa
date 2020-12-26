/**
 @file module_f.c
 Responsible for compressing RLE and calculating frequencies.
 Responsavel pela compressao RLE e pelo calculo das frequencias.
*/
#include "module_f.h"
#include "dynamic_arrays.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

/* relativo as frequencias */

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

Blocks_C *initializeBlocks_C() {
  Blocks_C *e = (Blocks_C *)calloc(1, sizeof(Blocks_C));
  e->prox = NULL;
  e->block_size = 0;
  e->tBList = NULL;
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

/* mike */
/* ler file e colocar no array */

/* imprime um ByteVec */
void printByteVec(ByteVec const *self) {
  size_t i = 0;
  size_t used = byte_vec_used(self);
  for (; i < used; i++)
    printf("%d ", byte_vec_index(self, i));
  printf("\n");
}

/* le um bloco de um ficheiro e converte-o em ByteVec */
ByteVec *loadArray(FILE *file, size_t *used, size_t block_size) {
  size_t i = 0;
  ByteVec *self = byte_vec_new();
  uint8_t x = 0;

  while (i < block_size && (fread(&x, sizeof(x), 1, file) == 1)) {
    byte_vec_push(self, x);
    i++;
  }
  *used = self->used;
  return self;
}

/* Calcula a compressao de um determinado ByteTupple */
float calcCompress(TuppleVec const *self, size_t *x) {
  size_t i = 0, r = 0;
  float b = 0;
  ByteTupple a;
  size_t used = tupple_vec_used(self);
  for (; i < used; i++) {
    a = tupple_vec_index(self, i);
    if (a.count > 3) {
      r = r + 3;
    } else
      r += a.count;
  }
  b = ((*x - 1) - r);
  b = (b / (*x - 1)) * 100;
  return b;
}

/* Faz a conversao RLE de um determinado ByteVec, returnando um TuppleVec */
TuppleVec *compress(ByteVec const *self) {
  TuppleVec *t = tupple_vec_new();
  ByteTupple a;
  size_t i = 1, count = 1;
  uint8_t last = self->vec[0], las2;
  size_t used = byte_vec_used(self);
  for (; i < used; i++) {
    if (last == self->vec[i]) {
      count++;
      last = byte_vec_index(self, i);
      las2 = byte_vec_index(self, i - 1);
    } else {
      tupple_vec_push(t, last, count);
      last = byte_vec_index(self, i);
      las2 = byte_vec_index(self, (i - 1));
      count = 1;
    }
  }
  if (last == las2) {
    a = tupple_vec_pop(t);
    tupple_vec_push(t, last, a.count + 1);
  } else if (last != 10)
    tupple_vec_push(t, last, count);

  return t;
}

/* NAO DA */
void Block_to_RLE(BlockFiles *file) {
  size_t i = 0;
  /* size_t x = 0; */
  file->blocks_c = initializeBlocks_C();
  Blocks_C *e = file->blocks_c;
  Blocks *aux = file->blocks;

  printByteVec(aux->blocklist);
  printByteVec(aux->prox->blocklist);
  printf("\nola     %d  \n", file->num_blocks);

  for (; i < file->num_blocks && aux; i++, aux = aux->prox) {
    printf("antes");
    /* e->tBList = tupple_vec_new_ola(); */
    /* e->tBList = compress_(aux->blocklist); */
    printf(":  apos\n");
    /* e->block_size = calcCompress(e->tBList, &x); /\* Adicionar a compressao
     * *\/ */
    e = e->prox;
  }
}

/* Escreve a compressao rle num file... so de um TuppleVec */
int write_compressed(FILE *file, TuppleVec const *self) {
  uint8_t x, temp, y;
  ByteTupple b;
  size_t i = 0;
  for (; i < self->used; i++) {
    temp = 0;
    b = tupple_vec_index(self, i);
    x = b.byte;
    y = b.count;
    if (y > 3 || x == 0) {
      fputc(0, file);
      fputc(x, file);
      fputc(y, file);
    } else
      while (temp != y) {
        fputc(x, file);
        temp++;
      }
  }
  fclose(file);
  return 0;
}

void printEqual(TuppleVec const *vec) {
  int counter = 0;
  size_t i = 0;
  for (; i < tupple_vec_used(vec); i++) {
    ByteTupple self = tupple_vec_index(vec, i);
    printf("%d,%d ", self.byte, self.count);
    counter = self.count + counter;
  }
  printf("\n %d \n", counter);
}

int checkSum(ByteVec *self, TuppleVec *tupplevec) {
  size_t used = 0;
  float i = 0;
  used = self->used;
  tupplevec = compress(self);
  i = calcCompress(tupplevec, &used);
  if (i < 5)
    return 0;
  return 1;
}

int building(FILE *file, BlockFiles *start, size_t const n_blocks,
             size_t const size_last_block, size_t const block_size,
             size_t *used) {
  if (file) {
    size_t num_block = 0;
    for (; num_block < (n_blocks - 1); num_block++) {
      Blocks *aux = initializeBlocks();

      aux->blocklist = loadArray(file, used, block_size);

      /* apagar depois */
      /* printByteVec(aux->blocklist); */

      aux->block_size = block_size;
      addedBlockTOBloc_file(start, aux);
    }
    Blocks *aux = initializeBlocks();
    aux->blocklist = loadArray(file, used, size_last_block);
    /* printByteVec(aux->blocklist); */
    aux->block_size = size_last_block;
    addedBlockTOBloc_file(start, aux);
    aux->prox = NULL;
    return 1;
  } else
    return BUILDING_ERROR_IN_FILE;
}

/* 1. dar outro nome
 * Recebe um BlockFiles e retorna as suas frequencias */
FreqBlock *calFreq(BlockFiles const *file) {
  if (file) {
    Blocks *blocks = file->blocks;
    int num_blocks = file->num_blocks, num = 0;
    ByteVec *vec = file->blocks->blocklist;
    size_t i = 0;
    int array[uint_range];
    for (; i < uint_range; i++)
      array[i] = 0;

    /*1 bloco*/
    size_t used = byte_vec_used(vec);
    size_t used_last = used;
    for (i = 0; i < used; i++)
      array[byte_vec_index(vec, i)]++;

    blocks = blocks->prox;
    num++;

    FreqBlock *freq = initializeFreq(array);
    /* outros blocos */
    while (num <= num_blocks && blocks) {
      vec = blocks->blocklist;
      used = byte_vec_used(vec);

      /* garantir q o array esta todo a 0 */
      for (i = 0; i < uint_range; i++)
        array[i] = 0;

      /* preencher o array com as freq */
      for (i = 0; i < byte_vec_used(vec); i++)
        array[byte_vec_index(vec, i)]++;

      /* adicionar as frequencias */
      arrayToFreqBlock(array, freq);
      num++;
      blocks = blocks->prox;
      used = used_last;
    }
    return freq;
  } else
    return NULL;
}

/* ------------------------------------------------------ */

/* escrever coisas.. sei la .. ta muito grande o file*/

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

  int i, j, last = -1;

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
  int num_freq;
  while (aux_Blocks && aux_Freq && i < n_blocks) {
    block_size = aux_Blocks->block_size;
    /* @[tamanho_do_bloco]@ */
    fprintf(fp, "%c%d%c", uint_Arroba, block_size, uint_Arroba);
    j = 0;
    /* Write the frequencies up to the symbol 254 */
    while (j < (uint_range - 1)) { /* tentar melhorar a eficacia...*/
      num_freq = aux_Freq->freq[j];

      if (last != num_freq) {
        fprintf(fp, "%d;", num_freq);
        last = num_freq;
      } else
        fprintf(fp, ";");

      j = j + 1;
    }
    /* frequency of symbol 255 */
    num_freq = aux_Freq->freq[j];
    if (last != num_freq)
      fprintf(fp, "%d;", num_freq);
    else {
      fprintf(fp, ";");
    }
    i = i + 1;
    aux_Freq = aux_Freq->prox;
    aux_Blocks = aux_Blocks->prox;
    last = -1;
  }
  fprintf(fp, "%c0", uint_Arroba);
  if (aux_Freq == NULL && i != n_blocks)
    return WriteFreq_ERROR_IN_FREQ;

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

int modulo_f(char const *filename, size_t const the_block_size,
             enum compression compression) {

  /* ler o ficheiro */
  char filename_[3064];
  strcpy(filename_, filename);
  size_t x = 0;
  FILE *rfile, *wfile;
  rfile = fopen(filename, "rb");
  if (!rfile)
    return -1;

  /* SEM COMPRESSAO */
  /*Iniciamos a nossa estrutura de dados*/
  BlockFiles *block_file = initializeBlockFiles();

  /* verificamos quantos blocos iremos ter e os seus tamanhos */
  size_t num_blocks = 2;
  size_t size_last_block = 952;

  /* Lemos o file por blocos, colocando o na nossa estrutura de dados */
  int error = building(rfile, block_file, num_blocks, size_last_block,
                       the_block_size, &x);

  if (error != 1)
    return -2;

  /* Iremos determinar a frequencia de cada bloco */
  FreqBlock *freq_file = calFreq(block_file);
  if (!freq_file)
    return -3;

  /* Escrever as frequencias obtidas (sem compressao) */
  char *filename_freq = filename_;
  strcat(filename_freq, ".freq");

  wfile = fopen(filename_freq, "w");
  error = writeFreq(wfile, filename_freq, block_file, freq_file);
  if (error != 1)
    return -1;

  /* RLE */
  if (compression == COMPRESSED) {
    /* fazer coisas */
  }

  /* apresentar menu final */
  return sucess;
}

/* pode se apagar antes de enviar */
void print_freq(FreqBlock *freq) {
  FreqBlock *aux = freq;
  while (aux) {
    for (int i = 0; i < uint_range; i++)
      printf(" %d,", aux->freq[i]);
    printf("\n");
    aux = aux->prox;
  }
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
  /* ------------------------------------------------------ */
  /* testar print_modulo_f */
  unsigned int n_blocks = 5;
  unsigned int time = 123;
  enum compression argh = COMPRESSED;
  /* enum compression argh=NOT_COMPRESSED; */
  const char *filename = "Aola.txt";
  print_modulo_f(filename, n_blocks, argh, time);
  /* --------------------------------------------------- */

  /*  mike  */
  /* imprime os blocos */
  /* Blocks *ola = start->blocks; */
  /* for (int r = 0; r < start->num_blocks; r++) { */
  /*   /\* printf("tamanha do  %ld\n", ola->blocklist->used); *\/ */
  /*   printf("bloco: %d\n", r); */
  /*   printByteVec(ola->blocklist); */
  /*   ola = ola->prox; */
  /* } */
  int arroz = modulo_f("bbb.txt", 2048, NOT_COMPRESSED);
  if (arroz != 1) {
    printf("algo deu mal\n");
  }

  /*-------------------------------------------*/
  printf("\n");
  printf("🤔Tava a dar🤔 \n");
  printf("🤔Agora da sem erros?!🤔 \n");
  printf("🤔Hummmm.. era suposto n dar🤔 \n\n");

  return sucess;
}
