#pragma once

#include <assert.h>
#include <stdio.h>
#include "symbol_table.h"
#include "expression_tree.h"
      
#define ASSERT_COMPILER(compiler) assert(compiler);              \
                                  assert(compiler->table);       \
                                  assert(compiler->tree);        \
                                  assert(compiler->file);        \

enum CompilerError
{
    COMPILER_NO_ERROR,
    COMPILER_ERROR_FILE_OPEN_FAILURE,
    COMPILER_ERROR_NO_MAIN_FUNCTION,
    COMPILER_ERROR_CALL_UNDEFINED_FUNCTION,
    COMPILER_ERROR_NO_STDIO_NASM_CODE,

    COMPILER_ERRORS_COUNT
};

static const char* COMPILER_ERROR_STRINGS[COMPILER_ERRORS_COUNT] = {
    "no error",
    "couldn't open file to write output to",
    "main function ('love') wasn't found",
    "calling undefined function",
    "could find io.nasm with standard I/O functions"
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

void          construct     (Compiler* compiler, Node* tree, SymbolTable* table);
void          destroy       (Compiler* compiler);
const char*   errorString   (CompilerError error);
CompilerError compile       (Compiler* compiler, const char* outputFile);

void          write         (Compiler* compiler, const char* format, ...);
void          writeNewLine  (Compiler* compiler);
void          writeIndented (Compiler* compiler, const char* format, ...);