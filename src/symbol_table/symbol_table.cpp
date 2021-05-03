#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "symbol_table.h"

int functionCmp(Function firstFunction, Function secondFunction)
{
    assert(firstFunction.name);
    assert(secondFunction.name);

    return strcmp(firstFunction.name, secondFunction.name);
}

int stringCmpByName(String firstString, String secondString)
{
    if (firstString.name == secondString.name)                       { return 0; }
    if (firstString.name == nullptr || secondString.name == nullptr) { return 1; }

    return strcmp(firstString.name, secondString.name);
}

int stringCmpByContent(String firstString, String secondString)
{
    if (firstString.content == secondString.content)                       { return 0; }
    if (firstString.content == nullptr || secondString.content == nullptr) { return 1; }

    return strcmp(firstString.content, secondString.content);
}

void construct(SymbolTable* table)
{
    assert(table);

    construct(&table->functionsData, functionCmp);
    construct(&table->stringsData,   stringCmpByName);
}

void destroy(SymbolTable* table)
{
    assert(table);

    destroy(&table->functionsData, destroyFunction);
    destroy(&table->stringsData,   nullptr);
}

Function* pushFunction(SymbolTable* table, const char* function)
{
    assert(table);
    assert(function);

    Function newFunction = {};
    newFunction.name     = function;
    construct(&newFunction.varsData, strcmp);

    int functionIdx = insertFunction(&table->functionsData, newFunction);

    return functionIdx != -1 ? table->functionsData.functions + functionIdx : nullptr;
}

Function* getFunction(SymbolTable* table, const char* function)
{
    assert(table);
    assert(function);

    int functionIdx = findFunction(&table->functionsData, {.name = function, 
                                                           .varsData = {}, 
                                                           .paramsCount = 0});

    return functionIdx != -1 ? table->functionsData.functions + functionIdx : nullptr;
}

String* pushString(SymbolTable* table, String string)
{
    assert(table);
    assert(string.name || string.content);

    int stringIdx = insertString(&table->stringsData, string);
    return stringIdx != -1 ? table->stringsData.strings + stringIdx : nullptr;
}

String* getStringByName(SymbolTable* table, const char* name)
{
    assert(table);
    assert(name);

    table->stringsData.cmp = stringCmpByName;
    int stringIdx = findString(&table->stringsData, { name, nullptr });

    return stringIdx != -1 ? table->stringsData.strings + stringIdx : nullptr;
}

String* getStringByContent(SymbolTable* table, const char* content)
{
    assert(table);
    assert(content);

    table->stringsData.cmp = stringCmpByContent;
    int stringIdx = findString(&table->stringsData, { nullptr, content });

    return stringIdx != -1 ? table->stringsData.strings + stringIdx : nullptr;
}

int getStringNumber(SymbolTable* table, String* string)
{
    assert(table);
    assert(string);

    return string - table->stringsData.strings;
}

void dump(const SymbolTable* table)
{
    assert(table);

    printf("================ Symbol table ================\n");
    dump(&table->functionsData);

    printf("\n");
    dump(&table->stringsData);
}