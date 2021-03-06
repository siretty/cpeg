#include "cp_vm.h"

#include "cp_stack.h"

#include <assert.h>
#include <stdbool.h>


#ifdef DEBUG
static void cp_debug( const char *fmt, ... )
  __attribute__(( format( printf, 1, 2) ));
#else
static void cp_debug( const char *fmt, ... )
{
  (void) fmt;
}
#endif


struct cp_back {
  size_t ins_idx;
  size_t sub_idx;

  size_t reta_size;
};

CP_STACK_DEFINE(st_back,struct cp_back);
CP_STACK_DEFINE(st_reta,size_t);


struct cp_vm {
  bool fail;
  size_t ins_idx;
  size_t sub_idx;
  
  size_t ib_size;
  struct cp_ins *ib;

  size_t bb_size;
  struct cp_byteset *bb;

  size_t sub_size;
  const char * sub;

  struct st_reta reta;
  struct st_back back;
};

static inline
bool cp_vm_backtrack( struct cp_vm *s ) {
  assert( s->fail );

  /* no backtrack entries: pattern fails */
  if( st_back_get_size( & s->back ) == 0 ) {
    return false;
  }

  /* fetch the backtrack entry */
  struct cp_back b = st_back_pop( & s->back );

  cp_debug( "BAC I:%03zu S:%03zu -> I:%03zu S:%03zu\n", s->ins_idx, s->sub_idx,
      b.ins_idx, b.sub_idx );

  /* reset the return address stack */
  st_reta_set_size( & s->reta, b.reta_size );

  /* reset indices */
  s->ins_idx = b.ins_idx;
  s->sub_idx = b.sub_idx;

  /* reset fail state */
  s->fail = false;

  return true;
}


static inline
bool cp_vm_ins_end( struct cp_vm *s ) {
  (void) s;

  return false;
}

static inline
bool cp_vm_ins_any( struct cp_vm *s ) {
  struct cp_ins i = s->ib[s->ins_idx];

  int16_t num = i.m;

  assert( num > 0 );

  if( (long int)(s->sub_size - s->sub_idx) < num ) {
    /* if not enough characters are left, set fail state */
    s->fail = true;
  }
  else {
    cp_debug( "MAT S:%03zu \"%.*s\"\n", s->sub_idx, num, &s->sub[s->sub_idx] );

    /* advance subject index by the number of bytes to match */
    s->sub_idx += num;
    /* advance instruction index by one */
    s->ins_idx ++;
  }

  return true;
}

static inline
bool cp_vm_ins_byte( struct cp_vm *s ) {
  struct cp_ins i = s->ib[s->ins_idx];

  uint8_t chr = i.c;

  if( s->sub_size - s->sub_idx < 1 ) {
    s->fail = true;
  }
  else {
    if( s->sub[s->sub_idx] == chr ) {
      cp_debug( "MAT S:%03zu '%c'\n", s->sub_idx, s->sub[s->sub_idx] );

      s->sub_idx ++;
      s->ins_idx ++;
    }
    else {
      s->fail = true;
    }
  }

  return true;
}

static inline
bool cp_vm_ins_byteset( struct cp_vm *s ) {
  struct cp_ins i = s->ib[s->ins_idx];

  int16_t key = i.m;

  assert( key >= 0 );

  if( s->sub_size - s->sub_idx < 1 ) {
    s->fail = true;
  }
  else {
    if( cp_byteset_check( & s->bb[key], s->sub[s->sub_idx] ) ) {
      cp_debug( "MAT S:%03zu '%c'\n", s->sub_idx, s->sub[s->sub_idx] );

      s->sub_idx ++;
      s->ins_idx ++;
    }
    else {
      s->fail = true;
    }
  }

  return true;
}

static inline
bool cp_vm_ins_choice( struct cp_vm *s ) {
  struct cp_ins i = s->ib[s->ins_idx];

  int ins_idx = (int)s->ins_idx + i.l;

  assert( ins_idx >= 0 ); assert( ins_idx < (int)s->ib_size );

  struct cp_back back = {
    .ins_idx = ins_idx,
    .sub_idx = s->sub_idx,
    .reta_size = st_reta_get_size( & s->reta ),
  };

  st_back_push( & s->back, back );
  s->ins_idx ++;

  return true;
}

static inline
bool cp_vm_ins_commit( struct cp_vm *s ) {
  struct cp_ins i = s->ib[s->ins_idx];

  int ins_idx = (int)s->ins_idx + i.l;

  assert( ins_idx >= 0 ); assert( ins_idx < (int)s->ib_size );

  st_back_pop( & s->back );
  s->ins_idx = ins_idx;
  
  return true;
}

static inline
bool cp_vm_ins_fail( struct cp_vm *s ) {
  s->fail = true;

  return true;
}

static inline
bool cp_vm_ins_jump( struct cp_vm *s ) {
  struct cp_ins i = s->ib[s->ins_idx];

  int ins_idx = (int)s->ins_idx + i.l;

  assert( ins_idx >= 0 ); assert( ins_idx < (int)s->ib_size );

  s->ins_idx = ins_idx;

  return true;
}

static inline
bool cp_vm_ins_call( struct cp_vm *s ) {
  struct cp_ins i = s->ib[s->ins_idx];

  int ins_idx = (int)s->ins_idx + i.l;

  assert( ins_idx >= 0 ); assert( ins_idx < (int)s->ib_size );

  st_reta_push( & s->reta, s->ins_idx + 1 );
  s->ins_idx = ins_idx;

  return true;
}

static inline
bool cp_vm_ins_return( struct cp_vm *s ) {
  s->ins_idx = st_reta_pop( & s->reta );

  return true;
}


struct {
  const char *name;
}
ins_desc[256] = {
  [CP_CODE_NO_CODE] = { "INVALID (NO_CODE)" },

  [CP_CODE_END] =     { "END" },
  [CP_CODE_ANY] =     { "ANY" },
  [CP_CODE_BYTE] =    { "BYTE" },
  [CP_CODE_BYTESET] = { "BYTESET" },
  [CP_CODE_CHOICE] =  { "CHOICE" },
  [CP_CODE_COMMIT] =  { "COMMIT" },
  [CP_CODE_FAIL] =    { "FAIL" },
  [CP_CODE_JUMP] =    { "JUMP" },
  [CP_CODE_CALL] =    { "CALL" },
  [CP_CODE_RETURN] =  { "RETURN" },

  [CP_CODE_MAX_CODE] = { "INVALID (MAX_CODE)" },
};


struct cp_vm * cp_vm_create( ) {
  struct cp_vm *s = malloc( sizeof( struct cp_vm ) );

  s->ib = NULL;
  s->ib_size = 0;
  s->bb = NULL;
  s->bb_size = 0;
  s->sub = NULL;
  s->sub_size = 0;
  
  st_reta_init( & s->reta, 128 );
  st_back_init( & s->back, 128 );

  return s;
}

void cp_vm_destroy( struct cp_vm *s ) {
  st_back_free( & s->back );
  st_reta_free( & s->reta );

  free( s );
}

void cp_vm_set_pattern( struct cp_vm *s,
    struct cp_ins *ib, size_t ib_size,
    struct cp_byteset *bb, size_t bb_size )
{
  assert( s ); assert( ib ); assert( ib_size > 0 );

  s->ib = ib;
  s->ib_size = ib_size;
  s->bb = bb;
  s->bb_size = bb_size;
}

void cp_vm_set_subject( struct cp_vm *s, const char *sub, size_t sub_size ) {
  assert( s ); assert( sub );

  s->sub = sub;
  s->sub_size = sub_size;
}


bool cp_vm_run( struct cp_vm *s ) {
  assert( s );
  assert( s->sub );
  assert( s->ib ); assert( s->ib_size > 0 );

  s->fail = false;
  s->ins_idx = 0;
  s->sub_idx = 0;

  bool run = true;

  do {
#ifdef DEBUG
    struct cp_ins dbg_ins = s->ib[s->ins_idx];
    size_t dbg_ins_idx_old = s->ins_idx;
    size_t dbg_sub_idx_old = s->sub_idx;
#endif

    switch( s->ib[s->ins_idx].code ) {
    case CP_CODE_END:
      run = cp_vm_ins_end( s );
      break;
    case CP_CODE_ANY:
      run = cp_vm_ins_any( s );
      break;
    case CP_CODE_BYTE:
      run = cp_vm_ins_byte( s );
      break;
    case CP_CODE_BYTESET:
      run = cp_vm_ins_byteset( s );
      break;
    case CP_CODE_CHOICE:
      run = cp_vm_ins_choice( s );
      break;
    case CP_CODE_COMMIT:
      run = cp_vm_ins_commit( s );
      break;
    case CP_CODE_FAIL:
      run = cp_vm_ins_fail( s );
      break;
    case CP_CODE_JUMP:
      run = cp_vm_ins_jump( s );
      break;
    case CP_CODE_CALL:
      run = cp_vm_ins_call( s );
      break;
    case CP_CODE_RETURN:
      run = cp_vm_ins_return( s );
      break;
    default:
      abort( );
    }

#ifdef DEBUG
    cp_debug( "INS I:%03zu S:%03zu -> I:%03zu S:%03zu %c %10s (%hhx)\n",
        dbg_ins_idx_old, dbg_sub_idx_old, s->ins_idx, s->sub_idx,
        s->fail ? 'F' : ' ', ins_desc[dbg_ins.code].name, dbg_ins.code );
#endif
    
    if( s->fail ) {
      run = cp_vm_backtrack( s );
    }
  }
  while( run );

  return ! s->fail;
}


#include <stdio.h>

#ifdef DEBUG
static void cp_debug( const char *fmt, ... )
{
  va_list va;
  va_start( va, fmt );
  vfprintf( stderr, fmt, va );
  va_end( va );
  fflush( stderr );
}
#endif


