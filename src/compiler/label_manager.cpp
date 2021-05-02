#include <string.h>
#include "label_manager.h"

const size_t DEFAULT_CAPACITY   = 8;
const double REALLOC_MULTIPLIER = 1.8;

#define STRUCT   LabelArray
#define ELEMENTS labels
#define INSERT   insertLabel
#define FIND     findLabel
#define elem_t   Label

#undef DYNAMIC_ARRAY_CPP
#include "../../libs/dynamic_array/dynamic_array.cpp"

#undef STRUCT             
#undef ELEMENTS      
#undef INSERT             
#undef FIND               
#undef elem_t 

int cmpLabels(Label firstLabel, Label secondLabel)
{
    assert(firstLabel.name);
    assert(secondLabel.name);

    bool firstHasFunctionName  = firstLabel.functionName  != nullptr; 
    bool secondHasFunctionName = secondLabel.functionName != nullptr; 

    if (firstHasFunctionName != secondHasFunctionName) { return -1; }

    if (firstHasFunctionName && 
        (strcmp(firstLabel.functionName, secondLabel.functionName) != 0))
    {
        return -1;
    }

    if (strcmp(firstLabel.name, secondLabel.name) != 0) 
    { 
        return -1;
    }

    return secondLabel.number - firstLabel.number;
}

void construct(LabelManager* labelManager)
{
    assert(labelManager);
    
    resetLabelNumbers(labelManager);
    construct(&labelManager->labelArray, cmpLabels);
}

void destroy(LabelManager* labelManager)
{
    assert(labelManager);

    destroy(&labelManager->labelArray, nullptr);
}

void resetLabelNumbers(LabelManager* labelManager)
{
    assert(labelManager);

    for (uint32_t i = 0; i < TOTAL_LABELS; i++)
    {
        labelManager->curLabelNumbers[i] = 0;
    }
}