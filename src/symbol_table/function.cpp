#include "function.h"

const size_t DEFAULT_CAPACITY   = 8;
const double REALLOC_MULTIPLIER = 1.8;

//-----------------------------------VarsData-----------------------------------
#define STRUCT   VarsData          
#define ELEMENTS vars              
#define INSERT   insertVariable    
#define FIND     findVariable      
#define elem_t   const char*  

#undef DYNAMIC_ARRAY_CPP
#include "../dynamic_array/dynamic_array.cpp"

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

    destroy(&function->varsData, (void (*) (const char**)) free);
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

    // FIXME: add isParam function or something similar
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
#include "../dynamic_array/dynamic_array.cpp"

#undef STRUCT             
#undef ELEMENTS      
#undef INSERT             
#undef FIND               
#undef elem_t 
//--------------------------------FunctionsData---------------------------------