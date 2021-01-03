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
CharVec char_vec_new() { return (CharVec){.vec = NULL, .size = 0, .used = 0}; }

void char_vec_push(CharVec *self, char c) {
  if (self->used == self->size) {
    self->size = self->size == 0 ? 1 : self->size * 2;
    self->vec = realloc(self->vec, self->size * sizeof(char));
  }
  self->vec[self->used] = c;
  self->used++;
}

void char_vec_del(Charvec *self) {
  free(self->vec);

}

ByteVec *byte_vec_new() {
  ByteVec *vec = (ByteVec *)calloc(1, sizeof(ByteVec));
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

void byte_vec_del(ByteVec *self) {
  free(self->vec);
  free(self);
}

size_t tupple_vec_size(TuppleVec const *self) { return self->size; }

size_t tupple_vec_used(TuppleVec const *self) { return self->used; }

ByteTupple tupple_vec_index(TuppleVec const *self, size_t index) {
  assert(index < self->used);
  return self->vec[index];
}

ByteTupple tupple_vec_pop(TuppleVec *self) {
  assert(self->used > 0);
  return self->vec[--self->used];
}

TuppleVec *tupple_vec_new() {
  TuppleVec *vec = (TuppleVec *)calloc(1, sizeof(TuppleVec));
  return vec;
}

void tupple_vec_push(TuppleVec *self, uint8_t b, uint8_t c) {
  if (self->used == self->size) {
    self->size = self->size == 0 ? 1 : self->size * 2;
    self->vec = realloc(self->vec, self->size * sizeof(struct ByteTupple));
  }
  self->vec[self->used].byte = b;
  self->vec[self->used].count = c;
  self->used++;
}

void tupple_vec_del(TuppleVec *self) {
  free(self->vec);
  free(self);
}
