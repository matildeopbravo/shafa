/**
 * @file module_f.c
 * @author Mariana Dinis Rodrigues e Mike
 * @date 28 Dezembro 2020
 * Responsible for compressing RLE and calculating frequencies.
 * @brief Responsavel pela compressao RLE e pelo calculo das frequencias.
 \brief Responsavel pela compressao RLE e pelo calculo das frequencias.
 */

#include "module_f.h"
#include "dynamic_arrays.h"
#include "fsize_.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

TuppleVec *compress(ByteVec const *self) {
  TuppleVec *t = tupple_vec_new();
  size_t i = 1, count = 1, used = self->used;
  uint8_t last = byte_vec_index(self, 0), las2 = 0;
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

int building_blocks(FILE *file, BlockFiles *self, size_t n_blocks,
                    size_t size_last_block, size_t block_size) {
  if (!file)
    return BUILDING_ERROR_IN_FILE;
  if (!self)
    self = initializeBlockFiles();

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
    a->block_size = list->block_size;
    addedBlock_CTOBloc_file(self, a);
    list = list->prox;
    i++;
  }
  self->compression_type = COMPRESSED;
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

FreqBlock *calFreq(BlockFiles const *file) {
  if (file) {
    Blocks *blocks = file->blocks;
    int num_blocks = file->num_blocks, num = 0;
    ByteVec *vec = file->blocks->blocklist;
    size_t i = 0;
    int array[uint_range];
    size_t used = byte_vec_used(vec);

    /* Vamos garantir que o array encontra-se todo a 0. */
    for (; i < uint_range; i++)
      array[i] = 0;

    /* 1 bloco */
    for (i = 0; i < used; i++)
      array[byte_vec_index(vec, i)]++;

    blocks = blocks->prox;
    num++;

    FreqBlock *freq = initializeFreq(array);
    /* Restantes Blocos. */
    while (num <= num_blocks && blocks) {
      vec = blocks->blocklist;
      used = byte_vec_used(vec);
      /* Vamos garantir que o array encontra-se todo a 0. */
      for (i = 0; i < uint_range; i++)
        array[i] = 0;

      /* Vamos preencher o array com as frequências de um bloco. */
      for (i = 0; i < byte_vec_used(vec); i++)
        array[byte_vec_index(vec, i)]++;

      /* Adicionar as frequencias ao nosso BlockFiles. */
      arrayToFreqBlock(array, freq);
      num++;
      blocks = blocks->prox;
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
    size_t i = 0, block_size = 0;
    int array[uint_range];
    /* Vamos garantir que o array encontra-se todo a 0. */
    for (; i < uint_range; i++)
      array[i] = 0;

    /*1 bloco*/
    ByteTupple aux;
    size_t used = tupple_vec_used(vec);
    size_t count, byte;

    for (i = 0; i < used; i++) {
      /* Variáveis auxiliares para n estramos sempre a ir à memória. */
      aux = tupple_vec_index(vec, i);
      count = aux.count;
      byte = aux.byte;

      array[byte]++;
      block_size++;
      /* Caso em que o nosso símbolo é o 0. */
      if (byte == 0) {
        array[0]++;
        array[count]++;
        block_size += 3;
      } else {
        /* Caso em que o nosso símbolo ocorre mais do que 3 vezes. */
        if (aux.count > 3) {
          array[0]++;
          array[count]++;
          block_size += 2;
        } else if (count == 2 || count == 3) {
          /* Caso em que o nosso símbolo ocorre entre 2 ou 3 vezes. */
          if (count == 3) {
            array[byte] += 2;
            block_size += 2;
          } else {
            block_size++;
            array[byte]++;
          }
        }
      }
    }
    /* Corrige o tamanho do nosso bloco analisado. */
    blocks->block_size = block_size;
    blocks = blocks->prox;
    num++;

    /* Inicializa a estrutura de dados das Frequências com o array obtido. */
    FreqBlock *freq = initializeFreq(array);
    /* Restantes blocos */
    while (num <= num_blocks && blocks) {
      block_size = 0;
      vec = blocks->tBList;
      used = tupple_vec_used(vec);

      /* Vamos garantir que o array encontra-se todo a 0. */
      for (i = 0; i < uint_range; i++)
        array[i] = 0;

      used = tupple_vec_used(vec);
      for (i = 0; i < used; i++) {
        /* Variáveis auxiliares para n estramos sempre a ir à memória. */
        aux = tupple_vec_index(vec, i);
        count = aux.count;
        byte = aux.byte;

        array[byte]++;
        block_size++;
        /* Caso em que o nosso símbolo é o 0. */
        if (byte == 0) {
          array[0]++;
          array[count]++;
          block_size += 3;
        } else {
          /* Caso em que o nosso símbolo ocorre mais do que 3 vezes. */
          if (aux.count > 3) {
            array[0]++;
            array[count]++;
            block_size += 2;
          } else if (count == 2 || count == 3) {
            /* Caso em que o nosso símbolo ocorre entre 2 ou 3 vezes. */
            if (count == 3) {
              array[byte] += 2;
              block_size += 2;
            } else {
              block_size++;
              array[byte]++;
            }
          }
        }
      }
      /* Adiciona o array das frequencias obtido na estrutura de dados. */
      arrayToFreqBlock(array, freq);
      num++;
      /* Corrige o tamanho do nosso bloco analisado. */
      blocks->block_size = block_size;
      blocks = blocks->prox;
    }
    return freq;
  } else
    return NULL;
}

int writeFreq(FILE *fp_in, const char *filename, BlockFiles *BlockFile,
              FreqBlock *freq) {

  long long n_blocks;

  /* Check if the BlockFile is not empty */
  /* Verifica se o BlockFile encontra-se vazio */
  if (BlockFile != NULL)
    n_blocks = BlockFile->num_blocks;
  else
    return WriteFreq_ERROR_IN_BLOCKFILES;

  /* Check if the frequency data is not empty */
  /* Verifica se a estrutura das Frequencias encontra-se vazio */
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

  /* Abrir o ficheiro recebido para leitura. */
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
    else
      block_size = aux_Blocks_C->block_size;

    /* @[tamanho_do_bloco]@ */
    fprintf(fp, "%c%ld%c", uint_Arroba, block_size, uint_Arroba);
    j = 0;
    /* Write the frequencies up to the symbol 255 */

    /* VER ISTOOOOOO*/
    /* int count = compression_type == 'N' ? (uint_range - 1) : (uint_range -
     * 2); */
    int count = uint_range - 1;
    while (j <= (count)) {
      num_freq = aux_Freq->freq[j];

      /* Verificar se o valor da frequência é igual ao do símbolo anterior. */
      if (last != num_freq) {
        fprintf(fp, "%d;", num_freq);
        last = num_freq;
      } else
        fprintf(fp, ";");

      j = j + 1;
    }
    i = i + 1;
    aux_Freq = aux_Freq->prox;
    aux_Blocks = aux_Blocks->prox;
    if (compression_type == 'R')
      aux_Blocks_C = aux_Blocks_C->prox;
    last = -1;
  }
  /* @0 */
  fprintf(fp, "%c0", uint_Arroba);
  if (aux_Freq == NULL && i != n_blocks)
    return WriteFreq_ERROR_IN_FREQ;

  fclose(fp);
  return sucess;
}

void print_module_f(const char *filename, BlockFiles const *self,
                    long double percentage_compression, double const time) {
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

int module_f(char const *filename, size_t const the_block_size,
             int FORCE_FLAG) {

  clock_t Ticks[2];
  /* Start time */
  Ticks[0] = clock();

  char filename_[3064];
  strcpy(filename_, filename);

  size_t x1 = 0;
  FILE *rfile, *wfile, *wfile_rle;

  /* Vamos abrir um file em modo binário com o char* recebido  */
  rfile = fopen(filename, "rb");
  if (!rfile)
    return Module_f_ERROR_IN_FILE;

  /* SEM COMPRESSAO */
  /* Iniciar a nossa estrutura de dados */
  BlockFiles *self = initializeBlockFiles();

  size_t n_blocks = 0, size_last_block = 0, block_size = the_block_size;

  FILE *fp1 = fopen(filename, "r");

  /* Aqui usamos  a função do professor com pequenas alterações. */
  n_blocks = fsize(fp1, filename, &block_size, &size_last_block);

  /* Lemos o file por blocos, colocando-o na nossa estrutura de dados */
  int error =
      building_blocks(rfile, self, n_blocks, size_last_block, the_block_size);
  /* Verificar se não houve ocorrência de erros na função executada
   * anteriormente. */
  if (error != 1)
    return Module_f_ERROR_IN_BLOCKFILES;
  else
    fclose(rfile);

  /* Iremos determinar a frequencia de cada bloco */
  FreqBlock *freq_file = calFreq(self);
  if (!freq_file)
    return Module_f_ERROR_IN_FREQ;

  /* Escrever as frequencias obtidas (sem compressao). */
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
    return Module_f_ERROR_IN_COMPRESSION;
  }

  char *filename_rle = filename_;
  strcat(filename_rle, ".rle");

  if (FORCE_FLAG || x1) {
    wfile_rle = fopen(filename_rle, "w");
    /* Escrever a compressão RLE num determinado ficheiro. */
    write_compressed(wfile_rle, self);

    /* Calcular as frequencias dos blocos comprimidos obtidos. */
    FreqBlock *freq_file_rle = calFreq_RLE(self);

    char *filename_rle_freq = filename_rle;
    strcat(filename_rle, ".freq");
    wfile_rle = fopen(filename_rle, "w");
    /* Imprimir as frequências obtidas dos blocos. */
    error = writeFreq(wfile, filename_rle_freq, self, freq_file_rle);
    if (error != 1)
      return Module_f_ERROR_IN_FILE;
    else
      free_Freq(freq_file_rle);
  }

  long double blocks = calcCompress_blocks(self);

  /* End time */
  Ticks[1] = clock();
  /* Calcular o tempo de execução do MODULE_F. */
  double time = (Ticks[1] - Ticks[0]) * 1000.0 / CLOCKS_PER_SEC;

  /* Apresentar menu final relativo ao módulo. */
  print_module_f(filename, self, blocks, time);

  /* Libertar o espaço alocado. */
  free_Freq(freq_file);
  free_Blocks_file(self);
  return sucess;
}

/* Apagar no final */
int main() {
  /* chama module f */
  int arroz = module_f("out.txt", 2048, 0);
  int ola = module_f("ola.txt", 2048, 0);
  if (arroz != 1 || ola != 1) {
    printf("algo deu mal\n");
  }
  return sucess;
}
