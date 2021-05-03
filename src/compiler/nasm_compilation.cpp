#include <inttypes.h>
#include "nasm_compilation.h"

void writeInstruction(Compiler* compiler, const Instruction_x86_64* instruction);

void writeComment (Compiler* compiler, Comment comment);
void writeMem64   (Compiler* compiler, Mem64 mem64);

void writeInstruction(Compiler* compiler, const Instruction_x86_64* instruction)
{
    ASSERT_COMPILER(compiler);

    ElfBuilder* builder = &compiler->builder;

    if (instruction->isRexUsed)
    {
        writeByte(builder, rexToByte(instruction->rex));
    }

    writeBytes(builder, instruction->opcode.bytes, instruction->opcode.size);

    if (instruction->isModrmUsed)
    {
        writeByte(builder, modrmToByte(instruction->modrm));
    }

    if (instruction->isSibUsed)
    {
        writeByte(builder, sibToByte(instruction->sib));
    }

    switch (instruction->dispSize)
    {
        case 1:  { writeByte   (builder, instruction->disp.disp8 ); break; }
        case 2:  { writeUInt16 (builder, instruction->disp.disp16); break; }
        case 4:  { writeUInt32 (builder, instruction->disp.disp32); break; }
        case 8:  { writeUInt64 (builder, instruction->disp.disp64); break; }

        case 0:  { break; }
        default: { assert(!"Valid displacement size."); }
    }

    switch (instruction->immSize)
    {
        case 1:  { writeByte   (builder, instruction->imm.imm8 ); break; }
        case 2:  { writeUInt16 (builder, instruction->imm.imm16); break; }
        case 4:  { writeUInt32 (builder, instruction->imm.imm32); break; }
        case 8:  { writeUInt64 (builder, instruction->imm.imm64); break; }

        case 0:  { break; }
        default: { assert(!"Valid immediate constant size."); }
    }
}

// FIXME: write '\n' here?
void writeComment(Compiler* compiler, Comment comment)
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

//===================================GENERAL====================================
void write_instruction_r64_r64(Compiler* compiler, Opcode opcode, Reg64 reg1, Reg64 reg2)
{
    ASSERT_COMPILER(compiler);

    Instruction_x86_64 instruction = {};
    
    addRex(&instruction);
    instruction.rex.w = 1; /* long mode */
    instruction.rex.x = 0; /* SIB isn't used */
    updateRexR(&instruction, reg2);
    updateRexB(&instruction, reg1);

    instruction.opcode = opcode;

    addModrm(&instruction, 0b11); /* mod 0b11 used for all such instructions */
    updateModrmReg(&instruction, reg2);
    updateModrmRm(&instruction, reg1);

    writeInstruction(compiler, &instruction);
}

void write_jump_rel32(Compiler* compiler, Opcode opcode, int32_t labelAddress)
{
    ASSERT_COMPILER(compiler);

    Instruction_x86_64 instruction = {};
    instruction.opcode    = opcode;
    instruction.immSize   = 4;
    
    int32_t instructionLength = opcode.size + 4;
    instruction.imm.imm32 = labelAddress - compiler->builder.offset - instructionLength;

    writeInstruction(compiler, &instruction);
}
//===================================GENERAL====================================


//====================================STACK=====================================
void write_push_r64(Compiler* compiler, Reg64 reg, Comment comment)
{
    ASSERT_COMPILER(compiler);

    /* ----------------BYTECODE---------------- */
    Instruction_x86_64 instruction = {};
    updateRexB(&instruction, reg);

    instruction.opcode           = OPCODE_BASE_PUSH_R64;
    instruction.opcode.bytes[0] += regSpecifier(reg);

    writeInstruction(compiler, &instruction);

    /* ------------------NASM------------------ */
    writeIndented(compiler, "push %s", reg64ToString(reg));
    writeComment(compiler, comment);
}

void write_pop_r64(Compiler* compiler, Reg64 reg, Comment comment)
{
    ASSERT_COMPILER(compiler);

    /* ----------------BYTECODE---------------- */
    Instruction_x86_64 instruction = {};
    updateRexB(&instruction, reg);
    
    instruction.opcode           = OPCODE_BASE_POP_R64;
    instruction.opcode.bytes[0] += regSpecifier(reg);

    writeInstruction(compiler, &instruction);

    /* ------------------NASM------------------ */
    writeIndented(compiler, "pop %s", reg64ToString(reg));
    writeComment(compiler, comment);
}
//====================================STACK=====================================


//===================================LOGICAL====================================
void write_test_r64_r64(Compiler* compiler, Reg64 reg1, Reg64 reg2, Comment comment)
{
    ASSERT_COMPILER(compiler);

    /* ----------------BYTECODE---------------- */
    write_instruction_r64_r64(compiler, OPCODE_TEST_R64_R64, reg1, reg2);

    /* ------------------NASM------------------ */
    writeIndented(compiler, "test %s, %s", reg64ToString(reg1), reg64ToString(reg2));
    writeComment(compiler, comment);
}

void write_xor_r64_r64(Compiler* compiler, Reg64 reg1, Reg64 reg2, Comment comment)
{
    ASSERT_COMPILER(compiler);

    /* ----------------BYTECODE---------------- */
    write_instruction_r64_r64(compiler, OPCODE_XOR_R64_R64, reg1, reg2);

    /* ------------------NASM------------------ */
    writeIndented(compiler, "xor %s, %s", reg64ToString(reg1), reg64ToString(reg2));
    writeComment(compiler, comment);
}

void write_cmp_r64_r64(Compiler* compiler, Reg64 reg1, Reg64 reg2, Comment comment)
{
    ASSERT_COMPILER(compiler);

    /* ----------------BYTECODE---------------- */
    write_instruction_r64_r64(compiler, OPCODE_CMP_R64_R64, reg1, reg2);

    /* ------------------NASM------------------ */
    writeIndented(compiler, "cmp %s, %s", reg64ToString(reg1), reg64ToString(reg2));
    writeComment(compiler, comment);
}
//===================================LOGICAL====================================


//=================================ARITHMETIC===================================
void write_add_r64_r64(Compiler* compiler, Reg64 reg1, Reg64 reg2, Comment comment)
{
    ASSERT_COMPILER(compiler);

    /* ----------------BYTECODE---------------- */
    write_instruction_r64_r64(compiler, OPCODE_ADD_R64_R64, reg1, reg2);

    /* ------------------NASM------------------ */
    writeIndented(compiler, "add %s, %s", reg64ToString(reg1), reg64ToString(reg2));
    writeComment(compiler, comment);
}

void write_add_r64_imm32(Compiler* compiler, Reg64 reg, int32_t imm, Comment comment)
{
    ASSERT_COMPILER(compiler);

    /* ----------------BYTECODE---------------- */
    Instruction_x86_64 instruction = {};
    
    addRex(&instruction);
    instruction.rex.w = 1; /* long mode */
    instruction.rex.r = 0; /* MODRM.reg extends the opcode, hence no need in REX.r */
    instruction.rex.x = 0; /* SIB isn't used */
    updateRexB(&instruction, reg);
    
    instruction.opcode = OPCODE_ADD_R64_IMM32;

    addModrm(&instruction, 0b11);
    instruction.modrm.reg = OPCODE_ADD_EXTENSION;
    updateModrmRm(&instruction, reg);

    instruction.dispSize    = 4;
    instruction.disp.disp32 = imm;

    writeInstruction(compiler, &instruction);

    /* ------------------NASM------------------ */
    writeIndented(compiler, "add %s, %" PRId32, reg64ToString(reg), imm);
    writeComment(compiler, comment);
}

void write_sub_r64_r64(Compiler* compiler, Reg64 reg1, Reg64 reg2, Comment comment)
{
    ASSERT_COMPILER(compiler);

    /* ----------------BYTECODE---------------- */
    write_instruction_r64_r64(compiler, OPCODE_SUB_R64_R64, reg1, reg2);

    /* ------------------NASM------------------ */
    writeIndented(compiler, "sub %s, %s", reg64ToString(reg1), reg64ToString(reg2));
    writeComment(compiler, comment);
}

void write_sub_r64_imm32(Compiler* compiler, Reg64 reg, int32_t imm, Comment comment)
{
    ASSERT_COMPILER(compiler);

    /* ----------------BYTECODE---------------- */
    Instruction_x86_64 instruction = {};
    
    addRex(&instruction);
    instruction.rex.w = 1; /* long mode */
    instruction.rex.r = 0; /* MODRM.reg extends the opcode, hence no need in REX.r */
    instruction.rex.x = 0; /* SIB isn't used */
    updateRexB(&instruction, reg);
    
    instruction.opcode = OPCODE_SUB_R64_IMM32;

    addModrm(&instruction, 0b11);
    instruction.modrm.reg = OPCODE_SUB_EXTENSION;
    updateModrmRm(&instruction, reg);

    instruction.dispSize    = 4;
    instruction.disp.disp32 = imm;

    writeInstruction(compiler, &instruction);

    /* ------------------NASM------------------ */
    writeIndented(compiler, "sub %s, %" PRId32, reg64ToString(reg), imm);
    writeComment(compiler, comment);
}

void write_imul_r64_r64(Compiler* compiler, Reg64 reg1, Reg64 reg2, Comment comment)
{
    ASSERT_COMPILER(compiler);

    /* ----------------BYTECODE---------------- */
    write_instruction_r64_r64(compiler, OPCODE_IMUL_R64_R64, reg2, reg1);

    /* ------------------NASM------------------ */
    writeIndented(compiler, "imul %s, %s", reg64ToString(reg1), reg64ToString(reg2));
    writeComment(compiler, comment);
}

void write_idiv_r64(Compiler* compiler, Reg64 reg, Comment comment)
{
    ASSERT_COMPILER(compiler);

    /* ----------------BYTECODE---------------- */
    Instruction_x86_64 instruction = {};
    
    addRex(&instruction);
    instruction.rex.w = 1; /* long mode */
    instruction.rex.r = 0; /* MODRM.reg extends the opcode, hence no need in REX.r */
    instruction.rex.x = 0; /* SIB isn't used */
    updateRexB(&instruction, reg);
    
    instruction.opcode = OPCODE_IDIV_R64;

    addModrm(&instruction, 0b11);
    instruction.modrm.reg = OPCODE_IDIV_EXTENSION;
    updateModrmRm(&instruction, reg);

    writeInstruction(compiler, &instruction);

    /* ------------------NASM------------------ */
    writeIndented(compiler, "idiv %s", reg64ToString(reg));
    writeComment(compiler, comment);
}

void write_neg_r64(Compiler* compiler, Reg64 reg, Comment comment)
{
    ASSERT_COMPILER(compiler);

    /* ----------------BYTECODE---------------- */
    Instruction_x86_64 instruction = {};
    
    addRex(&instruction);
    instruction.rex.w = 1; /* long mode */
    instruction.rex.r = 0; /* MODRM.reg extends the opcode, hence no need in REX.r */
    instruction.rex.x = 0; /* SIB isn't used */
    updateRexB(&instruction, reg);
    
    instruction.opcode = OPCODE_NEG_R64;

    addModrm(&instruction, 0b11);
    instruction.modrm.reg = OPCODE_NEG_EXTENSION;
    updateModrmRm(&instruction, reg);

    writeInstruction(compiler, &instruction);

    /* ------------------NASM------------------ */
    writeIndented(compiler, "neg %s", reg64ToString(reg));
    writeComment(compiler, comment);
}

void write_sal_r64_imm8(Compiler* compiler, Reg64 reg, int8_t imm, Comment comment)
{
    ASSERT_COMPILER(compiler);

    /* ----------------BYTECODE---------------- */
    Instruction_x86_64 instruction = {};
    
    addRex(&instruction);
    instruction.rex.w = 1; /* long mode */
    instruction.rex.r = 0; /* MODRM.reg extends the opcode, hence no need in REX.r */
    instruction.rex.x = 0; /* SIB isn't used */
    updateRexB(&instruction, reg);
    
    instruction.opcode = OPCODE_SAL_R64_IMM8;

    addModrm(&instruction, 0b11);
    instruction.modrm.reg = OPCODE_SAL_EXTENSION;
    updateModrmRm(&instruction, reg);

    instruction.dispSize   = 1;
    instruction.disp.disp8 = imm;

    writeInstruction(compiler, &instruction);

    /* ------------------NASM------------------ */
    writeIndented(compiler, "sal %s, %" PRId8, reg64ToString(reg), imm);
    writeComment(compiler, comment);
}
//=================================ARITHMETIC===================================


//================================CONTROL_FLOW==================================
void write_syscall(Compiler* compiler, Comment comment)
{
    ASSERT_COMPILER(compiler);

    /* ----------------BYTECODE---------------- */
    Instruction_x86_64 instruction = {};
    instruction.opcode = OPCODE_SYSCALL;
    writeInstruction(compiler, &instruction);

    /* ------------------NASM------------------ */
    writeIndented(compiler, "syscall");
    writeComment(compiler, comment);
}

void write_call_rel32(Compiler* compiler, Label label, Comment comment)
{
    ASSERT_COMPILER(compiler);

    /* ----------------BYTECODE---------------- */
    write_jump_rel32(compiler, OPCODE_CALL_REL32, label.offset);

    /* ------------------NASM------------------ */
    writeIndented(compiler, "call %s", label.name);
    if (label.number != -1)
    {
        write(compiler, "%d", label.number);
    }

    writeComment(compiler, comment);
}

void write_ret(Compiler* compiler, Comment comment)
{
    ASSERT_COMPILER(compiler);

    /* ----------------BYTECODE---------------- */
    Instruction_x86_64 instruction = {};
    instruction.opcode = OPCODE_RET;
    writeInstruction(compiler, &instruction);

    /* ------------------NASM------------------ */
    writeIndented(compiler, "ret");
    writeComment(compiler, comment);
}

void write_jmp_rel32(Compiler* compiler, Label label, Comment comment)
{
    ASSERT_COMPILER(compiler);

    /* ----------------BYTECODE---------------- */
    write_jump_rel32(compiler, OPCODE_JMP_REL32, label.offset);

    /* ------------------NASM------------------ */
    writeIndented(compiler, "jmp %s", label.name);
    if (label.number != -1)
    {
        write(compiler, "%d", label.number);
    }

    writeComment(compiler, comment);
}

void write_jz_rel32(Compiler* compiler, Label label, Comment comment)
{
    ASSERT_COMPILER(compiler);

    /* ----------------BYTECODE---------------- */
    write_jump_rel32(compiler, OPCODE_JZ_REL32, label.offset);

    /* ------------------NASM------------------ */
    writeIndented(compiler, "jz %s", label.name);
    if (label.number != -1)
    {
        write(compiler, "%d", label.number);
    }

    writeComment(compiler, comment);
}

void write_je_rel32(Compiler* compiler, Label label, Comment comment)
{
    /* ----------------BYTECODE---------------- */
    write_jump_rel32(compiler, OPCODE_JE_REL32, label.offset);

    /* ------------------NASM------------------ */
    writeIndented(compiler, "je %s", label.name);
    if (label.number != -1)
    {
        write(compiler, "%d", label.number);
    }

    writeComment(compiler, comment);
}

void write_jne_rel32(Compiler* compiler, Label label, Comment comment)
{
    /* ----------------BYTECODE---------------- */
    write_jump_rel32(compiler, OPCODE_JNE_REL32, label.offset);

    /* ------------------NASM------------------ */
    writeIndented(compiler, "jne %s", label.name);
    if (label.number != -1)
    {
        write(compiler, "%d", label.number);
    }

    writeComment(compiler, comment);
}

void write_jl_rel32(Compiler* compiler, Label label, Comment comment)
{
    /* ----------------BYTECODE---------------- */
    write_jump_rel32(compiler, OPCODE_JL_REL32, label.offset);

    /* ------------------NASM------------------ */
    writeIndented(compiler, "jl %s", label.name);
    if (label.number != -1)
    {
        write(compiler, "%d", label.number);
    }

    writeComment(compiler, comment);
}

void write_jg_rel32(Compiler* compiler, Label label, Comment comment)
{
    /* ----------------BYTECODE---------------- */
    write_jump_rel32(compiler, OPCODE_JG_REL32, label.offset);

    /* ------------------NASM------------------ */
    writeIndented(compiler, "jg %s", label.name);
    if (label.number != -1)
    {
        write(compiler, "%d", label.number);
    }

    writeComment(compiler, comment);
}

void write_jle_rel32(Compiler* compiler, Label label, Comment comment)
{
    /* ----------------BYTECODE---------------- */
    write_jump_rel32(compiler, OPCODE_JLE_REL32, label.offset);

    /* ------------------NASM------------------ */
    writeIndented(compiler, "jle %s", label.name);
    if (label.number != -1)
    {
        write(compiler, "%d", label.number);
    }

    writeComment(compiler, comment);
}

void write_jge_rel32(Compiler* compiler, Label label, Comment comment)
{
    /* ----------------BYTECODE---------------- */
    write_jump_rel32(compiler, OPCODE_JGE_REL32, label.offset);

    /* ------------------NASM------------------ */
    writeIndented(compiler, "jge %s", label.name);
    if (label.number != -1)
    {
        write(compiler, "%d", label.number);
    }

    writeComment(compiler, comment);
}
//================================CONTROL_FLOW==================================


//====================================MOVE======================================
void write_mov_r64_r64(Compiler* compiler, Reg64 dest, Reg64 src, Comment comment)
{
    ASSERT_COMPILER(compiler);

    /* ----------------BYTECODE---------------- */
    write_instruction_r64_r64(compiler, OPCODE_MOV_R64_R64, dest, src);

    /* ------------------NASM------------------ */
    writeIndented(compiler, "mov %s, %s", reg64ToString(dest), reg64ToString(src));
    writeComment(compiler, comment);
}

void write_mov_r64_imm64(Compiler* compiler, Reg64 dest, int64_t imm, Comment comment)
{
    ASSERT_COMPILER(compiler);

    /* ----------------BYTECODE---------------- */
    Instruction_x86_64 instruction = {};
    
    addRex(&instruction);
    instruction.rex.w = 1; /* long mode */
    instruction.rex.r = 0; /* MODRM.reg extends the opcode, hence no need in REX.r */
    instruction.rex.x = 0; /* SIB isn't used */
    updateRexB(&instruction, dest);
    
    instruction.opcode           = OPCODE_BASE_MOV_R64_IMM64;
    instruction.opcode.bytes[0] += regSpecifier(dest);

    instruction.dispSize    = 8;
    instruction.disp.disp64 = imm;

    writeInstruction(compiler, &instruction);

    /* ------------------NASM------------------ */
    writeIndented(compiler, "mov %s, %" PRId64, reg64ToString(dest), imm);
    writeComment(compiler, comment);
}

void write_mov_r64_imm64(Compiler* compiler, Reg64 dest, Label label, Comment comment)
{
    ASSERT_COMPILER(compiler);

    /* ----------------BYTECODE---------------- */
    Instruction_x86_64 instruction = {};
    
    addRex(&instruction);
    instruction.rex.w = 1; /* long mode */
    instruction.rex.r = 0; /* MODRM.reg extends the opcode, hence no need in REX.r */
    instruction.rex.x = 0; /* SIB isn't used */
    updateRexB(&instruction, dest);
    
    instruction.opcode           = OPCODE_BASE_MOV_R64_IMM64;
    instruction.opcode.bytes[0] += regSpecifier(dest);

    instruction.dispSize    = 8;
    instruction.disp.disp64 = label.offset + VIRTUAL_ADDRESS_START;

    writeInstruction(compiler, &instruction);

    /* ------------------NASM------------------ */
    writeIndented(compiler, "mov %s, %s", reg64ToString(dest), label.name);
    if (label.number != -1)
    {
        write(compiler, "%d", label.number);
    }

    writeComment(compiler, comment);
}

void write_mov_m64_r64(Compiler* compiler, Mem64 dest, Reg64 src, Comment comment)
{
    ASSERT_COMPILER(compiler);

    /* ----------------BYTECODE---------------- */
    Instruction_x86_64 instruction = {};
    setMemoryAddressing(&instruction, dest);
    
    updateRexR(&instruction, src);
    instruction.opcode = OPCODE_MOV_M64_R64;
    updateModrmReg(&instruction, src);

    writeInstruction(compiler, &instruction);

    /* ------------------NASM------------------ */
    writeIndented(compiler, "mov [");
    writeMem64(compiler, dest);
    write(compiler, "], %s", reg64ToString(src));

    writeComment(compiler, comment);
}

void write_mov_r64_m64(Compiler* compiler, Reg64 dest, Mem64 src, Comment comment)
{
    ASSERT_COMPILER(compiler);

    /* ----------------BYTECODE---------------- */
    Instruction_x86_64 instruction = {};
    setMemoryAddressing(&instruction, src);
    
    updateRexR(&instruction, dest);
    instruction.opcode = OPCODE_MOV_R64_M64;
    updateModrmReg(&instruction, dest);

    writeInstruction(compiler, &instruction);

    /* ------------------NASM------------------ */
    writeIndented(compiler, "mov %s, [", reg64ToString(dest));
    writeMem64(compiler, src);
    write(compiler, "]");
    writeComment(compiler, comment);
}
//====================================MOVE======================================