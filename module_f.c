/**
 * @file module_f.c
 * @author Mariana Dinis Rodrigues e Mike
 * @date 27 Dezembro 2020
 * Responsible for compressing RLE and calculating frequencies.
 * @brief Responsavel pela compressao RLE e pelo calculo das frequencias.
 */

#include "module_f.h"
#include "dynamic_arrays.h"
#include "fsize_.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//#define FORCE_FLAG 1
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

/* mike */
/* ler file e colocar no array */
/* Calcula a compressao de um determinado ByteTupple */
long double calcCompress(TuppleVec const *self) {
  size_t i = 0, r = 0, x = self->n_used, used = tupple_vec_used(self);
  long double b = 0;
  ByteTupple a;
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
        size_t i = 1, count = 1, used = self->used;
        uint8_t last = byte_vec_index(self,0), las2 = 0;
        while (i < used && count < 255) {
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
		i++;
        }
        if (last == las2) {
                tupple_vec_push(t, last, count);
        } else if (last != 10)
                tupple_vec_push(t, last, count);
        t->n_used = self->used;
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

int checkSum(ByteVec *self) {
  long double i = 0;
  TuppleVec *vec = tupple_vec_new();
  vec = compress(self);
  i = calcCompress(vec);
  if (i < 5)
    return 0;
  return 1;
}

/* recebe um file e coloca-o por blocos na nossa estrutura de dados
 * Recebe um start inicializado sem nada */
int building_blocks(FILE *file, BlockFiles *self, size_t n_blocks,size_t size_last_block, size_t block_size) {
	if (!file)
		return BUILDING_ERROR_IN_FILE;
	size_t block = 0;
        for (; block < (n_blocks - 1); block++) {
                Blocks *aux = initializeBlocks();
                aux->blocklist = loadArray(file, block_size);
                aux->block_size = block_size;
                addedBlockTOBloc_file(self, aux);
        }
        Blocks *aux = initializeBlocks();
        aux->blocklist = loadArray(file, size_last_block);
        aux->block_size = size_last_block;
        aux->prox = NULL;
        addedBlockTOBloc_file(self, aux);
	return 1;
}



/**/
int compress_blocks(BlockFiles *self,int FORCE_FLAG){
        Blocks *list = initializeBlocks();
        if (checkSum(self->blocks->blocklist) == 0 && !FORCE_FLAG){
                return 0;
        }
        list = self->blocks;
        int i = 1;
        while (list){
                Blocks_C *a = initializeBlocks_C();
                a->tBList = compress(list->blocklist);
                a->block_size = list->block_size;
                addedBlock_CTOBloc_file(self,a);
                list = list->prox;
                i++;
        }
        return 1;
}

long double calcCompress_blocks(BlockFiles const *self) {
  long double r = 0;
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
    while (j < (uint_range - 2)) {
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
void print_module_f(const char *filename, BlockFiles const *self, long double percentage_compression,
                    double const time) {
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
    fprintf(stdout, "Compressao RLE: %s.rle (%Lg %c compressão)\n", filename,
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

  size_t x1 = 0;
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

  FILE *fp1 = fopen(filename, "r");
  /* funcao do stor com algumas alteracoes */
  n_blocks = fsize(fp1, filename, &block_size, &size_last_block);
  /* fclose(fp1); */

  /* apagar depois */
  printf("%ld, %ld, %ld,\n ", n_blocks, size_last_block, block_size);

  /* Lemos o file por blocos, colocando o na nossa estrutura de dados */
  int error =
      building_blocks(rfile, self, n_blocks, size_last_block, the_block_size);

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
  /* wfile_rle = fopen(filename_rle, "w"); */

  if (FORCE_FLAG) {
    wfile_rle = fopen(filename_rle, "w");
    /* comprimir os blocos */
    write_compressed(wfile_rle, self);
    self->compression_type = COMPRESSED;

    /* Calcular as frequencias por bloco */
    FreqBlock *freq_file_rle = calFreq_RLE(self);

    char *filename_rle_freq = filename_rle;
    strcat(filename_rle, ".freq");
    wfile_rle = fopen(filename_rle, "w");
    /* imprimir as frequencias */
    error = writeFreq(wfile, filename_rle_freq, self, freq_file_rle);
    if (error != 1)
      return Module_f_ERROR_IN_FILE;
    else
      free_Freq(freq_file_rle);

  } else if (calcCompress(self->blocks_c->tBList) > 5) {
    /* comprimir os blocos */
    write_compressed(wfile_rle, self);
    self->compression_type = COMPRESSED;
    /* Calcular as frequencias */
    FreqBlock *freq_file_rle = calFreq_RLE(self);
    char *filename_rle_freq = filename_rle;
    strcat(filename_rle, ".freq");
    /* escrever as frequencias */
    error = writeFreq(wfile, filename_rle_freq, self, freq_file_rle);
    if (error != 1)
      return Module_f_ERROR_IN_FILE;
    else
      free_Freq(freq_file_rle);
  }

  /* apagar depois... ta a dar mal */
  long double block_1 = calcCompress(self->blocks_c->tBList);
  long double blocks = calcCompress(self->blocks_c->tBList);
  printf("First Block compression -> %Lg \n", block_1);
  printf("Average Compression of All Blocks -> %Lg \n", blocks);

  /* End time */
  Ticks[1] = clock();
  double time = (Ticks[1] - Ticks[0]) * 1000.0 / CLOCKS_PER_SEC;

  /* apresentar menu final */
  print_module_f(filename, self, blocks, time);

  free_Freq(freq_file);
  free_Blocks_file(self);

  return sucess;
}

/* pode se apagar antes de enviar */

int main() {
  /* chama module f */
  int arroz = module_f("ola.txt", 2048, 1);
  if (arroz != 1) {
    printf("algo deu mal\n");
  }
  return sucess;
}
