#ifndef FUNCTION_H
#define FUNCTION_H

#include <stdlib.h>
#include <string.h>

//-----------------------------------VarsData-----------------------------------
#define STRUCT   VarsData          
#define ELEMENTS vars              
#define INSERT   insertVariable    
#define FIND     findVariable      
#define elem_t   const char*             

#undef DYNAMIC_ARRAY_H
#include "../../libs/dynamic_array/dynamic_array.h" 
#undef STRUCT             
#undef ELEMENTS      
#undef INSERT             
#undef FIND               
#undef elem_t             
//-----------------------------------VarsData-----------------------------------


//-----------------------------------Function-----------------------------------
struct Function
{
    const char* name;
    VarsData    varsData;    // local variables (including parameters!)
    size_t      paramsCount; // parameters count
};

void destroyFunction (Function* function);
void pushParameter   (Function* function, const char* parameter);
void pushVariable    (Function* function, const char* variable);
int  getVarOffset    (Function* function, const char* variable);
//-----------------------------------Function-----------------------------------


//--------------------------------FunctionsData---------------------------------
#define STRUCT   FunctionsData
#define ELEMENTS functions
#define INSERT   insertFunction
#define FIND     findFunction
#define elem_t   Function

#undef DYNAMIC_ARRAY_H
#include "../../libs/dynamic_array/dynamic_array.h"
#undef STRUCT             
#undef ELEMENTS      
#undef INSERT             
#undef FIND               
#undef elem_t

void dump(const FunctionsData* functionsData);
//--------------------------------FunctionsData---------------------------------

#endif
