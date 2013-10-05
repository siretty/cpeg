#pragma once
#ifndef CP_INSTR_H
#define CP_INSTR_H 1

#include <assert.h>
#include <stdint.h>


/**
 * \brief contains an instruction.
 */
struct cp_ins {
  uint8_t code;
  struct {
    uint8_t c;
    int16_t l;
    int16_t m;
  };
};

enum cp_code {
  CP_CODE_NO_CODE = 0,

  CP_CODE_END,
  CP_CODE_ANY,
  CP_CODE_BYTE,
  CP_CODE_BYTESET,

  CP_CODE_CHOICE,
  CP_CODE_COMMIT,
  CP_CODE_FAIL,

  CP_CODE_JUMP,

  CP_CODE_CALL,
  CP_CODE_RETURN,

  CP_CODE_MAX_CODE,
};

#endif /* CP_INSTR_H */
