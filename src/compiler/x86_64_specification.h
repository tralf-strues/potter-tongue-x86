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
    Reg64   base;         // any general-purpose register or INVALID_REG64 to not use
    int32_t displacement; // any 32-bit number (if 0 then isn't used) 
    Reg64   index;        // any general-purpose register or INVALID_REG64 to not use
    uint8_t scale;        // either 2, 4 or 8 (otherwise isn't used)
};

static const char* REGISTERS_64_STRINGS[TOTAL_REGISTERS_64] = {
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