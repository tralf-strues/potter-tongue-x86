#include <stdio.h>
#include "strings_data.h"

#define STRUCT   StringsData          
#define ELEMENTS strings              
#define INSERT   insertString    
#define FIND     findString      
#define elem_t   String 

#undef DYNAMIC_ARRAY_CPP
#include "../../libs/dynamic_array/dynamic_array.cpp"

#undef STRUCT             
#undef ELEMENTS      
#undef INSERT             
#undef FIND               
#undef elem_t 

void dump(const StringsData* stringsData)
{
    assert(stringsData);

    printf("stringsCapacity = %zu\n"
           "stringsCount    = %zu\n\n"
           "strings = {", 
           stringsData->capacity, 
           stringsData->count);

    if (stringsData->count == 0)
    {
        printf(" nullptr }");
        return;
    }
    
    printf("\n");

    for (size_t i = 0; i < stringsData->count; i++)
    {
        String* string = stringsData->strings + i;
        printf("    { name='%s', content='%s' },\n", 
               string->name, 
               string->content[0] == '\n' ? "\\n" : string->content);
    }

    printf("}\n");
}