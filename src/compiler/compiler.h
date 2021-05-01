#ifndef COMPILER_H
#define COMPILER_H

#include <assert.h>
#include <stdio.h>
#include "label_manager.h"
#include "../symbol_table/symbol_table.h"
#include "../parser/expression_tree.h"
      
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

static const uint8_t COMPILER_FIRST_PASS   = 0;
static const uint8_t COMPILER_TOTAL_PASSES = 2;

struct Compiler
{
    SymbolTable*  table;
    Node*         tree;
    FILE*         file;
    Function*     curFunction;
    uint8_t       passNumber;
    LabelManager  labelManager;

    CompilerError status;
};

void          construct     (Compiler* compiler, Node* tree, SymbolTable* table);
void          destroy       (Compiler* compiler);
const char*   errorString   (CompilerError error);
CompilerError compile       (Compiler* compiler, const char* outputFile);

void          write         (Compiler* compiler, const char* format, ...);
void          writeNewLine  (Compiler* compiler);
void          writeIndented (Compiler* compiler, const char* format, ...);

#endif