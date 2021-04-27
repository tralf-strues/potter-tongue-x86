#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <assert.h>
#include <stdlib.h>

struct STRUCT
{
    elem_t* ELEMENTS;
    size_t  count;
    size_t  capacity;

    int (*cmp)(elem_t firstElement, elem_t secondElement);
};

void construct (STRUCT* container, int (*cmp)(elem_t firstElement, elem_t secondElement));
void destroy   (STRUCT* container, void (*destroyElem)(elem_t* element));

int  INSERT    (STRUCT* container, elem_t element);
int  FIND      (STRUCT* container, elem_t element);

#endif