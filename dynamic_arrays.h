#ifndef DYNAMIC_ARRAYS_H
#define DYNAMIC_ARRAYS_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct ByteVec {
  uint8_t *vec;
  size_t used;
  size_t size;
} ByteVec;

typedef struct ByteTupple {
  uint8_t byte;
  uint8_t count;
} ByteTupple;

typedef struct TuppleVec {
  ByteTupple *vec;
  size_t used;
  size_t n_used;
  size_t size;
} TuppleVec;

size_t byte_vec_size(ByteVec const *);

size_t byte_vec_used(ByteVec const *);

uint8_t byte_vec_index(ByteVec const *, size_t);

uint8_t byte_vec_pop(ByteVec *);

ByteVec *byte_vec_new();

void byte_vec_push(ByteVec *, uint8_t);

void byte_vec_del(ByteVec *);

size_t tupple_vec_size(TuppleVec const *);

size_t tupple_vec_used(TuppleVec const *);

ByteTupple tupple_vec_index(TuppleVec const *, size_t);

ByteTupple tupple_vec_pop(TuppleVec *);

TuppleVec *tupple_vec_new();

void tupple_vec_push(TuppleVec *, uint8_t, uint8_t);

void tupple_vec_del(TuppleVec *);

#endif /* DYNAMIC_ARRAYS_H*/
