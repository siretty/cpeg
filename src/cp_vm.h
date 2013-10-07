#pragma once
#ifndef CP_VM_H
#define CP_VM_H 1

#include "cp_ins.h"
#include "cp_byteset.h"

#include <stddef.h>


struct cp_vm;


/**
 * \brief create a struct cp_vm object.
 *
 * \return the created struct cp_vm object
 */
struct cp_vm * cp_vm_create( );

/**
 * \brief destroy a struct cp_vm objct.
 *
 * \param s [INOUT] the struct cp_vm object to destroy
 */
void cp_vm_destroy( struct cp_vm *s );


/**
 * \brief set the pattern to match.
 *
 * The buffers are NOT copied and NOT destroyed, when calling cp_vm_destroy.
 *
 * \param s [INOUT] the struct cp_vm object
 * \param ib [IN] pattern buffer
 * \param ib_size [IN] size of the pattern
 * \param bb [IN] byteset buffer
 * \param bb_size [IN] size of the byteset buffer
 */
void cp_vm_set_pattern( struct cp_vm *s,
    struct cp_ins *ib, size_t ib_size,
    struct cp_byteset *bb, size_t bb_size );

/**
 * \brief set the subject.
 *
 * The buffers are NOT copied and NOT destroyed, when calling cp_vm_destroy.
 *
 * \param s [INOUT] the struct cp_vm object
 * \param sub [IN] subject
 * \param sub_size [IN] size of the subject
 */
void cp_vm_set_subject( struct cp_vm *s, const char *sub, size_t sub_size );


/**
 * \brief run the vm with the previously set pattern and subject.
 *
 * \param s [INOUT] the struct cp_vm object
 *
 * \return true if the match succeeded, false if it failed or an internal
 *         error occured.
 */
bool cp_vm_run( struct cp_vm *s );

#endif /* CP_VM_H */
