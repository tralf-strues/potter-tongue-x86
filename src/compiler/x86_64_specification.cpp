#include <assert.h>

#include "x86_64_specification.h"

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