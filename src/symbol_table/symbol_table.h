#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdlib.h>
#include "functions_data.h"
#include "strings_data.h"

struct SymbolTable
{
    FunctionsData functionsData; 
    StringsData   stringsData;
};

void      construct          (SymbolTable* table);
void      destroy            (SymbolTable* table);
void      dump               (const SymbolTable* table);

Function* pushFunction       (SymbolTable* table, const char* function);
Function* getFunction        (SymbolTable* table, const char* function);

String*   pushString         (SymbolTable* table, String string);
String*   getStringByName    (SymbolTable* table, const char* name);
String*   getStringByContent (SymbolTable* table, const char* content);
int       getStringNumber    (SymbolTable* table, String* string);

#endif