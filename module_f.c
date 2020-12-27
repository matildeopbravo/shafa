/**
 @file module_f.c
 Responsible for compressing RLE and calculating frequencies.
 Responsavel pela compressao RLE e pelo calculo das frequencias.
*/
#include "module_f.h"
#include "dynamic_arrays.h"
#include "fsize.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* #define FORCE_FLAG 1 */
#define N_BLOCKS 2
#define SIZE_OF_LAST_BLOCK 952
#define BLOCK_SIZE 2048

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

/* libertar memoria */
void free_Freq(FreqBlock *e) {
  FreqBlock *aux;
  while (e) {
    aux = e;
    e = e->prox;
    free(aux);
  }
}

/* BLOCKS */
Blocks *initializeBlocks() {
  Blocks *e = (Blocks *)calloc(1, sizeof(Blocks));
  e->prox = NULL;
  e->block_size = 0;
  e->blocklist = NULL;
  return e;
}

void free_Blocks(Blocks *e) {
  Blocks *aux;
  ByteVec *aux_vec;
  while (e) {
    aux = e;
    aux_vec = e->blocklist;
    e->blocklist = NULL;
    byte_vec_del(aux_vec);
    e = e->prox;
    free(aux);
  }
}

Blocks_C *initializeBlocks_C() {
  Blocks_C *e = (Blocks_C *)calloc(1, sizeof(Blocks_C));
  e->prox = NULL;
  e->block_size = 0;
  e->tBList = NULL;
  return e;
}

void free_Blocks_C(Blocks_C *e) {
  Blocks_C *aux;
  TuppleVec *aux_vec;
  while (e) {
    aux = e;
    aux_vec = e->tBList;
    e->tBList = NULL;
    tupple_vec_del(aux_vec);
    e = e->prox;
    free(aux);
  }
}

BlockFiles *initializeBlockFiles() {
  BlockFiles *e = (BlockFiles *)calloc(1, sizeof(BlockFiles));
  e->blocks = NULL;
  e->blocks_c = NULL;
  e->compression_type = NOT_COMPRESSED;
  e->num_blocks = 0;
  return e;
}

void free_Blocks_file(BlockFiles *e) {
  free_Blocks(e->blocks);
  free_Blocks_C(e->blocks_c);
  free(e);
  e = NULL;
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

// Adiciona um bloco_c (Blocks_C) no nosso BlockFiles
void addedBlock_CTOBloc_file(BlockFiles *e, Blocks_C *self) {
  Blocks_C *aux = e->blocks_c;
  if (aux) {
    while (aux->prox)
      aux = aux->prox;
    aux->prox = self;
  } else {
    e->blocks_c = self;
  }
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
  /* nada de dar free ao aux ou ao new */
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
double calcCompress(TuppleVec const *self) {
  size_t i = 0, r = 0;
  size_t x = self->n_used;
  double b = 0;
  ByteTupple a;
  size_t used = tupple_vec_used(self);
  /* for (; i < self->used; i++) { */
  for (; i < used; i++) {
    a = tupple_vec_index(self, i);
    if (a.count > 3) {
      r = r + 3;
    } else
      r += a.count;
  }
  b = (x - r);
  b = (b / x) * 100;
  return b;
}

/* Faz a conversao RLE de um determinado ByteVec, returnando um TuppleVec */
TuppleVec *compress(ByteVec const *self) {
  TuppleVec *t = tupple_vec_new();
  ByteTupple a;
  size_t i = 1, count = 1;
  uint8_t last = self->vec[0], last2;
  size_t used = byte_vec_used(self);
  for (; i < used; i++) {
    if (last == self->vec[i]) {
      count++;
      last = byte_vec_index(self, i);
      last2 = byte_vec_index(self, i - 1);
    } else {
      tupple_vec_push(t, last, count);
      last = byte_vec_index(self, i);
      last2 = byte_vec_index(self, (i - 1));
      count = 1;
    }
  }
  if (last == last2) {
    a = tupple_vec_pop(t);
    tupple_vec_push(t, last, a.count + 1);
  } else if (last != 10)
    tupple_vec_push(t, last, count);

  return t;
}

/* Escreve a compressao rle num file... so de um TuppleVec */
int write_compressed(FILE *file, BlockFiles const *self) {
  uint8_t x, temp, y;
  ByteTupple b;
  Blocks_C *list = self->blocks_c;
  size_t used = 0, i;
  while (list) {
    TuppleVec *yes = list->tBList;
    used = tupple_vec_used(yes);
    for (i = 0; i < used; i++) {
      temp = 0;
      b = tupple_vec_index(yes, i);
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
    list = list->prox;
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

int checkSum(ByteVec *self) {
  float i = 0;
  TuppleVec *vec = tupple_vec_new();
  vec = compress(self);
  i = calcCompress(vec);
  if (i < 5)
    return 0;
  return 1;
}

/* recebe um file e coloca-o por blocos na nossa estrutura de dados
 * Recebe um start inicializado sem nada */
int building(FILE *file, BlockFiles *start, size_t const n_blocks,
             size_t const size_last_block, size_t const block_size,
             size_t *used) {
  if (file) {
    size_t num_block = 0;
    for (; num_block < (n_blocks - 1); num_block++) {
      Blocks *aux = initializeBlocks();
      aux->blocklist = loadArray(file, used, block_size);
      aux->block_size = block_size;
      addedBlockTOBloc_file(start, aux);
    }
    Blocks *aux = initializeBlocks();
    aux->blocklist = loadArray(file, used, size_last_block);
    aux->block_size = size_last_block;
    addedBlockTOBloc_file(start, aux);
    aux->prox = NULL;
    return 1;
  } else
    return BUILDING_ERROR_IN_FILE;
}

/**/
int compress_blocks(BlockFiles *self, int FORCE_FLAG) {
  Blocks *list = initializeBlocks();
  if (checkSum(self->blocks->blocklist) == 0 && !FORCE_FLAG) {
    return 0;
  }
  list = self->blocks;
  int i = 1;
  while (list) {
    Blocks_C *a = initializeBlocks_C();
    a->tBList = compress(list->blocklist);

    /* n sei se esta certo */
    a->block_size = a->tBList->n_used;
    /* printf("%d\n", a->block_size); */

    addedBlock_CTOBloc_file(self, a);
    list = list->prox;
    i++;
  }
  return 1;
}

double calcCompress_blocks(BlockFiles const *self) {
  double r = 0;
  Blocks_C *list = self->blocks_c;
  while (list) {
    r += calcCompress(list->tBList);
    list = list->prox;
  }
  r = r / self->num_blocks;
  return r;
}

/* 1. dar outro nome
 * Recebe um BlockFiles e retorna as suas frequencias na nossa estrutura de
 * freqs */
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

FreqBlock *calFreq_RLE(BlockFiles *file) {
  if (file) {
    Blocks_C *blocks = file->blocks_c;
    int num_blocks = file->num_blocks, num = 0;
    TuppleVec *vec = file->blocks_c->tBList;
    size_t i = 0;
    int array[uint_range];
    for (; i < uint_range; i++)
      array[i] = 0;

    size_t block_size = 0;

    /*1 bloco*/
    ByteTupple aux;
    size_t used = tupple_vec_used(vec);
    for (i = 0; i < used; i++) {
      aux = tupple_vec_index(vec, i);
      array[aux.byte]++;
      block_size++;
      if (aux.count > 1) {
        array[0]++;
        array[aux.count]++;
        block_size += 2;
      } else if (aux.count == 2) {
        array[aux.byte]++;
        block_size++;
      }
    }
    blocks->block_size = block_size;
    blocks = blocks->prox;
    num++;

    FreqBlock *freq = initializeFreq(array);
    /* outros blocos */
    while (num <= num_blocks && blocks) {
      block_size = 0;
      vec = blocks->tBList;
      used = tupple_vec_used(vec);

      /* garantir q o array esta todo a 0 */
      for (i = 0; i < uint_range; i++)
        array[i] = 0;

      /* preencher o array com as freq */
      used = tupple_vec_used(vec);
      for (i = 0; i < used; i++) {
        aux = tupple_vec_index(vec, i);
        array[aux.byte]++;
        block_size++;
        if (aux.count > 1) {
          array[0]++;
          array[aux.count]++;
          block_size += 2;
        } else if (aux.count == 2) {
          array[aux.byte]++;
          block_size++;
        }
      }
      /* adicionar as frequencias */
      arrayToFreqBlock(array, freq);
      num++;
      blocks->block_size = block_size;
      blocks = blocks->prox;
    }
    return freq;
  } else
    return NULL;
}

/* ------------------------------------------------------ */

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
  Blocks_C *aux_Blocks_C;
  aux_Blocks_C = BlockFile->blocks_c;
  FreqBlock *aux_Freq;
  aux_Freq = freq;
  size_t block_size;

  FILE *fp;
  if (fp_in != NULL)
    fp = fp_in;
  else {
    fp = fopen(filename, "r");
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
    if (compression_type == 'N')
      block_size = aux_Blocks->block_size;
    else {
      /* block_size = 100; */
      block_size = aux_Blocks_C->block_size;
      /* printf("\n block_size: %d \n", block_size); */
    }

    /* @[tamanho_do_bloco]@ */
    fprintf(fp, "%c%ld%c", uint_Arroba, block_size, uint_Arroba);
    j = 0;
    /* Write the frequencies up to the symbol 254 */
    while (j < (uint_range - 1)) {
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
    if (compression_type == 'R')
      aux_Blocks_C = aux_Blocks_C->prox;
    last = -1;
  }
  fprintf(fp, "%c0", uint_Arroba);
  if (aux_Freq == NULL && i != n_blocks)
    return WriteFreq_ERROR_IN_FREQ;

  fclose(fp);
  return sucess;
}

/* print modulo f */
void print_module_f(const char *filename, BlockFiles const *self,
                    double const time) {

  unsigned int percentage_compression = 100; /* mudar variavel!!!! */
  size_t n_blocks = self->num_blocks;
  enum compression compression_type = self->compression_type;

  /* FALTAM COISAAAASSS !!! */
  /* so onde estao os pontos de interrogacao */
  fprintf(stdout, "\n");
  fprintf(stdout, "Mariana Rodrigues, a93329 && Mike, a \n");
  fprintf(stdout, "MIEI/CD 26-Dezembro-2020 \n");
  fprintf(stdout, "Módulo: f (Cálculo das frequências dos símbolos) \n");
  fprintf(stdout, "Número de blocos: %ld\n", n_blocks);
  fprintf(stdout, "Tamanho dos blocos analisados: \n"); /* ?!!? */
  if (compression_type == COMPRESSED) {
    fprintf(stdout, "Compressao RLE: %s.rle (%d %c compressão)\n", filename,
            percentage_compression, uint_percentagem);
    fprintf(stdout,
            "Tamanho dos blocos analisados no ficheiro RLE: \n"); /* ?!!? */
  }
  fprintf(stdout, "Tempo de execução do módulo (milissegundos): %f\n", time);
  fprintf(stdout, "Ficheiros gerados: %s.freq", filename);
  if (compression_type == COMPRESSED)
    fprintf(stdout, ", %s.rle, %s.rle.freq\n", filename, filename);
  fprintf(stdout, "\n");
}

void determine_size(char const *filename, size_t *n_blocks,
                    size_t *size_last_blocks, size_t *block_size) {
  FILE *fp = openFile(filename, "r");
  size_t aux = ftell(fp);

  *n_blocks = aux / *block_size;
  if (((*n_blocks) * (*block_size)) == aux)
    *size_last_blocks = *block_size;
  else {
    *size_last_blocks = aux - (*n_blocks) * (*block_size);
    (*n_blocks)++;
  }

  fclose(fp);
}

int module_f(char const *filename, size_t const the_block_size,
             int FORCE_FLAG) {

  clock_t Ticks[2];
  /* Start time */
  Ticks[0] = clock();

  /* ler o ficheiro */
  char filename_[3064];
  strcpy(filename_, filename);

  size_t x = 0, x1 = 0;
  FILE *rfile, *wfile, *wfile_rle;
  rfile = fopen(filename, "rb");
  if (!rfile)
    return Module_f_ERROR_IN_FILE;

  /* SEM COMPRESSAO */
  /*Iniciamos a nossa estrutura de dados*/
  BlockFiles *self = initializeBlockFiles();

  /* verificamos quantos blocos iremos ter e os seus tamanhos */
  /* size_t num_blocks = 2; */
  /* size_t size_last_block = 952; */
  /* size_t n_blocks = N_BLOCKS, size_last_block = SIZE_OF_LAST_BLOCK, */
  /* block_size = BLOCK_SIZE; */

  size_t n_blocks = 0, size_last_block = 0, block_size = the_block_size;

  /* determine_size(filename, &n_blocks, &size_last_block, &block_size); */

  FILE *fp1 = fopen(filename, "r");
  n_blocks = fsize(fp1, filename, &block_size, &size_last_block);

  /* apagar depois */
  printf("%ld, %ld, %ld,\n ", n_blocks, size_last_block, block_size);

  /* Lemos o file por blocos, colocando o na nossa estrutura de dados */
  int error =
      building(rfile, self, n_blocks, size_last_block, the_block_size, &x);

  if (error != 1)
    return Module_f_ERROR_IN_BLOCKFILES;
  else
    fclose(rfile);

  /* Iremos determinar a frequencia de cada bloco */
  FreqBlock *freq_file = calFreq(self);
  if (!freq_file)
    return Module_f_ERROR_IN_FREQ;

  /* Escrever as frequencias obtidas (sem compressao) */
  char filename_freq[3000];
  strcpy(filename_freq, filename_);
  strcat(filename_freq, ".freq");

  wfile = fopen(filename_freq, "w");
  error = writeFreq(wfile, filename_freq, self, freq_file);
  if (error != 1)
    return Module_f_ERROR_IN_FILE;

  /* RLE */
  x1 = compress_blocks(self, FORCE_FLAG);
  if (!x1 && !FORCE_FLAG) {
    return 1;
  }

  char *filename_rle = filename_;
  strcat(filename_rle, ".rle");
  wfile_rle = fopen(filename_rle, "w");

  if (FORCE_FLAG) {
    write_compressed(wfile_rle, self);
    /* calcular as freqs e imprimir */
    self->compression_type = COMPRESSED;

    FreqBlock *freq_file_rle = calFreq_RLE(self);
    char *filename_rle_freq = filename_rle;
    strcat(filename_rle, ".freq");
    wfile_rle = fopen(filename_rle, "w");
    error = writeFreq(wfile, filename_rle_freq, self, freq_file_rle);
    if (error != 1)
      return Module_f_ERROR_IN_FILE;

  } else if (calcCompress(self->blocks_c->tBList) > 5) {
    write_compressed(wfile_rle, self);
    /* calcular as freqs e imprimir */
    self->compression_type = COMPRESSED;

    FreqBlock *freq_file_rle = calFreq_RLE(self);
    char *filename_rle_freq = filename_rle;
    strcat(filename_rle, ".freq");
    wfile_rle = fopen(filename_rle, "w");
    error = writeFreq(wfile, filename_rle_freq, self, freq_file_rle);
    if (error != 1)
      return Module_f_ERROR_IN_FILE;
  }

  float block_1 = calcCompress(self->blocks_c->tBList);
  float blocks = calcCompress(self->blocks_c->tBList);
  printf("First Block compression -> %d \n", (int)block_1);
  /* printf("Average Compression of All Blocks -> %f \n", (float)blocks); */
  printf("Average Compression of All Blocks -> %f \n", (double)blocks);

  /* End time */
  Ticks[1] = clock();
  double time = (Ticks[1] - Ticks[0]) * 1000.0 / CLOCKS_PER_SEC;

  /* apresentar menu final */
  print_module_f(filename, self, time);

  free_Freq(freq_file);
  free_Blocks_file(self);

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

  /* chama module f */
  int arroz = module_f("ola.txt", 2048, 1);
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
