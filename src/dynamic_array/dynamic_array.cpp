#ifndef DYNAMIC_ARRAY_CPP
#define DYNAMIC_ARRAY_CPP

#include "dynamic_array.h" 

void realloc(STRUCT* container);

void construct(STRUCT* container, int (*cmp)(elem_t firstElement, elem_t secondElement))
{
    assert(container);
    assert(cmp);

    container->ELEMENTS = (elem_t*) calloc(DEFAULT_CAPACITY, sizeof(elem_t));
    assert(container->ELEMENTS);

    container->count    = 0;
    container->capacity = DEFAULT_CAPACITY;
    container->cmp      = cmp;  
}

void destroy(STRUCT* container, void (*destroyElem)(elem_t* element))
{
    assert(container);

    if (destroyElem != nullptr)
    {
        for (size_t i = 0; i < container->count; i++)
        {
            destroyElem(container->ELEMENTS + i);
        }
    }

    if (container->ELEMENTS != nullptr) { free(container->ELEMENTS); }

    container->count    = 0;
    container->capacity = 0;
    container->cmp      = 0;
}

void realloc(STRUCT* container)
{
    assert(container);

    container->capacity     *= REALLOC_MULTIPLIER;
    container->ELEMENTS = (elem_t*) realloc(container->ELEMENTS, 
                                             container->capacity * sizeof(elem_t));
    assert(container->ELEMENTS);
}

int INSERT(STRUCT* container, elem_t element)
{
    assert(container);

    while (container->count >= container->capacity)
    {
        realloc(container);
    }

    container->ELEMENTS[container->count++] = element;
}

int FIND(STRUCT* container, elem_t element)
{
    assert(container);
    assert(container->ELEMENTS);

    for (int i = 0; i < container->count; i++)
    {
        if (container->cmp(container->ELEMENTS[i], element) == 0)
        {
            return i;
        }
    }

    return -1;
}

#undef construct
#undef destroy

#endif