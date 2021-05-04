#include <stdio.h>
#include "functions_data.h"

const size_t DEFAULT_CAPACITY   = 8;
const double REALLOC_MULTIPLIER = 1.8;

//-----------------------------------VarsData-----------------------------------
#define STRUCT   VarsData          
#define ELEMENTS vars              
#define INSERT   insertVariable    
#define FIND     findVariable      
#define elem_t   const char*  

#undef DYNAMIC_ARRAY_CPP
#include "../../libs/dynamic_array/dynamic_array.cpp"

#undef STRUCT             
#undef ELEMENTS      
#undef INSERT             
#undef FIND               
#undef elem_t 
//-----------------------------------VarsData-----------------------------------


//-----------------------------------Function-----------------------------------
void destroyFunction(Function* function)
{
    assert(function);

    destroy(&function->varsData, nullptr);
    function->name        = nullptr;
    function->paramsCount = 0;
}

void pushParameter(Function* function, const char* parameter)
{
    assert(function);
    pushVariable(function, parameter);
    function->paramsCount++;
}

void pushVariable(Function* function, const char* variable)
{
    assert(function);
    assert(variable);
    assert(function->varsData.vars);

    insertVariable(&function->varsData, variable);
}

int getVarOffset(Function* function, const char* variable)
{
    assert(function);
    assert(variable);
    assert(function->varsData.vars);

    int varIndex = findVariable(&function->varsData, variable);
    if (varIndex == -1) { return -1; }

    if (varIndex < (int) function->paramsCount)
    {
        return 2 * 8 + varIndex * 8;  
    }
    else 
    {
        return -(varIndex - (int) function->paramsCount + 1) * 8;
    }

    return -1;
}
//-----------------------------------Function-----------------------------------


//--------------------------------FunctionsData---------------------------------
#define STRUCT   FunctionsData
#define ELEMENTS functions
#define INSERT   insertFunction
#define FIND     findFunction
#define elem_t   Function

#undef DYNAMIC_ARRAY_CPP
#include "../../libs/dynamic_array/dynamic_array.cpp"

#undef STRUCT             
#undef ELEMENTS      
#undef INSERT             
#undef FIND               
#undef elem_t 

void dump(const FunctionsData* functionsData)
{
    assert(functionsData);

    printf("functionsCount = %zu\n"
           "functions = {\n    ", 
           functionsData->count);

    if (functionsData->count == 0)
    {
        printf("nullptr }");
        return;
    }

    for (size_t i = 0; i < functionsData->count; i++)
    {
        Function* function = functionsData->functions + i;
        VarsData* varsData = &function->varsData;
        printf("{ name='%s', varsCount=%zu, paramsCount=%zu, \n    "
               "  vars=[",
               function->name, 
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

        if (i < functionsData->count - 1)
        {
            printf(",\n\n    ");
        }
        else
        {
            printf("\n");
        }
    }

    printf("}\n");
}
//--------------------------------FunctionsData---------------------------------