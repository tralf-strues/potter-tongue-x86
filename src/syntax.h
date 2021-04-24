#pragma once
#include <stdlib.h>

enum MathOp
{
    ADD_OP,
    SUB_OP,
    MUL_OP,
    DIV_OP,

    EQUAL_OP,
    NOT_EQUAL_OP,
    LESS_EQUAL_OP,
    GREATER_EQUAL_OP,
    LESS_OP,
    GREATER_OP
};

enum KeywordCode
{
    PROG_START_KEYWORD,
    PROG_END_KEYWORD,
    NEW_LINE_KEYWORD,

    COMMENT_KEYWORD,

    FDECL_KEYWORD,
    VDECL_KEYWORD,

    ZERO_KEYWORD,
    TWO_KEYWORD,
    THREE_KEYWORD,
    SIX_KEYWORD,

    CALL_KEYWORD,
    RETURN_KEYWORD,

    OPEN_BRACE_KEYWORD,
    CLOSE_BRACE_KEYWORD,

    BRACKET_KEYWORD,
    COMMA_KEYWORD,

    CMD_LINE_KEYWORD,

    IF_KEYWORD,
    ELSE_KEYWORD,
    LOOP_KEYWORD,

    ASSIGN_KEYWORD,
    DEREF_KEYWORD,

    SCAN_FLOAT_KEYWORD,
    SCAN_KEYWORD,
    PRINT_FLOAT_KEYWORD,
    PRINT_KEYWORD,
    FLOOR_KEYWORD,
    SQRT_KEYWORD,

    RAND_JUMP_KEYWORD,

    PLUS_KEYWORD,
    MINUS_KEYWORD,
    MUL_KEYWORD,
    DIV_KEYWORD,

    EQUAL_KEYWORD,
    NOT_EQUAL_KEYWORD,
    LESS_EQUAL_KEYWORD,
    GREATER_EQUAL_KEYWORD,
    LESS_KEYWORD,
    GREATER_KEYWORD,

    ADECL_KEYWORD,
    MEM_ACCESS_KEYWORD,

    STR_QUOTE_KEYWORD,
    SDECL_KEYWORD,
    STR_ID_OPEN_KEYWORD,
    STR_ID_CLOSE_KEYWORD,
    STR_NEW_LINE_KEYWORD,

    KEYWORDS_COUNT,
    INVALID_KEYWORD = -1
};

struct Keyword
{
    const char* string;
    size_t      length;
    KeywordCode code;
    const char* codeString;
};

static const char* MAIN_FUNCTION_NAME = "love";

#define TO_STR(keywordCode) #keywordCode

static const Keyword KEYWORDS[KEYWORDS_COUNT] = {
    { "Godric's-Hollow",   15, PROG_START_KEYWORD,    TO_STR(PROG_START_KEYWORD)    },
    { "Privet-Drive",      12, PROG_END_KEYWORD,      TO_STR(PROG_END_KEYWORD)      },
    { "\n",                1,  NEW_LINE_KEYWORD,      TO_STR(NEW_LINE_KEYWORD)      },

    { "(oNo)",             5,  COMMENT_KEYWORD,       TO_STR(COMMENT_KEYWORD)       },

    { "imperio",           7,  FDECL_KEYWORD,         TO_STR(FDECL_KEYWORD)         },
    { "avenseguim",        10, VDECL_KEYWORD,         TO_STR(VDECL_KEYWORD)         },

    { "horcrux",           7,  ZERO_KEYWORD,          TO_STR(ZERO_KEYWORD)          },
    { "duo",               3,  TWO_KEYWORD,           TO_STR(TWO_KEYWORD)           },
    { "tria",              4,  THREE_KEYWORD,         TO_STR(THREE_KEYWORD)         },
    { "maxima",            6,  SIX_KEYWORD,           TO_STR(SIX_KEYWORD)           },

    { "depulso",           7,  CALL_KEYWORD,          TO_STR(CALL_KEYWORD)          },
    { "reverte",           7,  RETURN_KEYWORD,        TO_STR(RETURN_KEYWORD)        },

    { "alohomora",         9,  OPEN_BRACE_KEYWORD,    TO_STR(OPEN_BRACE_KEYWORD)    },
    { "colloportus",       11, CLOSE_BRACE_KEYWORD,   TO_STR(CLOSE_BRACE_KEYWORD)   },

    { "protego",           7,  BRACKET_KEYWORD,       TO_STR(BRACKET_KEYWORD)       },
    { ",",                 1,  COMMA_KEYWORD,         TO_STR(COMMA_KEYWORD)         },

    { "-",                 1,  CMD_LINE_KEYWORD,      TO_STR(CMD_LINE_KEYWORD)      },

    { "revelio",           7,  IF_KEYWORD,            TO_STR(IF_KEYWORD)            },
    { "otherwise",         9,  ELSE_KEYWORD,          TO_STR(ELSE_KEYWORD)          },
    { "while",             5,  LOOP_KEYWORD,          TO_STR(LOOP_KEYWORD)          },

    { "carpe-retractum",   15, ASSIGN_KEYWORD,        TO_STR(ASSIGN_KEYWORD)        },
    { "legilimens",        10, DEREF_KEYWORD,         TO_STR(DEREF_KEYWORD)         },

    { "accio-bombarda",    14, SCAN_FLOAT_KEYWORD,    TO_STR(SCAN_FLOAT_KEYWORD)    },
    { "accio",             5,  SCAN_KEYWORD,          TO_STR(SCAN_KEYWORD)          },
    { "flagrate-bombarda", 17, PRINT_FLOAT_KEYWORD,   TO_STR(PRINT_FLOAT_KEYWORD)   },
    { "flagrate",          8,  PRINT_KEYWORD,         TO_STR(PRINT_KEYWORD)         },
    { "colloshoo",         9,  FLOOR_KEYWORD,         TO_STR(FLOOR_KEYWORD)         },
    { "crucio",            6,  SQRT_KEYWORD,          TO_STR(SQRT_KEYWORD)          },

    { "riddikulus",        10, RAND_JUMP_KEYWORD,     TO_STR(RAND_JUMP_KEYWORD)     },

    { "epoximise",         9,  PLUS_KEYWORD,          TO_STR(PLUS_KEYWORD)          },
    { "flipendo",          8,  MINUS_KEYWORD,         TO_STR(MINUS_KEYWORD)         },
    { "geminio",           7,  MUL_KEYWORD,           TO_STR(MUL_KEYWORD)           },
    { "sectumsempra",      12, DIV_KEYWORD,           TO_STR(DIV_KEYWORD)           },

    { "equal",             5,  EQUAL_KEYWORD,         TO_STR(EQUAL_KEYWORD)         },
    { "not-equal",         9,  NOT_EQUAL_KEYWORD,     TO_STR(NOT_EQUAL_KEYWORD)     },
    { "less-equal",        10, LESS_EQUAL_KEYWORD,    TO_STR(LESS_EQUAL_KEYWORD)    },
    { "greater-equal",     13, GREATER_EQUAL_KEYWORD, TO_STR(GREATER_EQUAL_KEYWORD) }, 
    { "less",              4,  LESS_KEYWORD,          TO_STR(LESS_KEYWORD)          },
    { "greater",           7,  GREATER_KEYWORD,       TO_STR(GREATER_KEYWORD)       },
    
    { "capacious",         9,  ADECL_KEYWORD,         TO_STR(ADECL_KEYWORD)         },
    { "~",                 1,  MEM_ACCESS_KEYWORD,    TO_STR(MEM_ACCESS_KEYWORD)    },
    
    { "\"",                1,  STR_QUOTE_KEYWORD,     TO_STR(STR_QUOTE_KEYWORD)     },
    { "Chapter",           7,  SDECL_KEYWORD,         TO_STR(SDECL_KEYWORD)         },
    { "<<",                2,  STR_ID_OPEN_KEYWORD,   TO_STR(STR_ID_OPEN_KEYWORD)   },
    { ">>",                2,  STR_ID_CLOSE_KEYWORD,  TO_STR(STR_ID_CLOSE_KEYWORD)  },
    { "circumrota",        10, STR_NEW_LINE_KEYWORD,  TO_STR(STR_NEW_LINE_KEYWORD)  }
};

const char* mathOpToString      (MathOp operation);
const char* keywordCodeToString (KeywordCode keywordCode);
const char* getKeywordString    (KeywordCode keywordCode);