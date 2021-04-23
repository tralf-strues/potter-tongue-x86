#pragma once

#include <math.h>
#include <string.h>

static const double DOUBLE_PRECISION = 1e-9;
static const char*  LETTERS          = "abcdefghijklmnopqrstuvwxyz"
                                       "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

int    dcompare    (double num1, double num2, double precision);
int    dcompare    (double num1, double num2);
size_t digitsCount (size_t number);

char*  copyString  (const char* string, size_t length);

#ifdef UTB_DEFINITIONS

int dcompare(double num1, double num2, double precision)
{
    if (fabs(num1 - num2) < precision) { return 0; }

    if (num1 > num2) { return 1; }

    return -1;
}

int dcompare(double num1, double num2)
{
    return dcompare(num1, num2, DOUBLE_PRECISION);
}

size_t digitsCount(size_t number)
{
    size_t count = 0;

    while (number > 0)
    {
        number /= 10;
        count++;
    }

    return count;
}

char* copyString(const char* string, size_t length)
{
    assert(string != nullptr);

    char* newString = (char*) calloc(length + 1, sizeof(char));
    assert(newString != nullptr);

    strncpy(newString, string, length);
    newString[length] = '\0';

    return newString;
}

#undef UTB_DEFINITIONS
#endif