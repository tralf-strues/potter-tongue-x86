#include <assert.h>

#include "x86_64_specification.h"

uint8_t regSpecifier(Reg64 reg)
{
    return RDI & reg;
}

void addRex(Instruction_x86_64* instruction)
{
    assert(instruction);

    if (!instruction->isRexUsed)
    {
        instruction->rex = DEFAULT_REX;
    }

    instruction->isRexUsed = true;
}

#define UPDATE_REX_FIELD(field) if (reg < R8)                       \
                                {                                   \
                                    if (instruction->isRexUsed)     \
                                    {                               \
                                        instruction->rex.field = 0; \
                                    }                               \
                                }                                   \
                                else                                \
                                {                                   \
                                    if (!instruction->isRexUsed)    \
                                    {                               \
                                        addRex(instruction);        \
                                    }                               \
                                                                    \
                                    instruction->rex.field = 1;     \
                                }

void updateRexR(Instruction_x86_64* instruction, Reg64 reg)
{
    assert(instruction);

    UPDATE_REX_FIELD(r);
}

void updateRexX(Instruction_x86_64* instruction, Reg64 reg)
{
    assert(instruction);

    UPDATE_REX_FIELD(x);
}

void updateRexB(Instruction_x86_64* instruction, Reg64 reg)
{
    assert(instruction);

    UPDATE_REX_FIELD(b);
}

void addModrm(Instruction_x86_64* instruction, uint8_t mod)
{
    assert(instruction);
    assert(mod <= MODRM_MOD_MAX_VALUE);

    if (!instruction->isModrmUsed)
    {
        instruction->modrm = {.mod = mod, .reg = 0, .rm = 0};
    }

    instruction->isModrmUsed = true;
}

#define UPDATE_MODRM_FIELD(field) if (!instruction->isModrmUsed)              \
                                  {                                           \
                                      addModrm(instruction, 0);               \
                                  }                                           \
                                                                              \
                                  instruction->modrm.field = regSpecifier(reg);

void updateModrmReg(Instruction_x86_64* instruction, Reg64 reg)
{
    assert(instruction);

    UPDATE_MODRM_FIELD(reg);
}

void updateModrmRm(Instruction_x86_64* instruction, Reg64 reg)
{
    assert(instruction);

    UPDATE_MODRM_FIELD(rm);
}

void addSib(Instruction_x86_64* instruction)
{
    assert(instruction);

    if (!instruction->isSibUsed)
    {
        instruction->sib = {.scale = 0, .index = 0, .base = 0};
    }

    instruction->isSibUsed = true;
}

#define UPDATE_SIB_FIELD(field) if (!instruction->isSibUsed)                \
                                {                                           \
                                    addSib(instruction);                    \
                                }                                           \
                                                                            \
                                instruction->sib.field = regSpecifier(reg);

void updateSibIndex(Instruction_x86_64* instruction, Reg64 reg)
{
    assert(instruction);

    UPDATE_SIB_FIELD(index);
}

void updateSibBase(Instruction_x86_64* instruction, Reg64 reg)
{
    assert(instruction);

    UPDATE_SIB_FIELD(base);
}

bool setMemoryAddressing(Instruction_x86_64* instruction, Mem64 memory)
{
    assert(instruction);

    bool baseUsed  = memory.base  != INVALID_REG64;
    bool dispUsed  = memory.displacement != 0;
    bool indexUsed = memory.index != INVALID_REG64;
    bool scaleUsed = isValidScale(memory.scale);

    bool baseOrBaseDisp = (baseUsed && !dispUsed && !indexUsed && !scaleUsed) || 
                          (baseUsed &&  dispUsed && !indexUsed && !scaleUsed);

    /* Registers RBP and R13 can't be used as base without displacement,
     * even though it can be equal to 0. */
    if (baseUsed && (memory.base == RBP || memory.base == R13))
    {
        dispUsed = true;
    }

    if (indexUsed && memory.index == RSP) { return false; }

    /* -------------------REX------------------ */
    addRex(instruction);
    instruction->rex.w = 1; /* long mode */
    instruction->rex.r = 0; /* not used for addressing */

    if (memory.index != INVALID_REG64)
    {
        updateRexX(instruction, memory.index);
    }

    if (memory.base != INVALID_REG64)
    {
        updateRexB(instruction, memory.base);
    }
    /* -------------------REX------------------ */

    /* ------------------MODRM----------------- */
    /* See Intel's reference for information on different modes. */
    addModrm(instruction, 0b00);

    if (baseUsed && dispUsed) 
    { 
        instruction->modrm.mod = 0b10; 
    }

    instruction->modrm.reg = 0; /* not used for addressing */

    if (baseOrBaseDisp)
    {
        updateModrmRm(instruction, memory.base);
    }
    else 
    {
        instruction->modrm.rm = RSP;
    }
    /* ------------------MODRM----------------- */

    /* -------------------SIB------------------ */
    if (baseOrBaseDisp)
    {
        if (memory.base == RSP || memory.base == R12)
        {
            addSib(instruction);
            instruction->sib.index = RSP;
            instruction->sib.base  = RSP;
        }
    }
    else 
    {
        addSib(instruction);
        instruction->sib.scale = toSibScale(memory.scale);
        
        updateSibIndex (instruction, indexUsed ? memory.index : RSP);
        updateSibBase  (instruction, baseUsed  ? memory.base  : RBP);
    }
    /* -------------------SIB------------------ */

    /* -----------------DISP32----------------- */
    if (dispUsed)
    {
        instruction->dispSize    = 4;
        instruction->disp.disp32 = memory.displacement;
    }
    /* -----------------DISP32----------------- */

    return true;
}

const char* reg64ToString(Reg64 reg)
{
    assert(reg < TOTAL_REGISTERS_64);

    return REGISTERS_64_STRINGS[reg];
}

Mem64 mem64BaseDisp(Reg64 base, int32_t displacement) 
{
    Mem64 address        = {};
    address.base         = base;
    address.displacement = displacement;
    address.index        = INVALID_REG64;
    address.scale        = 0;

    return address;
}

bool isValidScale(uint8_t scale)
{
    return scale == 2 || scale == 4 || scale == 8;
}

uint8_t toSibScale(uint8_t scale)
{
    switch (scale)
    {
        case 1:  { return 0b00; }
        case 2:  { return 0b01; }
        case 4:  { return 0b10; }
        case 8:  { return 0b11; }

        default: { return 0b00; }
    }
}