#ifndef STRINGS_DATA_H
#define STRINGS_DATA_H

#include <stdlib.h>
#include <string.h>

const size_t DEFAULT_CAPACITY   = 8;
const double REALLOC_MULTIPLIER = 1.8;

struct String 
{
    const char* name;
    const char* content;
};

#define STRUCT   StringsData          
#define ELEMENTS strings              
#define INSERT   insertString    
#define FIND     findString      
#define elem_t   String             

#undef DYNAMIC_ARRAY_H
#include "../../libs/dynamic_array/dynamic_array.h" 
#undef STRUCT             
#undef ELEMENTS      
#undef INSERT             
#undef FIND               
#undef elem_t         

void dump(const StringsData* stringsData);

#endif