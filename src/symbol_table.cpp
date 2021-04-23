#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "symbol_table.h"

const double REALLOC_MULTIPLIER     = 1.8;
const size_t DEFAULT_FUNCS_CAPACITY = 8;
const size_t DEFAULT_VARS_CAPACITY  = 16;

void reallocFunctions(SymbolTable* table);
void reallocVariables(Function* function);

void construct(SymbolTable* table)
{
    assert(table);

    table->functions = (Function*) calloc(DEFAULT_FUNCS_CAPACITY, sizeof(Function));
    assert(table->functions);

    table->functionsCount    = 0;
    table->functionsCapacity = DEFAULT_FUNCS_CAPACITY;
}

void destroy(SymbolTable* table)
{
    assert(table);

    for (size_t i = 0; i < table->functionsCount; i++)
    {
        free(table->functions[i].vars);
    }

    if (table->functions != nullptr) { free(table->functions); }

    table->functionsCount    = 0;
    table->functionsCapacity = 0;
}

Function* pushFunction(SymbolTable* table, const char* function)
{
    assert(table);
    assert(function);
    assert(table->functions);

    while (table->functionsCount >= table->functionsCapacity)
    {
        reallocFunctions(table);
    }

    table->functions[table->functionsCount].name         = function;
    table->functions[table->functionsCount].vars         = (char**) calloc(DEFAULT_VARS_CAPACITY, sizeof(char*));
    table->functions[table->functionsCount].varsCapacity = DEFAULT_VARS_CAPACITY;

    table->functionsCount++;

    return &(table->functions[table->functionsCount - 1]);
}

Function* getFunction(SymbolTable* table, const char* function)
{
    assert(table);
    assert(function);
    assert(table->functions);

    for (size_t i = 0; i < table->functionsCount; i++)
    {
        if (strcmp(table->functions[i].name, function) == 0)
        {
            return &(table->functions[i]);
        }
    }

    return nullptr;
}

void pushParameter(Function* function, const char* parameter)
{
    pushVariable(function, parameter);
    function->paramsCount++;
}

void pushVariable(Function* function, const char* variable)
{
    assert(function);
    assert(variable);
    assert(function->vars);

    while (function->varsCount >= function->varsCapacity)
    {
        reallocVariables(function);
    }

    function->vars[function->varsCount] = (char*) variable;

    function->varsCount++;
}

int getVarOffset(Function* function, const char* variable)
{
    assert(function);
    assert(variable);
    assert(function->vars);

    for (size_t i = 0; i < function->varsCount; i++)
    {
        if (strcmp(function->vars[i], variable) == 0)
        {
            return i;
        }
    }

    return -1;
}

void reallocFunctions(SymbolTable* table)
{
    assert(table);
    assert(table->functions);

    table->functionsCapacity *= REALLOC_MULTIPLIER;
    table->functions = (Function*) realloc(table->functions, table->functionsCapacity * sizeof(Function));
    assert(table->functions);
}

void reallocVariables(Function* function)
{
    assert(function);
    assert(function->vars);

    function->varsCapacity *= REALLOC_MULTIPLIER;
    function->vars = (char**) realloc(function->vars, function->varsCapacity * sizeof(char*));
    assert(function->vars);
}

void dump(SymbolTable* table)
{
    assert(table);

    printf("Symbol table:\n"
           "    functionsCapacity = %zu\n"
           "    functionsCount    = %zu\n\n"
           "    functions = { \n                  ", 
           table->functionsCapacity, 
           table->functionsCount);

    if (table->functions == nullptr)
    {
        printf("nullptr }");
    }
    else
    {
        for (size_t i = 0; i < table->functionsCount; i++)
        {
            Function* function = &(table->functions[i]);
            printf("{ name='%s', varsCapacity=%zu, varsCount=%zu, paramsCount=%zu, \n                  "
                   "  vars=[",
                   function->name, 
                   function->varsCapacity,
                   function->varsCount,
                   function->paramsCount);

            if (function->varsCount != 0)
            {
                for (size_t j = 0; j < function->varsCount; j++)
                {
                    printf("'%s', ", function->vars[j]);
                }

                printf("\b\b] }");
            }
            else 
            {
                printf("none] }");
            }

            if (i < table->functionsCount - 1)
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
}