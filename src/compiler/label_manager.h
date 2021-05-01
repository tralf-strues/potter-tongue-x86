#ifndef LABEL_H
#define LABEL_H

#include <stdint.h>

enum LabelPurposeType
{
    LABEL_COND,
    LABEL_LOOP,
    LABEL_CMP,
    LABEL_RET,

    TOTAL_LABELS
};

struct Label
{
    /* Label's offset in the binary file. */
    uint64_t offset; 

    /* Label's name in the format: 
     * <functionName><name><number> (e.g. "main.WHILE_9").
     * If number is -1, then it is not used. */
    const char* functionName;
    const char* name;
    int32_t     number;
};

#define STRUCT   LabelArray
#define ELEMENTS labels
#define INSERT   insertLabel
#define FIND     findLabel
#define elem_t   Label

#undef DYNAMIC_ARRAY_H
#include "../../libs/dynamic_array/dynamic_array.h"
#undef STRUCT             
#undef ELEMENTS      
#undef INSERT             
#undef FIND               
#undef elem_t

struct LabelManager
{
    LabelArray labelArray;
    int32_t    curLabelNumbers[TOTAL_LABELS];
};

int  cmpLabels (Label firstLabel, Label secondLabel);
void construct (LabelManager* labelManager);
void destroy   (LabelManager* labelManager);

#endif