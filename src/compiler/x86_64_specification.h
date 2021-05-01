#ifndef X86_64_SPECIFICATION_H
#define X86_64_SPECIFICATION_H

#include <stdlib.h>
#include <stdint.h>

enum Reg64
{
    RAX,
    RCX,
    RDX,
    RBX,
    RSP,
    RBP,
    RSI,
    RDI,

    R8,
    R9,
    R10,
    R11,
    R12,
    R13,
    R14,
    R15,

    TOTAL_REGISTERS_64,
    INVALID_REG64 = -1
};

struct Mem64
{
    /* Any general-purpose register or INVALID_REG64 to not use. */
    Reg64 base;

    /* Any 32-bit number (if 0 then isn't used). */
    int32_t displacement;

    /* Any general-purpose register or INVALID_REG64 to not use. */
    Reg64 index;

    /* Either 2, 4 or 8 (otherwise isn't used). */
    uint8_t scale;
};

static const size_t OPCODE_MAX_SIZE = 3;

//------------------------------------------------------------------------------
//! Specifies what instruction to use. Can be either 1, 2 or 3 bytes in size.
//------------------------------------------------------------------------------
struct Opcode
{
    uint8_t size;
    uint8_t bytes[OPCODE_MAX_SIZE]; 
};

static const size_t REX_ID_SIZE = 4;
static const size_t REX_W_SIZE  = 1;
static const size_t REX_R_SIZE  = 1;
static const size_t REX_X_SIZE  = 1;
static const size_t REX_B_SIZE  = 1;

//------------------------------------------------------------------------------
//! Instruction prefix that is used (mostly; see Intel's reference for more
//! info on the subject) whenever one needs to use 64-bit operand size and the 
//! instruction doesn't default to it.
//------------------------------------------------------------------------------
struct REX
{
    /* The first four bits are a fixed bit pattern equal to 0b0100.
     * They indicate the REX byte. */
    uint8_t id : REX_ID_SIZE; // constant 0b0100

    /* When 1, a 64-bit operand size is used. 
     * Otherwise, when 0, the default operand size is used. */
    uint8_t w : REX_W_SIZE;

    /* Extension to the MODRM.reg field. */
    uint8_t r : REX_R_SIZE;

    /* Extension to the SIB.index field. */
    uint8_t x : REX_X_SIZE;

    /* Extension to the MODRM.rm field or the SIB.base field. */
    uint8_t b : REX_B_SIZE;
};

static const uint8_t REX_ID      = 0b0100;
static const REX     DEFAULT_REX = {.id = REX_ID, .w = 0, .r = 0, .x = 0, .b = 0};

static const size_t MODRM_MOD_SIZE = 2;
static const size_t MODRM_REG_SIZE = 3;
static const size_t MODRM_RM_SIZE  = 3;

//------------------------------------------------------------------------------
//! Many instructions that refer to an operand in memory have an addressing-form 
//! specifier byte (called the ModR/M byte) following the primary opcode.
//------------------------------------------------------------------------------
struct ModRM
{
    /* Combines with the r/m field to form 32 possible values: 
     * eight registers and 24 addressing modes. */
    uint8_t mod : MODRM_MOD_SIZE;

    /* Specifies either a register number or three more bits 
     * of opcode information. The purpose of the field is 
     * specified in the primary opcode. */
    uint8_t reg : MODRM_REG_SIZE;

    /* The r/m field can specify a register as an operand or 
     * it can be combined with the mod field to encode an
     * addressing mode. Sometimes, certain combinations of 
     * the mod field and the r/m field are used to express
     * opcode information for some instructions. */
    uint8_t rm : MODRM_RM_SIZE;
};

static const ModRM DEFAULT_MODRM = {.mod = 0, .reg = 0, .rm = 0};

static const size_t SIB_SCALE_SIZE = 2;
static const size_t SIB_INDEX_SIZE = 3;
static const size_t SIB_BASE_SIZE  = 3;

//------------------------------------------------------------------------------
//! Certain encodings of the ModR/M byte require a second addressing byte (the 
//! SIB byte). The base-index and scale-index forms of 32-bit addressing require
//! the SIB byte. 
//------------------------------------------------------------------------------
struct SIB
{  
    /* Specifies the scale factor. */
    uint8_t scale : SIB_SCALE_SIZE;

    /* Specifies the register number of the index register. */
    uint8_t index : SIB_INDEX_SIZE;

    /* Specifies the register number of the base register. */
    uint8_t base : SIB_BASE_SIZE;
};

static const SIB DEFAULT_SIB = {.scale = 0, .index = 0, .base = 0};

//------------------------------------------------------------------------------
//! A displacement value is a 1, 2, 4, or 8 byte offset added to the calculated 
//! address. When an 8 byte displacement is used, no immediate operand is 
//! encoded.
//------------------------------------------------------------------------------
union Displacement
{
    int8_t  disp8;
    int16_t disp16;
    int32_t disp32;
    int64_t disp64;
};

static const Displacement DEFAULT_DISPLACEMENT = {.disp64 = 0};

//------------------------------------------------------------------------------
//! Some instructions require an immediate value of size 1, 2, 4 or 8 byte. The 
//! instruction determine the length of the immediate value.
//------------------------------------------------------------------------------
union Immediate
{
    int8_t  imm8;
    int16_t imm16;
    int32_t imm32;
    int64_t imm64;
};

static const Immediate DEFAULT_IMMEDIATE = {.imm64 = 0};

//------------------------------------------------------------------------------
//! Specifies an x86-64 instruction. 
//------------------------------------------------------------------------------
struct Instruction_x86_64
{
    bool isRexUsed;
    bool isModrmUsed;
    bool isSIBUsed;
    bool isDisplacementUsed;
    bool isImmediateUsed;
    
    REX          rex;
    Opcode       opcode;
    ModRM        modRM;
    SIB          sib;
    Displacement displacement;
    Immediate    immediate;
};

static const Instruction_x86_64 INSTRUCTIONS[100] =
{
    //=============================STACK============================
    //---------------------------push_r64---------------------------
    /* 1) opcode += r64 & 0b111
     * 2) REX.B   = 1 if r64 >= R8 */ 
    { 
        .isRexUsed          = false,
        .isModrmUsed        = false,
        .isSIBUsed          = false,
        .isDisplacementUsed = false,
        .isImmediateUsed    = false,

        .rex                = DEFAULT_REX,
        .opcode             = {.size = 1, .bytes = {0x50, 0x00, 0x00}},
        .modRM              = DEFAULT_MODRM, 
        .sib                = DEFAULT_SIB, 
        .displacement       = DEFAULT_DISPLACEMENT, 
        .immediate          = DEFAULT_IMMEDIATE 
    },
    //---------------------------push_r64---------------------------

    //----------------------------pop_r64---------------------------
    /* 1) opcode += r64 & 0b111
     * 2) REX.B   = 1 (if r64 >= R8) */
    { 
        .isRexUsed          = false,
        .isModrmUsed        = false,
        .isSIBUsed          = false,
        .isDisplacementUsed = false,
        .isImmediateUsed    = false,

        .rex                = DEFAULT_REX,
        .opcode             = {.size = 1, .bytes = {0x58, 0x00, 0x00}}, 
        .modRM              = DEFAULT_MODRM, 
        .sib                = DEFAULT_SIB, 
        .displacement       = DEFAULT_DISPLACEMENT, 
        .immediate          = DEFAULT_IMMEDIATE 
    },
    //----------------------------pop_r64---------------------------
    //=============================STACK============================

    //============================LOGICAL===========================
    //--------------------------test_r64_r64------------------------
    /* 1) Operand1 = modRM.rm  (expanded by REX.b if needed)
     * 2) Operand2 = modRM.reg (expanded by REX.r if needed) */
    { 
        .isRexUsed          = true,
        .isModrmUsed        = true,
        .isSIBUsed          = false,
        .isDisplacementUsed = false,
        .isImmediateUsed    = false,

        .rex                = {.id = REX_ID, .w = 1, .r = 0, .x = 0, .b = 0},
        .opcode             = {.size = 1, .bytes = {0x85, 0x00, 0x00}},
        .modRM              = {.mod = 0b11, .reg = 0b000, .rm = 0b000}, 
        .sib                = DEFAULT_SIB, 
        .displacement       = DEFAULT_DISPLACEMENT, 
        .immediate          = DEFAULT_IMMEDIATE
    },
    //--------------------------test_r64_r64------------------------

    //--------------------------xor_r64_r64-------------------------
    /* 1) Operand1 = modRM.rm  (expanded by REX.b if needed)
     * 2) Operand2 = modRM.reg (expanded by REX.r if needed) */
    { 
        .isRexUsed          = true,
        .isModrmUsed        = true,
        .isSIBUsed          = false,
        .isDisplacementUsed = false,
        .isImmediateUsed    = false,

        .rex                = {.id = REX_ID, .w = 1, .r = 0, .x = 0, .b = 0},
        .opcode             = {.size = 1, .bytes = {0x31, 0x00, 0x00}},
        .modRM              = {.mod = 0b11, .reg = 0b000, .rm = 0b000}, 
        .sib                = DEFAULT_SIB, 
        .displacement       = DEFAULT_DISPLACEMENT, 
        .immediate          = DEFAULT_IMMEDIATE
    },
    //--------------------------xor_r64_r64-------------------------

    //--------------------------cmp_r64_r64-------------------------
    /* 1) Operand1 = modRM.rm  (expanded by REX.b if needed)
     * 2) Operand2 = modRM.reg (expanded by REX.r if needed) */
    { 
        .isRexUsed          = true,
        .isModrmUsed        = true,
        .isSIBUsed          = false,
        .isDisplacementUsed = false,
        .isImmediateUsed    = false,

        .rex                = {.id = REX_ID, .w = 1, .r = 0, .x = 0, .b = 0},
        .opcode             = {.size = 1, .bytes = {0x39, 0x00, 0x00}},
        .modRM              = {.mod = 0b11, .reg = 0b000, .rm = 0b000}, 
        .sib                = DEFAULT_SIB, 
        .displacement       = DEFAULT_DISPLACEMENT, 
        .immediate          = DEFAULT_IMMEDIATE
    },
    //--------------------------cmp_r64_r64-------------------------
    //============================LOGICAL===========================
    
    //==========================ARITHMETIC==========================
    //--------------------------add_r64_r64-------------------------
    /* 1) Operand1 = modRM.rm  (expanded by REX.b if needed)
     * 2) Operand2 = modRM.reg (expanded by REX.r if needed) */
    { 
        .isRexUsed          = true,
        .isModrmUsed        = true,
        .isSIBUsed          = false,
        .isDisplacementUsed = false,
        .isImmediateUsed    = false,

        .rex                = {.id = REX_ID, .w = 1, .r = 0, .x = 0, .b = 0},
        .opcode             = {.size = 1, .bytes = {0x01, 0x00, 0x00}},
        .modRM              = {.mod = 0b11, .reg = 0b000, .rm = 0b000}, 
        .sib                = DEFAULT_SIB, 
        .displacement       = DEFAULT_DISPLACEMENT, 
        .immediate          = DEFAULT_IMMEDIATE
    },
    //--------------------------add_r64_r64-------------------------

    //-------------------------add_r64_imm32------------------------
    /* 1) Operand1 = modRM.rm (expanded by REX.b if needed)
     * 2) Operand2 = immediate.imm32 */
    { 
        .isRexUsed          = true,
        .isModrmUsed        = true,
        .isSIBUsed          = false,
        .isDisplacementUsed = false,
        .isImmediateUsed    = true,

        .rex                = {.id = REX_ID, .w = 1, .r = 0, .x = 0, .b = 0},
        .opcode             = {.size = 1, .bytes = {0x81, 0x00, 0x00}},
        .modRM              = {.mod = 0b11, .reg = 0b000, .rm = 0b000}, 
        .sib                = DEFAULT_SIB, 
        .displacement       = DEFAULT_DISPLACEMENT, 
        .immediate          = DEFAULT_IMMEDIATE
    },
    //-------------------------add_r64_imm32------------------------

    //--------------------------sub_r64_r64-------------------------
    /* 1) Operand1 = modRM.rm  (expanded by REX.b if needed)
     * 2) Operand2 = modRM.reg (expanded by REX.r if needed) */
    { 
        .isRexUsed          = true,
        .isModrmUsed        = true,
        .isSIBUsed          = false,
        .isDisplacementUsed = false,
        .isImmediateUsed    = false,

        .rex                = {.id = REX_ID, .w = 1, .r = 0, .x = 0, .b = 0},
        .opcode             = {.size = 1, .bytes = {0x29, 0x00, 0x00}},
        .modRM              = {.mod = 0b11, .reg = 0b000, .rm = 0b000}, 
        .sib                = DEFAULT_SIB, 
        .displacement       = DEFAULT_DISPLACEMENT, 
        .immediate          = DEFAULT_IMMEDIATE
    },
    //--------------------------sub_r64_r64-------------------------

    //-------------------------sub_r64_imm32------------------------
    /* 1) Operand1 = modRM.rm (expanded by REX.b if needed)
     * 2) Operand2 = immediate.imm32 */
    { 
        .isRexUsed          = true,
        .isModrmUsed        = true,
        .isSIBUsed          = false,
        .isDisplacementUsed = false,
        .isImmediateUsed    = true,

        .rex                = {.id = REX_ID, .w = 1, .r = 0, .x = 0, .b = 0},
        .opcode             = {.size = 1, .bytes = {0x81, 0x00, 0x00}},
        .modRM              = {.mod = 0b11, .reg = 0b101, .rm = 0b000}, 
        .sib                = DEFAULT_SIB, 
        .displacement       = DEFAULT_DISPLACEMENT, 
        .immediate          = DEFAULT_IMMEDIATE
    },
    //-------------------------sub_r64_imm32------------------------

    //-------------------------imul_r64_r64-------------------------
    /* 1) Operand1 = modRM.reg (expanded by REX.r if needed)
     * 2) Operand2 = modRM.rm  (expanded by REX.b if needed) */
    { 
        .isRexUsed          = true,
        .isModrmUsed        = true,
        .isSIBUsed          = false,
        .isDisplacementUsed = false,
        .isImmediateUsed    = false,

        .rex                = {.id = REX_ID, .w = 1, .r = 0, .x = 0, .b = 0},
        .opcode             = {.size = 2, .bytes = {0x0f, 0xaf, 0x00}},
        .modRM              = {.mod = 0b11, .reg = 0b000, .rm = 0b000}, 
        .sib                = DEFAULT_SIB, 
        .displacement       = DEFAULT_DISPLACEMENT, 
        .immediate          = DEFAULT_IMMEDIATE
    },
    //-------------------------imul_r64_r64-------------------------

    //---------------------------idiv_r64---------------------------
    /* 1) Operand = modRM.rm (expanded by REX.b if needed) */
    { 
        .isRexUsed          = true,
        .isModrmUsed        = true,
        .isSIBUsed          = false,
        .isDisplacementUsed = false,
        .isImmediateUsed    = false,

        .rex                = {.id = REX_ID, .w = 1, .r = 0, .x = 0, .b = 0},
        .opcode             = {.size = 1, .bytes = {0xf7, 0x00, 0x00}},
        .modRM              = {.mod = 0b11, .reg = 0b111, .rm = 0b000}, 
        .sib                = DEFAULT_SIB, 
        .displacement       = DEFAULT_DISPLACEMENT, 
        .immediate          = DEFAULT_IMMEDIATE
    },
    //---------------------------idiv_r64---------------------------

    //----------------------------neg_r64---------------------------
    /* 1) Operand = modRM.rm (expanded by REX.b if needed) */
    { 
        .isRexUsed          = true,
        .isModrmUsed        = true,
        .isSIBUsed          = false,
        .isDisplacementUsed = false,
        .isImmediateUsed    = false,

        .rex                = {.id = REX_ID, .w = 1, .r = 0, .x = 0, .b = 0},
        .opcode             = {.size = 1, .bytes = {0xf7, 0x00, 0x00}},
        .modRM              = {.mod = 0b11, .reg = 0b011, .rm = 0b000}, 
        .sib                = DEFAULT_SIB, 
        .displacement       = DEFAULT_DISPLACEMENT, 
        .immediate          = DEFAULT_IMMEDIATE
    },
    //----------------------------neg_r64---------------------------

    //-------------------------sal_r64_imm8-------------------------
    /* 1) Operand1 = modRM.rm (expanded by REX.b if needed)
     * 2) Operand2 = immediate.imm8 */
    { 
        .isRexUsed          = true,
        .isModrmUsed        = true,
        .isSIBUsed          = false,
        .isDisplacementUsed = false,
        .isImmediateUsed    = true,

        .rex                = {.id = REX_ID, .w = 1, .r = 0, .x = 0, .b = 0},
        .opcode             = {.size = 1, .bytes = {0xc1, 0x00, 0x00}},
        .modRM              = {.mod = 0b11, .reg = 0b100, .rm = 0b000}, 
        .sib                = DEFAULT_SIB, 
        .displacement       = DEFAULT_DISPLACEMENT, 
        .immediate          = DEFAULT_IMMEDIATE
    },
    //-------------------------sal_r64_imm8-------------------------
    //==========================ARITHMETIC==========================

    //=========================CONTROL FLOW=========================
    //--------------------------call_rel32--------------------------
    /* 1) Operand = displacement.disp32 */
    { 
        .isRexUsed          = false,
        .isModrmUsed        = false,
        .isSIBUsed          = false,
        .isDisplacementUsed = true,
        .isImmediateUsed    = false,

        .rex                = DEFAULT_REX,
        .opcode             = {.size = 1, .bytes = {0xe8, 0x00, 0x00}},
        .modRM              = DEFAULT_MODRM, 
        .sib                = DEFAULT_SIB, 
        .displacement       = DEFAULT_DISPLACEMENT, 
        .immediate          = DEFAULT_IMMEDIATE
    },
    //--------------------------call_rel32--------------------------

    //------------------------------ret-----------------------------
    { 
        .isRexUsed          = false,
        .isModrmUsed        = false,
        .isSIBUsed          = false,
        .isDisplacementUsed = false,
        .isImmediateUsed    = false,

        .rex                = DEFAULT_REX,
        .opcode             = {.size = 1, .bytes = {0xc3, 0x00, 0x00}},
        .modRM              = DEFAULT_MODRM, 
        .sib                = DEFAULT_SIB, 
        .displacement       = DEFAULT_DISPLACEMENT, 
        .immediate          = DEFAULT_IMMEDIATE
    },
    //------------------------------ret-----------------------------

    //---------------------------jmp_rel32--------------------------
    /* 1) Operand = displacement.disp32 */
    { 
        .isRexUsed          = false,
        .isModrmUsed        = false,
        .isSIBUsed          = false,
        .isDisplacementUsed = true,
        .isImmediateUsed    = false,

        .rex                = DEFAULT_REX,
        .opcode             = {.size = 1, .bytes = {0xe9, 0x00, 0x00}},
        .modRM              = DEFAULT_MODRM, 
        .sib                = DEFAULT_SIB, 
        .displacement       = DEFAULT_DISPLACEMENT, 
        .immediate          = DEFAULT_IMMEDIATE
    },
    //---------------------------jmp_rel32--------------------------

    //---------------------------jz_rel32---------------------------
    /* 1) Operand = displacement.disp32 */
    { 
        .isRexUsed          = false,
        .isModrmUsed        = false,
        .isSIBUsed          = false,
        .isDisplacementUsed = true,
        .isImmediateUsed    = false,

        .rex                = DEFAULT_REX,
        .opcode             = {.size = 2, .bytes = {0x0f, 0x84, 0x00}},
        .modRM              = DEFAULT_MODRM, 
        .sib                = DEFAULT_SIB, 
        .displacement       = DEFAULT_DISPLACEMENT, 
        .immediate          = DEFAULT_IMMEDIATE
    },
    //---------------------------jz_rel32---------------------------

    //---------------------------je_rel32---------------------------
    /* 1) Operand = displacement.disp32 */
    { 
        .isRexUsed          = false,
        .isModrmUsed        = false,
        .isSIBUsed          = false,
        .isDisplacementUsed = true,
        .isImmediateUsed    = false,

        .rex                = DEFAULT_REX,
        .opcode             = {.size = 2, .bytes = {0x0f, 0x84, 0x00}},
        .modRM              = DEFAULT_MODRM, 
        .sib                = DEFAULT_SIB, 
        .displacement       = DEFAULT_DISPLACEMENT, 
        .immediate          = DEFAULT_IMMEDIATE
    },
    //---------------------------je_rel32---------------------------

    //---------------------------jne_rel32--------------------------
    /* 1) Operand = displacement.disp32 */
    { 
        .isRexUsed          = false,
        .isModrmUsed        = false,
        .isSIBUsed          = false,
        .isDisplacementUsed = true,
        .isImmediateUsed    = false,

        .rex                = DEFAULT_REX,
        .opcode             = {.size = 2, .bytes = {0x0f, 0x85, 0x00}},
        .modRM              = DEFAULT_MODRM, 
        .sib                = DEFAULT_SIB, 
        .displacement       = DEFAULT_DISPLACEMENT, 
        .immediate          = DEFAULT_IMMEDIATE
    },
    //---------------------------jne_rel32--------------------------

    //---------------------------jl_rel32---------------------------
    /* 1) Operand = displacement.disp32 */
    { 
        .isRexUsed          = false,
        .isModrmUsed        = false,
        .isSIBUsed          = false,
        .isDisplacementUsed = true,
        .isImmediateUsed    = false,

        .rex                = DEFAULT_REX,
        .opcode             = {.size = 2, .bytes = {0x0f, 0x8c, 0x00}},
        .modRM              = DEFAULT_MODRM, 
        .sib                = DEFAULT_SIB, 
        .displacement       = DEFAULT_DISPLACEMENT, 
        .immediate          = DEFAULT_IMMEDIATE
    },
    //---------------------------jl_rel32---------------------------

    //---------------------------jg_rel32---------------------------
    /* 1) Operand = displacement.disp32 */
    { 
        .isRexUsed          = false,
        .isModrmUsed        = false,
        .isSIBUsed          = false,
        .isDisplacementUsed = true,
        .isImmediateUsed    = false,

        .rex                = DEFAULT_REX,
        .opcode             = {.size = 2, .bytes = {0x0f, 0x8f, 0x00}},
        .modRM              = DEFAULT_MODRM, 
        .sib                = DEFAULT_SIB, 
        .displacement       = DEFAULT_DISPLACEMENT, 
        .immediate          = DEFAULT_IMMEDIATE
    },
    //---------------------------jg_rel32---------------------------

    //---------------------------jle_rel32--------------------------
    /* 1) Operand = displacement.disp32 */
    { 
        .isRexUsed          = false,
        .isModrmUsed        = false,
        .isSIBUsed          = false,
        .isDisplacementUsed = true,
        .isImmediateUsed    = false,

        .rex                = DEFAULT_REX,
        .opcode             = {.size = 2, .bytes = {0x0f, 0x8e, 0x00}},
        .modRM              = DEFAULT_MODRM, 
        .sib                = DEFAULT_SIB, 
        .displacement       = DEFAULT_DISPLACEMENT, 
        .immediate          = DEFAULT_IMMEDIATE
    },
    //---------------------------jle_rel32--------------------------

    //---------------------------jge_rel32--------------------------
    /* 1) Operand = displacement.disp32 */
    { 
        .isDisplacementUsed = true,
        .opcode             = {.size = 2, .bytes = {0x0f, 0x8d}}
    },
    //---------------------------jge_rel32--------------------------
    //=========================CONTROL FLOW=========================

    //=============================MOVE=============================
    //--------------------------mov_r64_r64-------------------------
    /* 1) Operand = displacement.disp32 */
    { 
        .isRexUsed   = true,
        .isModrmUsed = true,

        .rex    = {.id = REX_ID, .w = 1},
        .opcode = {.size = 2, .bytes = {0x0f, 0x8d}}
    },
    //--------------------------mov_r64_r64-------------------------
    //=============================MOVE=============================
};

static const char* REGISTERS_64_STRINGS[TOTAL_REGISTERS_64] = 
{
    "rax",
    "rcx",
    "rdx",
    "rbx",
    "rsp",
    "rbp",
    "rsi",
    "rdi",

    "r8",
    "r9",
    "r10",
    "r11",
    "r12",
    "r13",
    "r14",
    "r15"
};

const char* reg64ToString (Reg64 reg);
Mem64       mem64BaseDisp (Reg64 base, int32_t displacement); 
bool        isValidScale  (uint8_t scale);

#endif