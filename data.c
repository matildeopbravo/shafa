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

FreqBlock *initializeFreq(int array[uint_range]) {
  FreqBlock *e = (FreqBlock *)calloc(1, sizeof(FreqBlock));
  e->prox = NULL;
  int i;
  i = 0;
  for (; i < uint_range; i = i + 1)
    e->freq[i] = array[i];
  return e;
}

void free_Freq(FreqBlock *e) {
  FreqBlock *aux;
  while (e) {
    aux = e;
    /* free(aux->freq); */
    free(aux);
    e = e->prox;
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
  ByteVec *aux_vec;
  while (e) {
    aux = e;
    aux_vec = e->blocklist;
    byte_vec_del(aux_vec);
    /* se der bug é por causa deste free */
    /* free(aux_vec); */
    e->blocklist = NULL;
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
    tupple_vec_del(aux_vec);
    /* se der bug é por causa deste free */
    free(e->tBList);
    e->tBList = NULL;
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
}

void print_freq(FreqBlock *freq) {
  FreqBlock *aux = freq;
  while (aux) {
    for (int i = 0; i < uint_range; i++)
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
  int counter = 0;
  size_t i = 0;
  for (; i < tupple_vec_used(vec); i++) {
    ByteTupple self = tupple_vec_index(vec, i);
    printf("%d,%d ", self.byte, self.count);
    counter = self.count + counter;
  }
  printf("\n %d \n", counter);
}
