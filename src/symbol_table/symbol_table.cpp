#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "symbol_table.h"

void reallocFunctions (SymbolTable* table);
void reallocVariables (Function* function);

int functionCmp(Function firstFunction, Function secondFunction)
{
    assert(firstFunction.name);
    assert(secondFunction.name);

    return strcmp(firstFunction.name, secondFunction.name);
}

void construct(SymbolTable* table)
{
    assert(table);

    construct(&table->functionsData, functionCmp);
}

void destroy(SymbolTable* table)
{
    assert(table);

    destroy(&table->functionsData, destroyFunction);
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

    int functionIdx = findFunction(&table->functionsData, {.name = function});

    return functionIdx != -1 ? table->functionsData.functions + functionIdx : nullptr;
}

String* pushString(SymbolTable* table, const String string)
{
    assert(table);
    assert(string.name || string.content);

    // while (table->stringsCount >= table->stringsCapacity)
    // {
    //     reallocVariables(function);
    // }

    // function->vars[function->varsCount] = (char*) variable;

    // function->varsCount++;
}

String*   getStringByName    (SymbolTable* table, const char* name);
String*   getStringByContent (SymbolTable* table, const char* string);

void dump(SymbolTable* table)
{
    assert(table);

    printf("Symbol table:\n"
           "    functionsCapacity = %zu\n"
           "    functionsCount    = %zu\n\n"
           "    functions = { \n                  ", 
           table->functionsData.capacity, 
           table->functionsData.count);

    if (table->functionsData.count == 0)
    {
        printf("nullptr }");
        return;
    }

    for (size_t i = 0; i < table->functionsData.count; i++)
    {
        Function* function = table->functionsData.functions + i;
        VarsData* varsData = &function->varsData;
        printf("{ name='%s', varsCapacity=%zu, varsCount=%zu, paramsCount=%zu, \n                  "
                "  vars=[",
                function->name, 
                varsData->capacity,
                varsData->count,
                function->paramsCount);

        if (varsData->count != 0)
        {
            for (size_t j = 0; j < varsData->count; j++)
            {
                printf("'%s', ", varsData->vars[j]);
            }

            printf("\b\b] }");
        }
        else 
        {
            printf("none] }");
        }

        if (i < table->functionsData.count - 1)
        {
            printf(",\n                  ");
        }
        else
        {
            printf("\n");
        }
    }

    printf("                }\n");
}