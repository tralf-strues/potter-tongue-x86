#ifndef NASM_COMPILATION_H
#define NASM_COMPILATION_H

#include "compiler.h"
#include "x86_64_specification.h"

// TODO: add operations depending on a constant's size (e.g. add_r64_imm8)
// TODO: add add_rax_imm32, sub_rax_imm32

//---------------------------------NASM_STACK-----------------------------------
//! @defgroup NASM_STACK Stack instructions in NASM
//! @addtogroup NASM_STACK
//! @{

void write_push_r64 (Compiler* compiler, Reg64 reg, const char* comment = nullptr);
void write_pop_r64  (Compiler* compiler, Reg64 reg, const char* comment = nullptr);

//! @}
//---------------------------------NASM_STACK-----------------------------------


//--------------------------------NASM_LOGICAL----------------------------------
//! @defgroup NASM_LOGICAL Logical instructions in NASM
//! @addtogroup NASM_LOGICAL
//! @{
    
void write_test_r64_r64 (Compiler* compiler, Reg64 firstReg, Reg64 secondReg, const char* comment = nullptr);
void write_xor_r64_r64  (Compiler* compiler, Reg64 firstReg, Reg64 secondReg, const char* comment = nullptr);
void write_cmp_r64_r64  (Compiler* compiler, Reg64 firstReg, Reg64 secondReg, const char* comment = nullptr);

//! @}
//--------------------------------NASM_LOGICAL----------------------------------


//------------------------------NASM_ARITHMETIC---------------------------------
//! @defgroup NASM_ARITHMETIC Arithmetic instructions in NASM
//! @addtogroup NASM_ARITHMETIC
//! @{

void write_add_r64_r64   (Compiler* compiler, Reg64 firstReg, Reg64 secondReg, const char* comment = nullptr);
void write_add_r64_imm32 (Compiler* compiler, Reg64 reg,      int32_t number,  const char* comment = nullptr);
void write_sub_r64_r64   (Compiler* compiler, Reg64 firstReg, Reg64 secondReg, const char* comment = nullptr);
void write_sub_r64_imm32 (Compiler* compiler, Reg64 reg,      int32_t number,  const char* comment = nullptr);
void write_imul_r64_r64  (Compiler* compiler, Reg64 firstReg, Reg64 secondReg, const char* comment = nullptr);
void write_idiv_r64      (Compiler* compiler, Reg64 reg,                       const char* comment = nullptr);
void write_neg_r64       (Compiler* compiler, Reg64 reg,                       const char* comment = nullptr);
void write_sal_r64_imm8  (Compiler* compiler, Reg64 reg,      int8_t number,   const char* comment = nullptr);

//! @}
//------------------------------NASM_ARITHMETIC---------------------------------


//-----------------------------NASM_CONTROL_FLOW--------------------------------
//! @defgroup NASM_CONTROL_FLOW Control flow instructions in NASM
//! @addtogroup NASM_CONTROL_FLOW
//! @{
 
void write_call_rel32 (Compiler* compiler, const char* label, int labelNum, const char* comment = nullptr);
void write_ret        (Compiler* compiler, const char* comment = nullptr);
void write_jmp_rel32  (Compiler* compiler, const char* label, int labelNum, const char* comment = nullptr);
void write_jz_rel32   (Compiler* compiler, const char* label, int labelNum, const char* comment = nullptr);
void write_je_rel32   (Compiler* compiler, const char* label, int labelNum, const char* comment = nullptr);
void write_jne_rel32  (Compiler* compiler, const char* label, int labelNum, const char* comment = nullptr);
void write_jl_rel32   (Compiler* compiler, const char* label, int labelNum, const char* comment = nullptr);
void write_jg_rel32   (Compiler* compiler, const char* label, int labelNum, const char* comment = nullptr);
void write_jle_rel32  (Compiler* compiler, const char* label, int labelNum, const char* comment = nullptr);
void write_jge_rel32  (Compiler* compiler, const char* label, int labelNum, const char* comment = nullptr);

//! @}
//-----------------------------NASM_CONTROL_FLOW--------------------------------


//---------------------------------NASM_MOVE------------------------------------
//! @defgroup NASM_MOVE Move instructions in NASM
//! @addtogroup NASM_MOVE
//! @{

void write_mov_r64_r64   (Compiler* compiler, Reg64 dest, Reg64 src,      const char* comment = nullptr);
void write_mov_r64_imm64 (Compiler* compiler, Reg64 dest, int64_t number, const char* comment = nullptr);
void write_mov_r64_imm64 (Compiler* compiler, Reg64 dest, const char* label, int labelNum, const char* comment = nullptr);
void write_mov_m64_r64   (Compiler* compiler, Mem64 dest, Reg64 src,      const char* comment = nullptr);
void write_mov_r64_m64   (Compiler* compiler, Reg64 dest, Mem64 src,      const char* comment = nullptr);

//! @}
//---------------------------------NASM_MOVE------------------------------------

#endif