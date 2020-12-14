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

size_t byte_vec_size(ByteVec const *self);

size_t byte_vec_used(ByteVec const *self);

uint8_t byte_vec_index(ByteVec const *self, size_t index);

uint8_t byte_vec_pop(ByteVec *self);

ByteVec byte_vec_new();

void byte_vec_push(ByteVec *self, uint8_t v);

void byte_vec_del(ByteVec *self);

#endif /* DYNAMIC_ARRAYS_H*/