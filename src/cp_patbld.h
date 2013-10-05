#pragma once
#ifndef CP_PATBLD_H
#define CP_PATBLD_H 1

#include "cp_ins.h"
#include "cp_byteset.h"

#include <stddef.h>
#include <stdint.h>


struct cp_patbld;


/**
 * \brief creates a struct cp_patbld object.
 *
 * \return created object
 */
struct cp_patbld * cp_patbld_create( );

/**
 * \brief destroy a struct cp_patbld object.
 *
 * \param the struct cp_patbld object to destroy
 */
void cp_patbld_destroy( struct cp_patbld *s );


/**
 * \brief coies the internal buffers and returns them to you.
 *
 * automatically appends an END instruction if it wasn't appended before.
 *
 * \param s [INOUT] the struct cp_patbld object
 * \param ib [OUT] the instruction buffer
 * \param ib_size [OUT] the instruction buffer size
 * \param bb the [OUT] byteset buffer
 * \param bb_size [OUT] the byteset buffer size
 */
void cp_patbld_export( struct cp_patbld *s,
    struct cp_ins **ib, size_t *ib_size,
    struct cp_byteset **bb, size_t *bb_size );


/**
 * \brief append an instruction.
 *
 * \param s   [INOUT] the struct cp_patbld object
 * \param ins [IN] the instruction to append
 *
 * \return index of the appended instruction
 */
size_t cp_patbld_app_ins( struct cp_patbld *s, struct cp_ins ins );

/**
 * \brief set instruction at the specified index.
 *
 * \param s   [INOUT] the struct cp_patbld object
 * \param idx [IN] the index of the instruction to set
 * \param ins [IN] the instruction to append
 *
 * \return index of the appended instruction
 */
void cp_patbld_set_ins( struct cp_patbld *s, size_t idx, struct cp_ins ins );

/**
 * \brief append another pattern builder.
 *
 * This function appends the contents of another pattern builder. It does _not_
 * copy the END instruction from the other pattern.
 *
 * \param s [INOUT] the struct cp_patbld object
 * \param o [IN] the other struct cp_patbld object
 *
 * \return number of appended instructions
 */
size_t cp_patbld_app_patbld( struct cp_patbld *s, struct cp_patbld *o );

/**
 * \brief append a byteset.
 *
 * \param s [INOUT] the struct cp_patbld object
 * \param b [IN] the byteset to append
 *
 * \return index of the appended byteset
 */
size_t cp_patbld_app_byteset( struct cp_patbld *s, struct cp_byteset b );


#endif /* CP_PATBLD_H */
