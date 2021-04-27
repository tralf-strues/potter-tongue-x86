#pragma once

#include <stdlib.h>
#include "function.h"

struct String 
{
    const char* name;
    const char* content;
};

struct SymbolTable
{
    FunctionsData functionsData;

    String*   strings;
    size_t    stringsCapacity;
    size_t    stringsCount;
};

void      construct          (SymbolTable* table);
void      destroy            (SymbolTable* table);
void      dump               (SymbolTable* table);

Function* pushFunction       (SymbolTable* table, const char* function);
Function* getFunction        (SymbolTable* table, const char* function);

String*   pushString         (SymbolTable* table, const String string);
String*   getStringByName    (SymbolTable* table, const char* name);
String*   getStringByContent (SymbolTable* table, const char* content);