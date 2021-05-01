#include <inttypes.h>
#include "nasm_compilation.h"

void writeComment (Compiler* compiler, const char* comment);
void writeMem64   (Compiler* compiler, Mem64 mem64);

// FIXME: write '\n' here?
void writeComment(Compiler* compiler, const char* comment)
{
    ASSERT_COMPILER(compiler);

    if (comment != nullptr)
    {
        write(compiler, " ; %s", comment);
    }

    writeNewLine(compiler);
}

void writeMem64(Compiler* compiler, Mem64 mem64)
{
    ASSERT_COMPILER(compiler);

    Reg64 base  = mem64.base;
    Reg64 index = mem64.index;

    if (base != INVALID_REG64)
    {
        write(compiler, reg64ToString(base));
    }

    if (index != INVALID_REG64)
    {
        if (base != INVALID_REG64) { write(compiler, " + "); }

        write(compiler, reg64ToString(index));

        if (isValidScale(mem64.scale))
        {
            write(compiler, " * %" PRIu8, mem64.scale);
        }
    }

    if (mem64.displacement != 0)
    {
        if (base != INVALID_REG64 || index != INVALID_REG64) 
        { 
            write(compiler, mem64.displacement > 0 ? " + " : " - "); 
        }

        write(compiler, "%" PRId32, abs(mem64.displacement));
    }
}

//---------------------------------NASM_STACK-----------------------------------
void write_push_r64(Compiler* compiler, Reg64 reg, const char* comment)
{
    ASSERT_COMPILER(compiler);

    

    writeIndented(compiler, "push %s", reg64ToString(reg));
    writeComment(compiler, comment);
}

void write_pop_r64(Compiler* compiler, Reg64 reg, const char* comment)
{
    ASSERT_COMPILER(compiler);

    writeIndented(compiler, "pop %s", reg64ToString(reg));
    writeComment(compiler, comment);
}
//---------------------------------NASM_STACK-----------------------------------


//--------------------------------NASM_LOGICAL----------------------------------
void write_test_r64_r64(Compiler* compiler, Reg64 firstReg, Reg64 secondReg, const char* comment)
{
    ASSERT_COMPILER(compiler);

    writeIndented(compiler, "test %s, %s", reg64ToString(firstReg), reg64ToString(secondReg));
    writeComment(compiler, comment);
}

void write_xor_r64_r64(Compiler* compiler, Reg64 firstReg, Reg64 secondReg, const char* comment)
{
    ASSERT_COMPILER(compiler);

    writeIndented(compiler, "xor %s, %s", reg64ToString(firstReg), reg64ToString(secondReg));
    writeComment(compiler, comment);
}

void write_cmp_r64_r64(Compiler* compiler, Reg64 firstReg, Reg64 secondReg, const char* comment)
{
    ASSERT_COMPILER(compiler);

    writeIndented(compiler, "cmp %s, %s", reg64ToString(firstReg), reg64ToString(secondReg));
    writeComment(compiler, comment);
}
//--------------------------------NASM_LOGICAL----------------------------------


//------------------------------NASM_ARITHMETIC---------------------------------
void write_add_r64_r64(Compiler* compiler, Reg64 firstReg, Reg64 secondReg, const char* comment)
{
    ASSERT_COMPILER(compiler);

    writeIndented(compiler, "add %s, %s", reg64ToString(firstReg), reg64ToString(secondReg));
    writeComment(compiler, comment);
}

void write_add_r64_imm32(Compiler* compiler, Reg64 reg, int32_t number, const char* comment)
{
    ASSERT_COMPILER(compiler);

    writeIndented(compiler, "add %s, %" PRId32, reg64ToString(reg), number);
    writeComment(compiler, comment);
}

void write_sub_r64_r64(Compiler* compiler, Reg64 firstReg, Reg64 secondReg, const char* comment)
{
    ASSERT_COMPILER(compiler);

    writeIndented(compiler, "sub %s, %s", reg64ToString(firstReg), reg64ToString(secondReg));
    writeComment(compiler, comment);
}

void write_sub_r64_imm32(Compiler* compiler, Reg64 reg, int32_t number, const char* comment)
{
    ASSERT_COMPILER(compiler);

    writeIndented(compiler, "sub %s, %" PRId32, reg64ToString(reg), number);
    writeComment(compiler, comment);
}

void write_imul_r64_r64(Compiler* compiler, Reg64 firstReg, Reg64 secondReg, const char* comment)
{
    ASSERT_COMPILER(compiler);

    writeIndented(compiler, "imul %s, %s", reg64ToString(firstReg), reg64ToString(secondReg));
    writeComment(compiler, comment);
}

void write_idiv_r64(Compiler* compiler, Reg64 reg, const char* comment)
{
    ASSERT_COMPILER(compiler);

    writeIndented(compiler, "idiv %s", reg64ToString(reg));
    writeComment(compiler, comment);
}

void write_neg_r64(Compiler* compiler, Reg64 reg, const char* comment)
{
    ASSERT_COMPILER(compiler);

    writeIndented(compiler, "neg %s", reg64ToString(reg));
    writeComment(compiler, comment);
}

void write_sal_r64_imm8(Compiler* compiler, Reg64 reg, int8_t number, const char* comment)
{
    ASSERT_COMPILER(compiler);

    writeIndented(compiler, "sal %s, %" PRId8, reg64ToString(reg), number);
    writeComment(compiler, comment);
}
//------------------------------NASM_ARITHMETIC---------------------------------


//-----------------------------NASM_CONTROL_FLOW--------------------------------
void write_call_rel32(Compiler* compiler, Label label, const char* comment)
{
    ASSERT_COMPILER(compiler);

    writeIndented(compiler, "call %s", label.name);
    if (label.number != -1)
    {
        write(compiler, "%d", label.number);
    }

    writeComment(compiler, comment);
}

void write_ret(Compiler* compiler, const char* comment)
{
    ASSERT_COMPILER(compiler);

    writeIndented(compiler, "ret");
    writeComment(compiler, comment);
}

void write_jmp_rel32(Compiler* compiler, Label label, const char* comment)
{
    ASSERT_COMPILER(compiler);

    writeIndented(compiler, "jmp %s", label.name);
    if (label.number != -1)
    {
        write(compiler, "%d", label.number);
    }

    writeComment(compiler, comment);
}

void write_jz_rel32(Compiler* compiler, Label label, const char* comment)
{
    ASSERT_COMPILER(compiler);

    writeIndented(compiler, "jz %s", label.name);
    if (label.number != -1)
    {
        write(compiler, "%d", label.number);
    }

    writeComment(compiler, comment);
}

void write_je_rel32(Compiler* compiler, Label label, const char* comment)
{
    writeIndented(compiler, "je %s", label.name);
    if (label.number != -1)
    {
        write(compiler, "%d", label.number);
    }

    writeComment(compiler, comment);
}

void write_jne_rel32(Compiler* compiler, Label label, const char* comment)
{
    writeIndented(compiler, "jne %s", label.name);
    if (label.number != -1)
    {
        write(compiler, "%d", label.number);
    }

    writeComment(compiler, comment);
}

void write_jl_rel32(Compiler* compiler, Label label, const char* comment)
{
    writeIndented(compiler, "jl %s", label.name);
    if (label.number != -1)
    {
        write(compiler, "%d", label.number);
    }

    writeComment(compiler, comment);
}

void write_jg_rel32(Compiler* compiler, Label label, const char* comment)
{
    writeIndented(compiler, "jg %s", label.name);
    if (label.number != -1)
    {
        write(compiler, "%d", label.number);
    }

    writeComment(compiler, comment);
}

void write_jle_rel32(Compiler* compiler, Label label, const char* comment)
{
    writeIndented(compiler, "jle %s", label.name);
    if (label.number != -1)
    {
        write(compiler, "%d", label.number);
    }

    writeComment(compiler, comment);
}

void write_jge_rel32(Compiler* compiler, Label label, const char* comment)
{
    writeIndented(compiler, "jge %s", label.name);
    if (label.number != -1)
    {
        write(compiler, "%d", label.number);
    }

    writeComment(compiler, comment);
}
//-----------------------------NASM_CONTROL_FLOW--------------------------------


//---------------------------------NASM_MOVE------------------------------------
void write_mov_r64_r64(Compiler* compiler, Reg64 dest, Reg64 src, const char* comment)
{
    ASSERT_COMPILER(compiler);

    writeIndented(compiler, "mov %s, %s", reg64ToString(dest), reg64ToString(src));
    writeComment(compiler, comment);
}

void write_mov_r64_imm64(Compiler* compiler, Reg64 dest, int64_t number, const char* comment)
{
    ASSERT_COMPILER(compiler);

    writeIndented(compiler, "mov %s, %" PRId64, reg64ToString(dest), number);
    writeComment(compiler, comment);
}

void write_mov_r64_imm64(Compiler* compiler, Reg64 dest, Label label, const char* comment)
{
    ASSERT_COMPILER(compiler);

    writeIndented(compiler, "mov %s, %s", reg64ToString(dest), label.name);
    if (label.number != -1)
    {
        write(compiler, "%d", label.number);
    }

    writeComment(compiler, comment);
}

void write_mov_m64_r64(Compiler* compiler, Mem64 dest, Reg64 src, const char* comment)
{
    ASSERT_COMPILER(compiler);

    writeIndented(compiler, "mov [");
    writeMem64(compiler, dest);
    write(compiler, "], %s", reg64ToString(src));

    writeComment(compiler, comment);
}

void write_mov_r64_m64(Compiler* compiler, Reg64 dest, Mem64 src, const char* comment)
{
    ASSERT_COMPILER(compiler);

    writeIndented(compiler, "mov %s, [", reg64ToString(dest));
    writeMem64(compiler, src);
    write(compiler, "]");
    writeComment(compiler, comment);
}
//---------------------------------NASM_MOVE------------------------------------