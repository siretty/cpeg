#pragma once
#ifndef CP_STACK_H
#define CP_STACK_H 1

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>


#define CP_STACK_DEFINE(FULL_NAME,VALUE_TYPE) \
  CP_STACK_DEFINE_STRUCT(FULL_NAME,VALUE_TYPE) \
  CP_STACK_DEFINE_INIT(FULL_NAME,VALUE_TYPE) \
  CP_STACK_DEFINE_FREE(FULL_NAME,VALUE_TYPE) \
  CP_STACK_DEFINE_GET_SIZE(FULL_NAME,VALUE_TYPE) \
  CP_STACK_DEFINE_SET_SIZE(FULL_NAME,VALUE_TYPE) \
  CP_STACK_DEFINE_PUSH(FULL_NAME,VALUE_TYPE) \
  CP_STACK_DEFINE_POP(FULL_NAME,VALUE_TYPE)

#define CP_STACK_DEFINE_STRUCT(FULL_NAME,VALUE_TYPE) \
  struct FULL_NAME { \
    size_t capacity; \
    size_t size; \
    VALUE_TYPE *data; \
  };

#define CP_STACK_DEFINE_INIT(FULL_NAME,VALUE_TYPE) \
  static inline void FULL_NAME ## _init( struct FULL_NAME *s, size_t capacity ) { \
    assert( s ); assert( capacity > 0); \
    s->capacity = capacity; \
    s->size = 0; \
    s->data = malloc( sizeof( VALUE_TYPE ) * capacity ); \
  }

#define CP_STACK_DEFINE_FREE(FULL_NAME,VALUE_TYPE) \
  static inline void FULL_NAME ## _free( struct FULL_NAME *s ) { \
    assert( s ); \
    free( s->data ); \
  }

#define CP_STACK_DEFINE_GET_SIZE(FULL_NAME,VALUE_TYPE) \
  static inline size_t FULL_NAME ## _get_size( struct FULL_NAME *s ) { \
    assert( s ); \
    return s->size; \
  }

#define CP_STACK_DEFINE_SET_SIZE(FULL_NAME,VALUE_TYPE) \
  static inline void FULL_NAME ## _set_size( struct FULL_NAME *s, size_t n ) { \
    assert( s ); assert( n <= s->capacity ); \
    s->size = n; \
  }


#define CP_STACK_DEFINE_PUSH(FULL_NAME,VALUE_TYPE) \
  static inline void FULL_NAME ## _push( struct FULL_NAME *s, VALUE_TYPE v ) { \
    assert( s ); assert( s->data ); assert( s->size < s->capacity ); \
    s->data[ s->size ++ ] = v; \
  }

#define CP_STACK_DEFINE_POP(FULL_NAME,VALUE_TYPE) \
  static inline VALUE_TYPE FULL_NAME ## _pop( struct FULL_NAME *s ) { \
    assert( s ); assert( s->data ); assert( s->size > 0 ); \
    return s->data[ -- s->size ]; \
  }

#endif /* CP_STACK_H */
