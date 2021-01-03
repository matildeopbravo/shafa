#include "data.h"
#include "dynamic_arrays.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *replace_str(char *str, char *orig, char *rep) {
  static char buffer[4096];
  char *p;

  if (!(p = strstr(str, orig)))
    return str;

  strncpy(buffer, str, p - str);
  buffer[p - str] = '\0';

  sprintf(buffer + (p - str), "%s%s", rep, p + strlen(orig));

  return buffer;
}

ByteVec *loadArray(FILE *file, size_t block_size) {
  size_t i = 0;
  ByteVec *self = byte_vec_new();
  uint8_t x = 0;
  while (i < block_size && (fread(&x, sizeof(x), 1, file) == 1)) {
    byte_vec_push(self, x);
    i++;
  }
  return self;
}

FreqBlock *initializeFreq(size_t array[uint_range]) {
  FreqBlock *e = (FreqBlock *)calloc(1, sizeof(FreqBlock));
  e->prox = NULL;
  size_t i;
  i = 0;
  for (; i < uint_range; i = i + 1)
    e->freq[i] = array[i];
  return e;
}

void free_Freq(FreqBlock *e) {
  FreqBlock *aux;
  while (e) {
    aux = e;
    e = e->prox;
    aux->prox = NULL;
    /* free(aux->freq); */
    free(aux);
  }
}

Blocks *initializeBlocks() {
  Blocks *e = (Blocks *)calloc(1, sizeof(Blocks));
  e->prox = NULL;
  e->block_size = 0;
  e->blocklist = NULL;
  return e;
}

void free_Blocks(Blocks *e) {
  Blocks *aux;
  while (e) {
    aux = e;
    byte_vec_del(e->blocklist);
    e = e->prox;
    aux->prox = NULL;
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
  while (e) {
    aux = e;
    tupple_vec_del(e->tBList);
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
  if (e->blocks)
    free_Blocks(e->blocks);
  if (e->blocks_c)
    free_Blocks_C(e->blocks_c);
  free(e);
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
void arrayToFreqBlock(size_t array[uint_range], FreqBlock *e) {
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

void print_freq(FreqBlock *freq) {
  FreqBlock *aux = freq;
  size_t i;
  while (aux) {
    for (i = 0; i < uint_range; i++)
      printf(" %d,", aux->freq[i]);
    printf("\n");
    aux = aux->prox;
  }
}

void printByteVec(ByteVec const *self) {
  size_t i = 0;
  size_t used = byte_vec_used(self);
  for (; i < used; i++)
    printf("%d ", byte_vec_index(self, i));
  printf("\n");
}

void printEqual(TuppleVec const *vec) {
  size_t counter = 0;
  size_t i = 0;
  for (; i < tupple_vec_used(vec); i++) {
    ByteTupple self = tupple_vec_index(vec, i);
    printf("%d,%d ", self.byte, self.count);
    counter = self.count + counter;
  }
  printf("\n %ld \n", counter);
}

size_t size_last_block_C_rle(Blocks_C *self) {
  Blocks_C *aux = self;
  while (aux->prox)
    aux = aux->prox;
  return aux->block_size;
}
