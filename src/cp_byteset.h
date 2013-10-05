#pragma once
#ifndef CP_BYTESET_H
#define CP_BYTESET_H 1

#include <stdint.h>
#include <stdbool.h>


struct cp_byteset {
  uint64_t a, b, c, d;
};


static inline void cp_byteset_enable( struct cp_byteset *s, uint8_t b ) {
  if( b < 64 ) {
    s->a |= 1 << b;
  }
  else if( b < 128 ) {
    s->b |= 1 << (b - 64);
  }
  else if( b < 192 ) {
    s->c |= 1 << (b - 128);
  }
  else {
    s->d |= 1 << (b - 192);
  }
}

static inline void cp_byteset_disable( struct cp_byteset *s, uint8_t b ) {
  if( b < 64 ) {
    s->a &= ! (1 << b);
  }
  else if( b < 128 ) {
    s->b &= ! (1 << (b - 64));
  }
  else if( b < 192 ) {
    s->c &= ! (1 << (b - 128));
  }
  else {
    s->d &= ! (1 << (b - 192));
  }
}

static inline void cp_byteset_toggle( struct cp_byteset *s, uint8_t b ) {
  if( b < 64 ) {
    s->a ^= 1 << b;
  }
  else if( b < 128 ) {
    s->b ^= 1 << (b - 64);
  }
  else if( b < 192 ) {
    s->c ^= 1 << (b - 128);
  }
  else {
    s->d ^= 1 << (b - 192);
  }
}

static inline bool cp_byteset_check( struct cp_byteset *s, uint8_t b ) {
  if( b < 64 ) {
    return s->a & ((uint64_t)1 << b);
  }
  else if( b < 128 ) {
    return s->b & ((uint64_t)1 << (b - 64));
  }
  else if( b < 192 ) {
    return s->c & ((uint64_t)1 << (b - 128));
  }
  else {
    return s->d & ((uint64_t)1 << (b - 192));
  }
}

#endif /* CP_BYTESET_H */
