#include "src/cp_vm.h"

#include "src/cp_patbld.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
  

#define APP_INS(BLD,INS,...) \
    cp_patbld_app_ins( (BLD), \
        (struct cp_ins){ .code = CP_CODE_ ## INS, __VA_ARGS__ } )


struct cp_patbld * create_patbld_commas( ) {
  struct cp_patbld *b = cp_patbld_create( );

  /* NC <- [^,]* */
  struct cp_patbld *pm = cp_patbld_create( );

  struct cp_byteset bs_nc = cp_byteset_all;
  cp_byteset_disable( & bs_nc, ',' );

  uint16_t bs_nc_idx = cp_patbld_app_byteset( pm, bs_nc );

  APP_INS(pm, CHOICE, .l = +3 );
  APP_INS(pm, BYTESET, .m = bs_nc_idx );
  APP_INS(pm, COMMIT, .l = -2 );

  /* NC "," NC "," NC "," NC "," -[.;] [.;] */
  struct cp_byteset bs = {};
  cp_byteset_enable( & bs, '.' );
  cp_byteset_enable( & bs, ';' );

  uint16_t bs_idx = cp_patbld_app_byteset( b, bs );

  cp_patbld_app_patbld( b, pm );
  APP_INS(b, BYTE, .c = ',' );
  cp_patbld_app_patbld( b, pm );
  APP_INS(b, BYTE, .c = ',' );
  cp_patbld_app_patbld( b, pm );
  APP_INS(b, BYTE, .c = ',' );
  cp_patbld_app_patbld( b, pm );
  APP_INS(b, BYTE, .c = ',' );
  
  APP_INS(b, CHOICE, .l = +7 );
    APP_INS(b, CHOICE, .l = +4 );
    APP_INS(b, BYTESET, .m = bs_idx );
    APP_INS(b, COMMIT, .l = +1 );
    APP_INS(b, FAIL );
    APP_INS(b, ANY, .m = 1 );
  APP_INS(b, COMMIT, .l = -6 );

  APP_INS(b, BYTESET, .m = bs_idx );

  cp_patbld_destroy( pm );

  return b;
}

void create_subject_commas_invalid( char **sub, size_t *sub_size ) {
  size_t str_len = 80;
  char *str = malloc( str_len );
  for( size_t i = 0; i < str_len; i ++ )
    str[i] = ',';
  *sub = str;
  *sub_size = str_len;
}

void create_subject_commas_valid( char **sub, size_t *sub_size ) {
  const char * str ="part 1, part 2 ,part 3, part 4 , part 5 ;";
  size_t str_len = strlen( str );

  *sub = strdup( str );
  *sub_size = str_len;
}

struct cp_patbld * test_pattern( ) {
  struct cp_byteset bs_alpha;
  cp_byteset_enable( & bs_alpha, '_' );
  for( int c = 'a'; c < 'z'; c ++ ) cp_byteset_enable( & bs_alpha, (uint8_t)c );
  for( int c = 'A'; c < 'z'; c ++ ) cp_byteset_enable( & bs_alpha, (uint8_t)c );

  struct cp_byteset bs_alnum = bs_alpha;
  for( int c = '0'; c < '9'; c ++ ) cp_byteset_enable( & bs_alnum, (uint8_t)c );

  struct cp_byteset bs_special;
  cp_byteset_enable( & bs_special, '.' );
  cp_byteset_enable( & bs_special, '?' );
  cp_byteset_enable( & bs_special, '!' );
  
  struct cp_patbld *pb = cp_patbld_create( );

  uint16_t bs_special_idx = cp_patbld_app_byteset( pb, bs_special );

  APP_INS( pb, ANY,     .m = 3 );
  APP_INS( pb, BYTE,    .c = 'l' );
  APP_INS( pb, CHOICE,  .l = +3 );
  APP_INS( pb, BYTE,    .c = 'i' );
  APP_INS( pb, COMMIT,  .l = +2 );
  APP_INS( pb, BYTE,    .c = 'o' );
  APP_INS( pb, BYTE,    .c = ' ' );
  APP_INS( pb, CALL,    .l = +2 );
  APP_INS( pb, JUMP,    .l = +7 );
  APP_INS( pb, BYTE,    .c = 'm' );
  APP_INS( pb, BYTE,    .c = 'e' );
  APP_INS( pb, BYTE,    .c = 'i' );
  APP_INS( pb, BYTE,    .c = 'n' );
  APP_INS( pb, BYTE,    .c = 'e' );
  APP_INS( pb, RETURN );
  APP_INS( pb, BYTE,    .c = ' ' );
  APP_INS( pb, CHOICE,  .l = +7 );
  APP_INS( pb, BYTE,    .c = 'L' );
  APP_INS( pb, BYTE,    .c = 'i' );
  APP_INS( pb, BYTE,    .c = 'e' );
  APP_INS( pb, BYTE,    .c = 'b' );
  APP_INS( pb, BYTE,    .c = 'e' );
  APP_INS( pb, COMMIT,  .l = +8 );
  APP_INS( pb, BYTE,    .c = 'L' );
  APP_INS( pb, BYTE,    .c = 'i' );
  APP_INS( pb, BYTE,    .c = 'e' );
  APP_INS( pb, BYTE,    .c = 'b' );
  APP_INS( pb, BYTE,    .c = 's' );
  APP_INS( pb, BYTE,    .c = 't' );
  APP_INS( pb, BYTE,    .c = 'e' );
  APP_INS( pb, BYTESET, .m = bs_special_idx );
  APP_INS( pb, CHOICE,  .l = +4 );
  APP_INS( pb, ANY,     .m = 1 );
  APP_INS( pb, COMMIT,  .l = +1 );
  APP_INS( pb, FAIL );

  return pb;
}


int main( int argc, char **argv ) {
  (void) argc; (void) argv;

  struct cp_vm * vm = cp_vm_create( );

  struct cp_patbld *pb = create_patbld_commas( );
  char * sub; size_t sub_size;
  create_subject_commas_valid( & sub, & sub_size );

  size_t ib_size, bb_size;
  struct cp_ins * ib;
  struct cp_byteset *bb;

  cp_patbld_export( pb, & ib, & ib_size, & bb, & bb_size );
  cp_patbld_destroy( pb );

  cp_vm_set_pattern( vm, ib, ib_size, bb, bb_size );
  cp_vm_set_subject( vm, sub, sub_size );

  if( ! cp_vm_run( vm ) ) {
    fprintf( stdout, "pattern failed\n" );
  }
  
  free( sub );

  cp_vm_destroy( vm );
}

