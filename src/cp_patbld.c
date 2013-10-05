#include "cp_patbld.h"

#include <stdlib.h>
#include <string.h>


#define DEFAULT_CAPACITY 2


struct cp_patbld {
  size_t ib_capacity;
  size_t ib_size;
  struct cp_ins *ib;

  size_t bb_capacity;
  size_t bb_size;
  struct cp_byteset *bb;
};


static void promise_size(
    void **object_buf, size_t *current_n, size_t wanted_n,
    size_t object_size )
{
  size_t new_n = *current_n;
  while( new_n < wanted_n ) new_n *= 2;

  if( new_n > *current_n ) {
    *current_n = new_n;
    void * p = realloc( *object_buf, *current_n * object_size );
    if( ! p ) abort( );
    *object_buf = p;
  }
}


struct cp_patbld * cp_patbld_create( ) {
  struct cp_patbld *s;

  s = malloc( sizeof( struct cp_patbld ) );

  s->ib_capacity = DEFAULT_CAPACITY;
  s->ib_size = 0;
  s->ib = malloc( s->ib_capacity * sizeof( struct cp_ins ) );

  s->bb_capacity = DEFAULT_CAPACITY;
  s->bb_size = 0;
  s->bb = malloc( s->bb_capacity * sizeof( struct cp_byteset ) );

  return s;
}

void cp_patbld_destroy( struct cp_patbld *s ) {
  assert( s );

  free( s->ib );
  free( s->bb );
  free( s );
}


void cp_patbld_export( struct cp_patbld *s,
    struct cp_ins **ib, size_t *ib_size,
    struct cp_byteset **bb, size_t *bb_size )
{
  assert( s ); assert( ib ); assert( ib_size ); assert( bb ); assert( bb_size );

  size_t byte_count;
 
  size_t ib_size_export = s->ib_size;
  if( s->ib[s->ib_size - 1].code != CP_CODE_END ) {
    ib_size_export ++;
  }

  byte_count = ib_size_export * sizeof( struct cp_ins );
  *ib_size = ib_size_export;
  *ib = malloc( byte_count );
  memcpy( *ib, s->ib, byte_count );

  byte_count = s->bb_size * sizeof( struct cp_byteset );
  *bb_size = s->bb_size;
  *bb = malloc( byte_count );
  memcpy( *bb, s->bb, byte_count );
}


size_t cp_patbld_app_ins( struct cp_patbld *s, struct cp_ins ins ) {
  assert( s );

  promise_size( (void **)(& s->ib), & s->ib_capacity, s->ib_size + 1,
      sizeof( struct cp_ins ) );

  s->ib[s->ib_size] = ins;

  return s->ib_size ++;
}

void cp_patbld_set_ins( struct cp_patbld *s, size_t idx, struct cp_ins ins ) {
  assert( s ); assert( idx >= 0 ); assert( idx < s->ib_size );

  s->ib[idx] = ins;
}

size_t cp_patbld_app_patbld( struct cp_patbld *s, struct cp_patbld *o ) {
  assert( s ); assert( o );

  promise_size( (void **)(& s->ib), & s->ib_capacity, s->ib_size + o->ib_size,
      sizeof( struct cp_ins ) );
  promise_size( (void **)(& s->bb), & s->bb_capacity, s->bb_size + o->bb_size,
      sizeof( struct cp_byteset ) );

  size_t size = o->ib_size;

  if( o->ib[size - 1].code == CP_CODE_END )
    size --;

  memmove( & s->ib[s->ib_size], & o->ib, size );

  s->ib_size += size;

  return size;
}


size_t cp_patbld_app_byteset( struct cp_patbld *s, struct cp_byteset b ) {
  assert( s );

  promise_size( (void **)(& s->bb), & s->bb_capacity, s->bb_size + 1,
      sizeof( struct cp_byteset ) );

  s->bb[s->bb_size] = b;

  return s->bb_size ++;
}


