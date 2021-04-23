#pragma once 
#include "syntax.h"

union TokenData
{
    char*       quotedString;
    KeywordCode keywordCode;
    int64_t     number;
    char*       id;
};

enum TokenType
{
    QUOTED_STRING_TOKEN_TYPE,
    KEYWORD_TOKEN_TYPE,
    NUMBER_TOKEN_TYPE,
    ID_TOKEN_TYPE
};

struct Token
{
    TokenType   type;
    TokenData   data;
    size_t      line;
    const char* pos;
};

struct Tokenizer
{
    const char* buffer;
    size_t      bufferSize;
    const char* position;

    bool        useNumericNumbers;

    Token*      tokens;
    size_t      tokensCount;
    size_t      currentLine;
};

void construct          (Tokenizer* tokenizer, const char* buffer, size_t bufferSize, bool useNumericNumbers);
void destroy            (Tokenizer* tokenizer);

bool isQuotedStringType (const Token* token);
bool isKeywordType      (const Token* token);
bool isNumberType       (const Token* token);
bool isIdType           (const Token* token);

bool isQuotedString     (const Token* token, const char* quotedString);
bool isKeyword          (const Token* token, KeywordCode keywordCode);
bool isNumber           (const Token* token, int64_t number);
bool isId               (const Token* token, const char* id);

bool isComparand        (const Token* token);
bool isTerm             (const Token* token);
bool isFactor           (const Token* token);
 
void tokenizeBuffer     (Tokenizer* tokenizer);
void dumpTokens         (const Token* tokens, size_t count, FILE* file);

const char* tokenTypeToString(TokenType type);
