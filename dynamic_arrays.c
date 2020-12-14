#include "dynamic_arrays.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

size_t byte_vec_size(ByteVec const *self) { return self->size; }

size_t byte_vec_used(ByteVec const *self) { return self->used; }

uint8_t byte_vec_index(ByteVec const *self, size_t index) {
  assert(index < self->used);
  return self->vec[index];
}

uint8_t byte_vec_pop(ByteVec *self) {
  assert(self->used > 0);
  return self->vec[--self->used];
}

ByteVec byte_vec_new() {
  ByteVec vec;
  vec.size = 0;
  vec.used = 0;
  vec.vec = NULL;
  return vec;
}

void byte_vec_push(ByteVec *self, uint8_t v) {
  if (self->used == self->size) {
    self->size = self->size == 0 ? 1 : self->size * 2;
    self->vec = realloc(self->vec, self->size * sizeof(uint8_t));
  }
  self->vec[self->used] = v;
  self->used++;
}

void byte_vec_del(ByteVec *self) { free(self->vec); }
