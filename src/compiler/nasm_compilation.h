#ifndef COMPILATION_H
#define COMPILATION_H

#include "compiler.h"
#include "x86_64_specification.h"

typedef const char* Comment;

// TODO: add operations depending on a constant's size (e.g. add_r64_imm8)
// TODO: add add_rax_imm32, sub_rax_imm32, sal_r64_1

//===================================GENERAL====================================
//! @defgroup GENERAL Template instructions based on the operands' types.
//! @addtogroup GENERAL
//! @{

void write_instruction_r64_r64 (Compiler* compiler, Opcode opcode, Reg64 reg1, Reg64 reg2);
void write_jump_rel32          (Compiler* compiler, Opcode opcode, int32_t rel);

//! @}
//===================================GENERAL====================================


//====================================STACK=====================================
//! @defgroup STACK Stack instructions.
//! @addtogroup STACK
//! @{

static const Opcode OPCODE_BASE_PUSH_R64 = {.size = 1, .bytes = {0x50}};
static const Opcode OPCODE_BASE_POP_R64  = {.size = 1, .bytes = {0x58}};

void write_push_r64 (Compiler* compiler, Reg64 reg, Comment comment = nullptr);
void write_pop_r64  (Compiler* compiler, Reg64 reg, Comment comment = nullptr); 

//! @}
//====================================STACK=====================================


//===================================LOGICAL====================================
//! @defgroup LOGICAL Logical instructions.
//! @addtogroup LOGICAL
//! @{
    
static const Opcode OPCODE_TEST_R64_R64 = {.size = 1, .bytes = {0x85}};
static const Opcode OPCODE_XOR_R64_R64  = {.size = 1, .bytes = {0x31}};
static const Opcode OPCODE_CMP_R64_R64  = {.size = 1, .bytes = {0x39}};

void write_test_r64_r64 (Compiler* compiler, Reg64 reg1, Reg64 reg2, Comment comment = nullptr);
void write_xor_r64_r64  (Compiler* compiler, Reg64 reg1, Reg64 reg2, Comment comment = nullptr);
void write_cmp_r64_r64  (Compiler* compiler, Reg64 reg1, Reg64 reg2, Comment comment = nullptr);

//! @}
//===================================LOGICAL====================================


//=================================ARITHMETIC===================================
//! @defgroup ARITHMETIC Arithmetic instructions.
//! @addtogroup ARITHMETIC
//! @{

static const Opcode  OPCODE_ADD_R64_R64    = {.size = 1, .bytes = {0x01}      };
static const Opcode  OPCODE_ADD_R64_IMM32  = {.size = 1, .bytes = {0x81}      };
static const uint8_t OPCODE_ADD_EXTENSION  = 0b000;
static const Opcode  OPCODE_SUB_R64_R64    = {.size = 1, .bytes = {0x29}      };
static const Opcode  OPCODE_SUB_R64_IMM32  = {.size = 1, .bytes = {0x81}      };
static const uint8_t OPCODE_SUB_EXTENSION  = 0b101;
static const Opcode  OPCODE_IMUL_R64_R64   = {.size = 2, .bytes = {0x0F, 0xAF}};
static const Opcode  OPCODE_IDIV_R64       = {.size = 1, .bytes = {0xF7}      };
static const uint8_t OPCODE_IDIV_EXTENSION = 0b111;
static const Opcode  OPCODE_NEG_R64        = {.size = 1, .bytes = {0xF7}      };
static const uint8_t OPCODE_NEG_EXTENSION  = 0b011;
static const Opcode  OPCODE_SAL_R64_IMM8   = {.size = 1, .bytes = {0xC1}      };
static const uint8_t OPCODE_SAL_EXTENSION  = 0b100;

void write_add_r64_r64   (Compiler* compiler, Reg64 reg1, Reg64   reg2, Comment comment = nullptr);
void write_add_r64_imm32 (Compiler* compiler, Reg64 reg,  int32_t imm,  Comment comment = nullptr);
void write_sub_r64_r64   (Compiler* compiler, Reg64 reg1, Reg64   reg2, Comment comment = nullptr);
void write_sub_r64_imm32 (Compiler* compiler, Reg64 reg,  int32_t imm,  Comment comment = nullptr);
void write_imul_r64_r64  (Compiler* compiler, Reg64 reg1, Reg64   reg2, Comment comment = nullptr);
void write_idiv_r64      (Compiler* compiler, Reg64 reg,                Comment comment = nullptr);
void write_neg_r64       (Compiler* compiler, Reg64 reg,                Comment comment = nullptr);
void write_sal_r64_imm8  (Compiler* compiler, Reg64 reg,  int8_t  imm,  Comment comment = nullptr);

//! @}
//=================================ARITHMETIC===================================


//================================CONTROL_FLOW==================================
//! @defgroup CONTROL_FLOW Control flow instructions.
//! @addtogroup CONTROL_FLOW
//! @{

static const uint64_t SYSCALL_EXIT      = 0x3C;
static const Opcode   OPCODE_SYSCALL    = {.size = 2, .bytes = {0x0F, 0x05}};
static const Opcode   OPCODE_CALL_REL32 = {.size = 1, .bytes = {0xE8}};
static const Opcode   OPCODE_RET        = {.size = 1, .bytes = {0xC3}};
static const Opcode   OPCODE_JMP_REL32  = {.size = 1, .bytes = {0xE9}};
static const Opcode   OPCODE_JZ_REL32   = {.size = 2, .bytes = {0x0F, 0x84}};
static const Opcode   OPCODE_JE_REL32   = {.size = 2, .bytes = {0x0F, 0x84}};
static const Opcode   OPCODE_JNE_REL32  = {.size = 2, .bytes = {0x0F, 0x85}};
static const Opcode   OPCODE_JL_REL32   = {.size = 2, .bytes = {0x0F, 0x8C}};
static const Opcode   OPCODE_JG_REL32   = {.size = 2, .bytes = {0x0F, 0x8F}};
static const Opcode   OPCODE_JLE_REL32  = {.size = 2, .bytes = {0x0F, 0x8E}};
static const Opcode   OPCODE_JGE_REL32  = {.size = 2, .bytes = {0x0F, 0x8D}};
 
void write_syscall    (Compiler* compiler,              Comment comment = nullptr);
void write_call_rel32 (Compiler* compiler, Label label, Comment comment = nullptr);
void write_ret        (Compiler* compiler,              Comment comment = nullptr);
void write_jmp_rel32  (Compiler* compiler, Label label, Comment comment = nullptr);
void write_jz_rel32   (Compiler* compiler, Label label, Comment comment = nullptr);
void write_je_rel32   (Compiler* compiler, Label label, Comment comment = nullptr);
void write_jne_rel32  (Compiler* compiler, Label label, Comment comment = nullptr);
void write_jl_rel32   (Compiler* compiler, Label label, Comment comment = nullptr);
void write_jg_rel32   (Compiler* compiler, Label label, Comment comment = nullptr);
void write_jle_rel32  (Compiler* compiler, Label label, Comment comment = nullptr);
void write_jge_rel32  (Compiler* compiler, Label label, Comment comment = nullptr);

//! @}
//================================CONTROL_FLOW==================================


//====================================MOVE======================================
//! @defgroup MOVE Move instructions.
//! @addtogroup MOVE
//! @{

static const Opcode OPCODE_MOV_R64_R64        = {.size = 1, .bytes = {0x89}};
static const Opcode OPCODE_BASE_MOV_R64_IMM64 = {.size = 1, .bytes = {0xB8}};
static const Opcode OPCODE_MOV_M64_R64        = {.size = 1, .bytes = {0x89}};
static const Opcode OPCODE_MOV_R64_M64        = {.size = 1, .bytes = {0x8B}};

void write_mov_r64_r64   (Compiler* compiler, Reg64 dest, Reg64   src,   Comment comment = nullptr);
void write_mov_r64_imm64 (Compiler* compiler, Reg64 dest, int64_t imm,   Comment comment = nullptr);
void write_mov_r64_imm64 (Compiler* compiler, Reg64 dest, Label   label, Comment comment = nullptr);
void write_mov_m64_r64   (Compiler* compiler, Mem64 dest, Reg64   src,   Comment comment = nullptr);
void write_mov_r64_m64   (Compiler* compiler, Reg64 dest, Mem64   src,   Comment comment = nullptr);

//! @}
//====================================MOVE======================================

#endif