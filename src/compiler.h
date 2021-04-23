#pragma once

#include <stdio.h>
#include "symbol_table.h"
#include "expression_tree.h"

enum CompilerError
{
    COMPILER_NO_ERROR,
    COMPILER_ERROR_FILE_OPEN_FAILURE,
    COMPILER_ERROR_NO_MAIN_FUNCTION,
    COMPILER_ERROR_CALL_UNDEFINED_FUNCTION,

    COMPILER_ERRORS_COUNT
};

static const char* COMPILER_ERROR_STRINGS[COMPILER_ERRORS_COUNT] = {
    "no error",
    "couldn't open file to write output to",
    "main function ('love') wasn't found",
    "calling undefined function"
};

struct Compiler
{
    SymbolTable*  table;
    Node*         tree;
    FILE*         file;
    Function*     curFunction;

    size_t        curCondLabel;
    size_t        curLoopLabel;
    size_t        curCmpLabel;

    CompilerError status;
};

void          construct   (Compiler* compiler, Node* tree, SymbolTable* table);
void          destroy     (Compiler* compiler);
const char*   errorString (CompilerError error);
CompilerError compile     (Compiler* compiler, const char* outputFile);