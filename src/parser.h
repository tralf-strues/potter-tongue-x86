#pragma once

#include "tokenizer.h"
#include "expression_tree.h"
#include "symbol_table.h"

enum ParseError
{
    PARSE_NO_ERROR,

    PARSE_ERROR_NO_PROG_START,
    PARSE_ERROR_NO_PROG_END,
    PARSE_ERROR_NEW_LINE_NEEDED,

    PARSE_ERROR_FUNCTION_DECLARATION_NEEDED,
    PARSE_ERROR_FUNCTION_SECOND_DECLARATION,
    PARSE_ERROR_FUNCTION_PARAMS_NEEDED,
    PARSE_ERROR_FUNCTION_BODY_NEEDED,
    PARSE_ERROR_RETURN_EXPRESSION_NEEDED,

    PARSE_ERROR_FUNCTION_CALL_PARAMS_NEEDED,
    PARSE_ERROR_FUNCTION_CALL_EXTRA_PARAMS,

    PARSE_ERROR_PRINT_EXPRESSION_NEEDED,
    PARSE_ERROR_FLOOR_EXPRESSION_NEEDED,

    PARSE_ERROR_IF_EXPRESSION_NEEDED,
    PARSE_ERROR_IF_BLOCK_NEEDED,
    PARSE_ERROR_ELSE_BLOCK_NEEDED,

    PARSE_ERROR_LOOP_EXPRESSION_NEEDED,
    PARSE_ERROR_LOOP_BLOCK_NEEDED,

    PARSE_ERROR_VARIABLE_DECLARATION_NO_ASSIGNMENT,
    PARSE_ERROR_VARIABLE_SECOND_DECLARATION,
    PARSE_ERROR_VARIABLE_UNDECLARED_USAGE,
    PARSE_ERROR_VARIABLE_ASSIGNMENT_NO_EXPRESSION,
    PARSE_ERROR_DEREFERENCING_NO_VARIABLE,

    PARSE_ERROR_OPEN_BRACE_NEEDED,
    PARSE_ERROR_CLOSE_BRACE_NEEDED,
    PARSE_ERROR_BRACKET_NEEDED,

    PARSE_ERROR_ID_NEEDED,
    PARSE_ERROR_INVALID_ID,

    PARSE_ERROR_COMPARAND_NOT_FOUND,
    PARSE_ERROR_TERM_NOT_FOUND,
    PARSE_ERROR_FACTOR_NOT_FOUND,

    PARSE_ERROR_INVALID_COMPARISON_OPERATION,
    PARSE_ERROR_INVALID_TERM_OPERATION,
    PARSE_ERROR_INVALID_FACTOR_OPERATION,

    PARSE_ERROR_NO_EXPRESSION_INSIDE_BRACKETS,

    PARSE_ERROR_NO_STRING_AFTER_QUOTE,
    PARSE_ERROR_NO_SECOND_QUOTE_AFTER_STRING,
    PARSE_ERROR_NO_CLOSE_STRING_ID,
    PARSE_ERROR_NO_STRING_ID_IN_DECLARATION,
    PARSE_ERROR_NO_QUOTED_STRING_IN_DECLARATION,
    PARSE_ERROR_STRING_DECLARATION_AFTER_FIRST_FUNCTION_DECLARATION,

    TOTAL_PARSE_ERRORS
};

static const char* PARSE_ERROR_STRINGS[TOTAL_PARSE_ERRORS] = {
    "no error",

    "no program start ('Godric's-Hollow') found",
    "no program end ('Privet-Drive') found",
    "new line needed",

    "no function declaration found ('imperio')",
    "second declaration of the function",
    "couldn't find function's parameters",
    "couldn't find function's body",
    "couldn't find an expression after 'reverte' operator",

    "function call: not enough parameters passed",
    "function call: too many parameters passed",

    "couldn't find an expression after 'flagrate' operator",
    "couldn't find an expression after 'colloshoo' operator",

    "revelio operator needs a condition",
    "couldn't find revelio operator's body",
    "couldn't find otherwise operator's body",

    "while operator needs a condition",
    "couldn't find while operator's body",

    "no assignment after declaring a variable",
    "second declaration of the variable",
    "the variable hasn't been defined",
    "no expression after assignment operator 'carpe-retractum'",
    "'legilimens' isn't followed by a variable",

    "couldn't find alohomora",
    "couldn't find colloportus",
    "couldn't find protego",

    "couldn't find a name",
    "invalid name",

    "couldn't find a comparand",
    "couldn't find a term",
    "couldn't find a factor",

    "there's no such comparison operation",
    "there's no such term operation",
    "there's no such factor operation",

    "an expression inside protegos is needed",

    "after an opening quote \" there has to be a string",
    "quoted string has to have a closing \" after a string",

    "global string reference has to end with '>>'",
    "there has to be <<StringName>> after 'Chapter' in global string declaration",
    "there has to be \"String\" after <<StringName>>  in global string declaration",
    "all global string have to be declared before any function declarations"
};

struct Parser
{
    Tokenizer*   tokenizer;
    size_t       offset;
    ParseError   status;

    SymbolTable* table;
    Function*    curFunction;
};

void        construct    (Parser* parser, Tokenizer* tokenizer);
void        destroy      (Parser* parser);
const char* errorString  (ParseError error);
ParseError  parseProgram (Parser* parser, SymbolTable* table, Node** root);

