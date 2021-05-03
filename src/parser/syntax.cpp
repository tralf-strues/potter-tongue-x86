#include <assert.h>
#include <string.h>
#include "syntax.h"

const char* mathOpToString(MathOp operation)
{
    switch (operation)
    {
        case ADD_OP:           { return "+";  }
        case SUB_OP:           { return "-";  }    
        case MUL_OP:           { return "*";  }
        case DIV_OP:           { return "/";  }

        case EQUAL_OP:         { return "=="; }
        case NOT_EQUAL_OP:     { return "!="; }
        case LESS_OP:          { return "<";  }
        case GREATER_OP:       { return ">";  }
        case LESS_EQUAL_OP:    { return "<="; }
        case GREATER_EQUAL_OP: { return ">="; }

        default:               { return "invalid operation"; }
    }

    return "invalid operation";
}

const char* keywordCodeToString(KeywordCode keywordCode)
{
    if (keywordCode == INVALID_KEYWORD) { return nullptr; }

    return KEYWORDS[keywordCode].codeString;    
}

const char* getKeywordString(KeywordCode keywordCode) 
{
    return KEYWORDS[keywordCode].string;
}

const StdFunctionInfo* getStdFunctionInfo(KeywordCode keywordCode)
{
    if (keywordCode < SCAN_FLOAT_KEYWORD || keywordCode > SQRT_KEYWORD)
    {
        return nullptr;
    }

    return &STANDARD_FUNCTIONS[keywordCode - SCAN_FLOAT_KEYWORD];
}

KeywordCode isStdFunction(const char* functionName)
{
    assert(functionName);

    KeywordCode code = INVALID_KEYWORD;

    for (size_t i = 0; i < STANDARD_FUNCTIONS_COUNT; i++)
    {
        if (strcmp(functionName, STANDARD_FUNCTIONS[i].workingName) == 0)
        {
            code = STANDARD_FUNCTIONS[i].code;
            break;
        }
    }

    return code;
}