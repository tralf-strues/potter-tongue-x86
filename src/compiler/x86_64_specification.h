//------------------------------------------------------------------------------
//! Many of the comments are taken from official Intel documentation.
//! 
//! @file   x86_64_specification.h
//! @author tralf-strues
//! @date   2021-05-04
//! 
//! @copyright Copyright (c) 2021
//------------------------------------------------------------------------------

#ifndef X86_64_SPECIFICATION_H
#define X86_64_SPECIFICATION_H

#include <stdlib.h>
#include <stdint.h>

//------------------------------------------------------------------------------
//! Specifies 64-bit registers in the same order they are interpreted in
//! instructions. Registers RAX-RDI can also be referred to as R0-R7.
//------------------------------------------------------------------------------
enum Reg64
{
    RAX, RCX, RDX, RBX, RSP, RBP, RSI, RDI,
    R8,  R9,  R10, R11, R12, R13, R14, R15,

    TOTAL_REGISTERS_64,
    INVALID_REG64 = -1
};

//------------------------------------------------------------------------------
//! String representations of 64-bit registers in lower-case ASCII characters.
//------------------------------------------------------------------------------
static const char* REGISTERS_64_STRINGS[TOTAL_REGISTERS_64] = 
{
    "rax", "rcx", "rdx", "rbx", "rsp", "rbp", "rsi", "rdi",
    "r8",  "r9",  "r10", "r11", "r12", "r13", "r14", "r15"
};

//------------------------------------------------------------------------------
//! Specifies memory-addressing used in several instructions. If all the fields
//! are used then the address is calculated the following way: 
//!     [base + index * scale + displacement] 
//!
//! Note that not all memory addressing combinations of registers are valid. The 
//! cases bellow aren't supported:
//! 1) [base]                          - RBP and R13 can't be used as base 
//! 2) [base + index * scale]          - RBP and R13 can't be used as base
//! 3) [index * scale + disp32]        - RSP         can't be used as index
//! 4) [base + index * scale + disp32] - RSP         can't be used as index
//------------------------------------------------------------------------------
struct Mem64
{
    /* Any general-purpose register or INVALID_REG64 to not use. */
    Reg64 base;

    /* Any 32-bit number (if 0 then not used). */
    int32_t displacement;

    /* Any general-purpose register or INVALID_REG64 to not use. */
    Reg64 index;

    /* Either 1, 2, 4 or 8 (otherwise not used). */
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

static const size_t  REX_ID_SIZE = 4;
static const uint8_t REX_ID      = 0b0100;
static const size_t  REX_W_SIZE  = 1;
static const size_t  REX_R_SIZE  = 1;
static const size_t  REX_X_SIZE  = 1;
static const size_t  REX_B_SIZE  = 1;

//------------------------------------------------------------------------------
//! Instruction prefix that is used (mostly; see Intel's reference for more
//! info on the subject) whenever one needs to use 64-bit operand size and the 
//! instruction doesn't default to it.
//------------------------------------------------------------------------------
struct Rex
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

static const Rex DEFAULT_REX = {.id = REX_ID, .w = 0, .r = 0, .x = 0, .b = 0};

static const size_t  MODRM_MOD_SIZE      = 2;
static const size_t  MODRM_REG_SIZE      = 3;
static const size_t  MODRM_RM_SIZE       = 3;
static const uint8_t MODRM_MOD_MAX_VALUE = 0b11;
static const uint8_t MODRM_REG_MAX_VALUE = 0b111;
static const uint8_t MODRM_RM_MAX_VALUE  = 0b111;

//------------------------------------------------------------------------------
//! Many instructions that refer to an operand in memory have an addressing-form 
//! specifier byte (called the ModR/M byte) following the primary opcode.
//------------------------------------------------------------------------------
struct Modrm
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

static const size_t SIB_SCALE_SIZE = 2;
static const size_t SIB_INDEX_SIZE = 3;
static const size_t SIB_BASE_SIZE  = 3;

//------------------------------------------------------------------------------
//! Certain encodings of the ModR/M byte require a second addressing byte (the 
//! SIB byte). The base-index and scale-index forms of 32-bit addressing require
//! the SIB byte. 
//------------------------------------------------------------------------------
struct Sib
{  
    /* Specifies the scale factor. */
    uint8_t scale : SIB_SCALE_SIZE;

    /* Specifies the register number of the index register. */
    uint8_t index : SIB_INDEX_SIZE;

    /* Specifies the register number of the base register. */
    uint8_t base : SIB_BASE_SIZE;
};

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

//------------------------------------------------------------------------------
//! Specifies an x86-64 instruction. 
//------------------------------------------------------------------------------
struct Instruction_x86_64
{
    /* Specify whether the respective optional instruction 
     * bytes are used. */
    bool isRexUsed;
    bool isModrmUsed;
    bool isSibUsed;

    /* These two values are in bytes. */
    uint8_t dispSize;
    uint8_t immSize;
    
    Rex          rex;
    Opcode       opcode;
    Modrm        modrm;
    Sib          sib;
    Displacement disp;
    Immediate    imm;
};

//------------------------------------------------------------------------------
//! @param reg
//! 
//! @return The lower 3 bits of the register number.
//------------------------------------------------------------------------------
uint8_t regSpecifier(Reg64 reg);

//------------------------------------------------------------------------------
//!       7   6   5   4   3   2   1   0
//!     +---+---+---+---+---+---+---+---+
//!     | 0 | 1 | 0 | 0 | W | R | X | B |       
//!     +---+---+---+---+---+---+---+---+
//! 
//! @param rex
//! @return Byte representation of rex in the format above.
//------------------------------------------------------------------------------
uint8_t rexToByte(Rex rex);

//------------------------------------------------------------------------------
//!       7   6   5   4   3   2   1   0
//!     +---+---+---+---+---+---+---+---+
//!     |  mod  |    reg    |    r/m    |       
//!     +---+---+---+---+---+---+---+---+
//! 
//! @param modrm
//! @return Byte representation of modrm in the format above.
//------------------------------------------------------------------------------
uint8_t modrmToByte(Modrm modrm);

//------------------------------------------------------------------------------
//!       7   6   5   4   3   2   1   0
//!     +---+---+---+---+---+---+---+---+
//!     | scale |   index   |   base    |       
//!     +---+---+---+---+---+---+---+---+
//! 
//! @param sib
//! @return Byte representation of sib in the format above.
//------------------------------------------------------------------------------
uint8_t sibToByte(Sib sib);

//------------------------------------------------------------------------------
//! Adds REX byte to the instruction (if not already used) equal to DEFAULT_REX.
//! 
//! @param instruction
//------------------------------------------------------------------------------
void addRex(Instruction_x86_64* instruction);

//------------------------------------------------------------------------------
//! Updates REX byte of the instruction based on the reg:
//! 1) if reg is in RAX-RDI
//!     a) if REX is used, sets REX.R to 0 
//!     b) otherwise does nothing
//! 2) if reg is in R8-R15
//!     a) if REX is used, sets REX.R to 1 
//!     b) otherwise adds REX byte and sets REX.B to 1
//! 
//! @param instruction
//! @param reg
//------------------------------------------------------------------------------
void updateRexR(Instruction_x86_64* instruction, Reg64 reg);

//------------------------------------------------------------------------------
//! Updates REX byte of the instruction based on the reg:
//! 1) if reg is in RAX-RDI
//!     a) if REX is used, sets REX.X to 0 
//!     b) otherwise does nothing
//! 2) if reg is in R8-R15
//!     a) if REX is used, sets REX.X to 1 
//!     b) otherwise adds REX byte and sets REX.B to 1
//! 
//! @param instruction
//! @param reg
//------------------------------------------------------------------------------
void updateRexX(Instruction_x86_64* instruction, Reg64 reg);

//------------------------------------------------------------------------------
//! Updates REX byte of the instruction based on the reg:
//! 1) if reg is in RAX-RDI
//!     a) if REX is used, sets REX.B to 0 
//!     b) otherwise does nothing
//! 2) if reg is in R8-R15
//!     a) if REX is used, sets REX.B to 1 
//!     b) otherwise adds REX byte and sets REX.B to 1
//! 
//! @param instruction
//! @param reg
//------------------------------------------------------------------------------
void updateRexB(Instruction_x86_64* instruction, Reg64 reg);

//------------------------------------------------------------------------------
//! Adds MODRM byte to the instruction (if not already used) with the specified 
//! mod and fields reg and rm set to zero.
//! 
//! @param instruction
//! @param mod
//------------------------------------------------------------------------------
void addModrm(Instruction_x86_64* instruction, uint8_t mod);

//------------------------------------------------------------------------------
//! Adds MODRM (with mod = 0b00) byte if not already used and sets MODRM.reg to 
//! the lower 3 bits of the register. 
//! 
//! @param instruction
//! @param reg
//------------------------------------------------------------------------------
void updateModrmReg(Instruction_x86_64* instruction, Reg64 reg);

//------------------------------------------------------------------------------
//! Adds MODRM (with mod = 0b00) byte if not already used and sets MODRM.rm to 
//! the lower 3 bits of the register. 
//! 
//! @param instruction
//! @param reg
//------------------------------------------------------------------------------
void updateModrmRm(Instruction_x86_64* instruction, Reg64 reg);

//------------------------------------------------------------------------------
//! Adds zero SIB byte to the instruction (if not already used).
//! 
//! @param instruction
//------------------------------------------------------------------------------
void addSib(Instruction_x86_64* instruction);

//------------------------------------------------------------------------------
//! Adds SIB (with everything set to 0) byte if not already used and sets 
//! SIB.index to the lower 3 bits of the register. 
//! 
//! @param instruction
//! @param reg
//------------------------------------------------------------------------------
void updateSibIndex(Instruction_x86_64* instruction, Reg64 reg);

//------------------------------------------------------------------------------
//! Adds SIB (with everything set to 0) byte if not already used and sets 
//! SIB.base to the lower 3 bits of the register. 
//! 
//! @param instruction
//! @param reg
//------------------------------------------------------------------------------
void updateSibBase(Instruction_x86_64* instruction, Reg64 reg);

//------------------------------------------------------------------------------
//! Sets REX, MODRM and (if necessary SIB) bytes as well as displacement based
//! on memory. 
//! 
//! @param instruction
//! @param memory
//!
//! @return Whether the addressing mode is valid (see Mem64 declaration for 
//!         information on invalid cases).
//------------------------------------------------------------------------------
bool setMemoryAddressing(Instruction_x86_64* instruction, Mem64 memory);

//------------------------------------------------------------------------------
//! @param reg
//! 
//! @return String representation of reg using lower-case ASCII characters, for 
//!         example "rdi" if reg = RDI.
//------------------------------------------------------------------------------
const char* reg64ToString(Reg64 reg);

//------------------------------------------------------------------------------
//! @param base
//! @param displacement
//! 
//! @return Mem64 with the specified base register and 32-bit displacement.
//------------------------------------------------------------------------------
Mem64 mem64BaseDisp(Reg64 base, int32_t displacement); 

//------------------------------------------------------------------------------
//! Determines whether or not scale has one of the valid values: 1, 2, 4 or 8.
//! 
//! @param scale
//! 
//! @return Whether scale is 1, 2, 4, 8 or not.
//------------------------------------------------------------------------------
bool isValidScale(uint8_t scale);

//------------------------------------------------------------------------------
//! Transforms valid Mem64 scales like 1, 2, 4 and 8 to 0b00, 0b01, 0b10 and 
//! 0b11, respectively. Invalid scales are transformed to 0b00.
//! 
//! @param scale
//! 
//! @return Transformed to SIB format scale.
//------------------------------------------------------------------------------
uint8_t toSibScale(uint8_t scale);

#endif