#pragma once

#include <stdlib.h>

struct Function
{
    const char* name;

    char**      vars;
    size_t      varsCapacity;
    size_t      varsCount;   // local variables count (including parameters!)
    size_t      paramsCount; // parameters count
};

struct SymbolTable
{
    Function* functions;
    size_t    functionsCapacity;
    size_t    functionsCount;
};

void      construct     (SymbolTable* table);
void      destroy       (SymbolTable* table);
void      dump          (SymbolTable* table);

Function* pushFunction  (SymbolTable* table, const char* function);
Function* getFunction   (SymbolTable* table, const char* function);

void      pushParameter (Function* function, const char* parameter);
void      pushVariable  (Function* function, const char* variable);
int       getVarOffset  (Function* function, const char* variable);
