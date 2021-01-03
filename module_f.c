/**
 * @file module_f.c
 * @author Mariana Dinis Rodrigues e Mike
 * @date 02 Janeiro 2021
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
    if (a.count > 3 || a.byte == 0) {
      r = r + 3;
    } else
      r += a.count;
  }
  b = (x - r);
  b = (b / x) * 100;
  if (b < 0)
    b = 0;
  return b;
}

TuppleVec *compress(ByteVec const *self) {
  TuppleVec *t = tupple_vec_new();
  size_t i = 1, count = 1, used = self->used;
  uint8_t last = byte_vec_index(self, 0);
  while (i < used && count < (uint_range - 1)) {
    /* if (last == self->vec[i]) { */
    if (last == byte_vec_index(self, i)) {
      count++;
      last = byte_vec_index(self, i);
    } else {
      tupple_vec_push(t, last, count);
      last = byte_vec_index(self, i);
      count = 1;
    }
    i++;
  }
  tupple_vec_push(t, last, count);
  t->n_used = self->used;
  return t;
}

size_t write_compressed(FILE *file, BlockFiles const *self) {
  uint8_t x, temp, y;
  ByteTupple b;
  Blocks_C *list = self->blocks_c;
  size_t used = 0, i;
  TuppleVec *yes;
  while (list) {
    yes = list->tBList;
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

size_t checkSum(ByteVec *self) {
  long double i = 0;
  TuppleVec *vec = tupple_vec_new();
  vec = compress(self);
  i = calcCompress(vec);
  tupple_vec_del(vec);
  if (i < 5)
    return 0;
  return 1;
}

size_t building_blocks(FILE *file, BlockFiles *self, size_t n_blocks,
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

size_t compress_blocks(BlockFiles *self, size_t FORCE_FLAG) {
  /* Blocks *list = initializeBlocks(); */
  Blocks *list;
  /* Blocks *list; */
  if (checkSum(self->blocks->blocklist) == 0 && !FORCE_FLAG) {
    return 0;
  }
  list = self->blocks;
  while (list) {
    Blocks_C *a = initializeBlocks_C();
    a->tBList = compress(list->blocklist);
    a->block_size = list->block_size;
    addedBlock_CTOBloc_file(self, a);
    list = list->prox;
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
    size_t num_blocks = file->num_blocks, num = 0;
    ByteVec *vec = file->blocks->blocklist;
    size_t i = 0;
    size_t array[uint_range];
    size_t used = byte_vec_used(vec);
    /* Vamos garantir que o array encontra-se todo a 0. */
    for (; i < uint_range; i++)
      array[i] = 0;
    /* 1 bloco */
    for (i = 0; i < used; i++)
      array[byte_vec_index(vec, i)]++;
    blocks = blocks->prox;
    num++;

    /* Inicializamos a nossa estrutura das frequências através do array obtido
     */
    FreqBlock *freq = initializeFreq(array);
    /* Restantes Blocos. */
    while (num <= num_blocks && blocks) {
      vec = blocks->blocklist;
      used = byte_vec_used(vec);
      /* Vamos garantir que o array encontra-se todo a 0. */
      for (i = 0; i < uint_range; i++)
        array[i] = 0;
      /* Vamos preencher o array com as frequências de um bloco. */
      for (i = 0; i < used; i++)
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

size_t TuppleVec_freq(TuppleVec *vec, size_t *block_size_,
                      size_t array[uint_range]) {
  if (vec) {
    ByteTupple aux;
    size_t i, used = tupple_vec_used(vec);
    size_t block_size = 0;
    size_t count, byte;
    /* Vamos garantir que o array encontra-se todo a 0. */
    for (i = 0; i < uint_range; i++)
      array[i] = 0;
    for (i = 0; i < used; i++) {
      /* Variáveis auxiliares para n estar-mos sempre a ir à memória. */
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
    *block_size_ = block_size;
    return sucess;
  } else
    return Module_f_ERROR_IN_FREQ;
}

FreqBlock *calFreq_RLE(BlockFiles *file) {
  if (file) {
    Blocks_C *blocks = file->blocks_c;
    size_t num_blocks = file->num_blocks, num = 0;
    TuppleVec *vec = file->blocks_c->tBList;
    size_t block_size = 0;
    size_t array[uint_range];

    if (sucess != TuppleVec_freq(vec, &block_size, array))
      return NULL;
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
      if (sucess != TuppleVec_freq(vec, &block_size, array))
        return NULL;

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

size_t writeFreq(FILE *fp, const char *filename, BlockFiles *BlockFile,
                 FreqBlock *freq) {
  size_t n_blocks;

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

  size_t i, j, last = -1;
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
  if (fp == NULL) {
    fp = fopen(filename, "r");
    if (fp == NULL)
      return WriteFreq_ERROR_IN_FILE;
  }

  /* ESCREVER:  */
  /* dados inicias */
  /* @<R|N>@NumerodeBlocos */
  fprintf(fp, "@%c@%ld", compression_type, n_blocks);
  /* write the frequency of a block */
  i = 0;
  size_t num_freq;
  while ((aux_Blocks || aux_Blocks_C) && aux_Freq && i < n_blocks) {
    if (compression_type == 'N')
      block_size = aux_Blocks->block_size;
    else
      block_size = aux_Blocks_C->block_size;
    /* @[tamanho_do_bloco]@ */
    fprintf(fp, "@%ld@", block_size);
    j = 0;
    /* Write the frequencies up to the symbol 255 */
    size_t count = uint_range - 1;
    /* Simbolo 0 ate ao 254 */
    while (j < (count)) {
      num_freq = aux_Freq->freq[j];
      /* Verificar se o valor da frequência é igual ao do símbolo anterior. */
      if (last != num_freq) {
        fprintf(fp, "%ld;", num_freq);
        last = num_freq;
      } else
        fprintf(fp, ";");
      j = j + 1;
    }
    /* Simbolo 255 */
    num_freq = aux_Freq->freq[j];
    /* Verificar se o valor da frequência é igual ao do símbolo anterior. */
    if (last != num_freq)
      fprintf(fp, "%ld", num_freq);
    i = i + 1;
    aux_Freq = aux_Freq->prox;
    aux_Blocks = aux_Blocks->prox;
    if (compression_type == 'R')
      aux_Blocks_C = aux_Blocks_C->prox;
    last = -1;
  }
  /* @0 */
  fprintf(fp, "@0");
  if (aux_Freq == NULL && i != n_blocks)
    return WriteFreq_ERROR_IN_FREQ;
  fclose(fp);
  return sucess;
}

void print_module_f(const char *filename, BlockFiles const *self,
                    long double percentage_compression, double const time,
                    size_t block_size, size_t size_last, size_t block_size_rle,
                    size_t size_last_rle) {
  size_t n_blocks = self->num_blocks;
  enum compression compression_type = self->compression_type;

  /* FALTAM COISAAAASSS !!! */
  /* so onde estao os pontos de interrogacao */
  fprintf(stdout, "\n");
  fprintf(stdout, "Mariana Rodrigues, a93329 && Mike, a \n");
  fprintf(stdout, "MIEI/CD 26-Dezembro-2020 \n");
  fprintf(stdout, "Módulo: f (Cálculo das frequências dos símbolos) \n");
  fprintf(stdout, "Número de blocos: %ld\n", n_blocks);
  fprintf(stdout, "Tamanho dos blocos analisados: %ld/%ld\n", size_last,
          block_size);
  if (compression_type == COMPRESSED) {
    fprintf(stdout, "Compressao RLE: %s.rle (%Lg %c compressão)\n", filename,
            percentage_compression, uint_percentagem);
    fprintf(stdout, "Tamanho dos blocos analisados no ficheiro RLE: %ld/%ld\n",
            size_last_rle, block_size_rle);
  }
  fprintf(stdout, "Tempo de execução do módulo (milissegundos): %f\n", time);
  fprintf(stdout, "Ficheiros gerados: %s.freq", filename);
  if (compression_type == COMPRESSED)
    fprintf(stdout, ", %s.rle, %s.rle.freq\n\n", filename, filename);
  else
    fprintf(stdout, "\n\n");
}

size_t module_f(char const *filename, size_t const the_block_size,
                size_t const FORCE_FLAG) {

  clock_t Ticks[2];
  /* Start time */
  Ticks[0] = clock();

  char filename_[3064];
  strcpy(filename_, filename);

  size_t x1 = 0;
  FILE *rfile, *wfile, *wfile_rle, *wfile_rle_freq;

  /* Vamos abrir um file em modo binário com o char* recebido  */
  rfile = fopen(filename, "rb");
  if (!rfile)
    return Module_f_ERROR_IN_FILE;

  /* SEM COMPRESSAO */
  /* Iniciar a nossa estrutura de dados */
  BlockFiles *self = initializeBlockFiles();
  size_t n_blocks = 0, size_last_block = 0, block_size = the_block_size;
  FILE *fp1 = fopen(filename, "rb");

  /* Aqui usamos  a função do professor com pequenas alterações. */
  n_blocks = fsize(fp1, filename, &block_size, &size_last_block);
  if (fp1)
    fclose(fp1);

  /* Lemos o file por blocos, colocando-o na nossa estrutura de dados */
  size_t error =
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
  /* error = writeFreq(wfile, filename_freq, self, freq_file); */
  error = writeFreq(wfile, filename_freq, self, freq_file);
  if (error != 1)
    return Module_f_ERROR_IN_FILE;

  /* RLE */
  x1 = compress_blocks(self, FORCE_FLAG);
  if (!x1 && FORCE_FLAG) {
    /* End time */
    Ticks[1] = clock();
    /* Calcular o tempo de execução do MODULE_F. */
    double time = (Ticks[1] - Ticks[0]) * 1000.0 / CLOCKS_PER_SEC;
    print_module_f(filename, self, 0, time, block_size, size_last_block, 0, 0);
    free_Freq(freq_file);
    free_Blocks_file(self);
    return Module_f_ERROR_IN_COMPRESSION;
  }

  /* char *filename_rle = filename_; */
  char filename_rle[3000];
  strcpy(filename_rle, filename_);
  strcat(filename_rle, ".rle");

  if (FORCE_FLAG || x1) {
    wfile_rle = fopen(filename_rle, "w");
    /* Escrever a compressão RLE num determinado ficheiro. */
    write_compressed(wfile_rle, self);
    /* Calcular as frequencias dos blocos comprimidos obtidos. */
    FreqBlock *freq_file_rle = calFreq_RLE(self);

    /* char *filename_rle_freq = filename_rle; */
    strcat(filename_rle, ".freq");
    wfile_rle_freq = fopen(filename_rle, "w");
    /* Imprimir as frequências obtidas dos blocos. */
    /* error = writeFreq(wfile, filename_rle, self, freq_file_rle); */
    error = writeFreq(wfile_rle_freq, filename_rle, self, freq_file_rle);
    if (error != 1)
      return Module_f_ERROR_IN_FILE;
    free_Freq(freq_file_rle);
  }

  long double blocks = calcCompress_blocks(self);

  /* End time */
  Ticks[1] = clock();
  /* Calcular o tempo de execução do MODULE_F. */
  double time = (Ticks[1] - Ticks[0]) * 1000.0 / CLOCKS_PER_SEC;

  /* Apresentar menu final relativo ao módulo. */
  size_t size_block_rle = self->blocks_c->block_size,
         size_last_rle = size_last_block_C_rle(self->blocks_c);

  print_module_f(filename, self, blocks, time, block_size, size_last_block,
                 size_block_rle, size_last_rle);
  /* Libertar o espaço alocado. */
  free_Freq(freq_file);
  free_Blocks_file(self);
  return sucess;
}

void argumentos_invalidos() {
  printf("\nDados inválidos:\n");
  printf("shafa file -m f -b (k|K|m|M) -c r\n");
}

size_t convert_block_size(char letter) {
  size_t block_size = M;
  switch (letter) {
  case 'k':
    block_size = k;
    break;
  case 'K':
    block_size = K;
    break;
  case 'm':
    block_size = m;
    break;
  case 'M':
    block_size = M;
    break;
  default:
    argumentos_invalidos();
    printf("O(s) tamanho(s) do(s) bloco(s) analisado(s) serão de %d\n\n", M);
    break;
  }
  return block_size;
}

void verificar_erro(size_t error) {
  switch (error) {
  case (-1):
    printf("Verifique se o ficheiro dado encontra-se correto\n");
    break;
  case (-2):
    printf("Erro ao escrever/ler as frequencias do ficheiro\n");
    break;
  case (-3):
    printf("Erro ao escrever/ler os blocos do ficheiro\n");
    break;
  case (-4):
    printf("Erro a comprimir o ficheiro\n");
    break;
  }
}

size_t call_module_f(char *filename, char *options[]) {
  size_t result = 0, block_size = k, FORCE_FLAG = 0;

  if (options && options[0][1] && options[1]) {
    switch (options[0][1]) {
    case 'b':
      block_size = convert_block_size(options[1][0]);

      if (options[2] && options[3]) {
        if (options[2][1] == 'c' && options[3][0] == 'r')
          FORCE_FLAG = 1;
        else
          argumentos_invalidos();
      }
      break;

    case 'c':
      if (options[1][0] == 'r') {
        FORCE_FLAG = 1;
        if (options[2]) {
          if (options[2][1] == 'b' && options[3]) {
            block_size = convert_block_size(options[3][0]);
          } else
            argumentos_invalidos();
        }
      } else
        argumentos_invalidos();

      break;

    default:
      argumentos_invalidos();
      break;
    }
  }
  /* block_size = 655360; */
  /* block_size = 2048; */
  /* block_size = 8388608; */
  result = module_f(filename, block_size, FORCE_FLAG);
  verificar_erro(result);
  return result;
}

/* Apagar no final */
/*   /\* int ola = module_f("oaaa.txt", 2048, 0); *\/ */
/*   int adeus = module_f("obbb.zip", 8388608, 0); */
/*   /\* int ate_logo = module_f("occc.txt", 655360, 0); *\/ */
