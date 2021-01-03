/**
 * @file dynamic_arrays.h
 * @author Alexandre Flores, Guilherme Fernandes, Maria Rita, Mariana Rodrigues,
  Matilde Bravo e Miguel Gomes.
 * @date 03 Janeiro 2021
 * @brief Estruturas de arrays dinámicos.
 */
#ifndef DYNAMIC_ARRAYS_H
#define DYNAMIC_ARRAYS_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

/**
 \brief Símbolos relativos ao bloco sem compressão.
*/
typedef struct ByteVec {
  uint8_t *vec; /**< Array dos símbolos. */
  size_t used;  /**< Número de elementos usados no array. */
  size_t size;  /**< Tamanho total do array. */
} ByteVec;

/**
 \brief Array de caracteres dinámico.
 */
typedef struct CharVec {
  char *vec;   /**< Array dos caracteres. */
  size_t used; /**< Número de elementos usados no array. */
  size_t size; /**< Tamanho total do array. */
} CharVec;

/**
 \brief Compressão de um símbolo.
 */
typedef struct ByteTupple {
  uint8_t byte;  /**< Símbolo. */
  uint8_t count; /**< Número de ocorrências. */
} ByteTupple;

/**
 \brief Símbolos relativos ao bloco com compressão.
*/
typedef struct TuppleVec {
  ByteTupple *vec; /**< Array dos símbolos. */
  size_t used;     /**< Número de elementos usados no array. */
  size_t n_used;   /**< Tamanho do array sem compressão. */
  size_t size;     /**< Tamanho total do array. */
} TuppleVec;

/**
 \brief Função que devolve o tamanho do array dinámico.
 */
size_t byte_vec_size(ByteVec const *);

/**
 \brief Função que devolve o número de elementos do array dinámico.
 */
size_t byte_vec_used(ByteVec const *);

/**
 \brief Função que returna o elemento que no index dado.
 */
uint8_t byte_vec_index(ByteVec const *, size_t);

/**
 \brief Função que elimina o último elemento do array.
 \return Elemento eliminado.
*/
uint8_t byte_vec_pop(ByteVec *);

/**
 \brief Inicializa uma array de caracteres dinámico.
 */
CharVec char_vec_new();

/**
 \brief Função que adiciona uma caracter ao array dinámico.
 */
void char_vec_push(CharVec *self, char c);

/**
 \brief Inicializa uma array de elementos dinámico.
 */
ByteVec *byte_vec_new();

/**
 \brief Função que adiciona o elemento dado ao array dinámico.
 */
void byte_vec_push(ByteVec *, uint8_t);

/**
 \brief Liberte o espaço alocado.
 */
void byte_vec_del(ByteVec *);

/**
 \brief Função que devolve o tamanho do array dinámico.
 */
size_t tupple_vec_size(TuppleVec const *);

/**
 \brief Função que devolve o número de elementos do array dinámico.
 */
size_t tupple_vec_used(TuppleVec const *);

/**
 \brief Função que returna o elemento que no index dado.
 */
ByteTupple tupple_vec_index(TuppleVec const *, size_t);

/**
 \brief Função que elimina o último elemento do array.
 \return Elemento eliminado.
 */
ByteTupple tupple_vec_pop(TuppleVec *);

/**
 \brief Inicializa uma array de elementos dinámico.
 */
TuppleVec *tupple_vec_new();

/**
 \brief Função que adiciona o elemento dado ao array dinámico.
 */
void tupple_vec_push(TuppleVec *, uint8_t, uint8_t);

/**
 \brief Liberte o espaço alocado.
 */
void tupple_vec_del(TuppleVec *);

#endif /* DYNAMIC_ARRAYS_H*/
