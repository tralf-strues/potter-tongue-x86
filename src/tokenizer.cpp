#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "tokenizer.h"
#include "../libs/utilib.h"

#define ASSERT_TOKENIZER(tokenizer) assert((tokenizer));           \
                                    assert((tokenizer)->buffer);   \
                                    assert((tokenizer)->position); \
                                    assert((tokenizer)->tokens); 

const size_t MAX_TOKENS_COUNT = 8192;

bool    finished            (Tokenizer* tokenizer);
void    skipSpaces          (Tokenizer* tokenizer);
void    proceed             (Tokenizer* tokenizer, size_t step);
void    addToken            (Tokenizer* tokenizer, Token token);

bool    processQuotedString (Tokenizer* tokenizer);
bool    processKeyword      (Tokenizer* tokenizer);
bool    isKeywordNumber     (Keyword keyword);
int64_t keywordToNumber     (Keyword keyword);
bool    processNumeric      (Tokenizer* tokenizer);
bool    processId           (Tokenizer* tokenizer);

void construct(Tokenizer* tokenizer, const char* buffer, size_t bufferSize, bool useNumericNumbers)
{
    assert(tokenizer);

    tokenizer->buffer      = buffer;
    tokenizer->bufferSize  = bufferSize;
    tokenizer->position    = buffer;

    tokenizer->tokens      = (Token*) calloc(MAX_TOKENS_COUNT, sizeof(Token));
    tokenizer->tokensCount = 0;
    tokenizer->currentLine = 0;

    tokenizer->useNumericNumbers = useNumericNumbers;

    assert(tokenizer->tokens);
}

void destroy(Tokenizer* tokenizer)
{
    ASSERT_TOKENIZER(tokenizer);

    free(tokenizer->tokens);

    tokenizer->buffer      = nullptr;
    tokenizer->bufferSize  = 0;
    tokenizer->position    = nullptr;

    tokenizer->tokens      = nullptr;
    tokenizer->tokensCount = 0;
    tokenizer->currentLine = 0;
}

bool isQuotedStringType(const Token* token)
{
    assert(token);

    return token->type == QUOTED_STRING_TOKEN_TYPE;
}

bool isKeywordType(const Token* token)
{
    assert(token);

    return token->type == KEYWORD_TOKEN_TYPE;
}

bool isNumberType(const Token* token)
{
    assert(token);

    return token->type == NUMBER_TOKEN_TYPE;
}

bool isIdType(const Token* token)
{
    assert(token);

    return token->type == ID_TOKEN_TYPE;
}

bool isQuotedString(const Token* token, const char* quotedString)
{
    assert(token);

    return isQuotedStringType(token) && 
           strcmp(token->data.quotedString, quotedString) == 0;
}

bool isKeyword(const Token* token, KeywordCode keywordCode)
{
    assert(token);

    return isKeywordType(token) && token->data.keywordCode == keywordCode;
}

bool isNumber(const Token* token, int64_t number)
{
    assert(token);

    return isNumberType(token) && token->data.number == number;
}

bool isId(const Token* token, const char* id)
{
    assert(token);

    return isIdType(token) && strcmp(token->data.id, id) == 0;
}

bool isComparand(const Token* token)
{
    assert(token);

    return isKeywordType(token) && 
           token->data.keywordCode >= EQUAL_KEYWORD && 
           token->data.keywordCode <= GREATER_KEYWORD;
}

bool isTerm(const Token* token)
{
    assert(token);

    return isKeywordType(token) && 
           token->data.keywordCode >= PLUS_KEYWORD && 
           token->data.keywordCode <= MINUS_KEYWORD;
}

bool isFactor(const Token* token)
{
    assert(token);

    return isKeywordType(token) && 
           token->data.keywordCode >= MUL_KEYWORD && 
           token->data.keywordCode <= DIV_KEYWORD;
}

void tokenizeBuffer(Tokenizer* tokenizer)
{
    ASSERT_TOKENIZER(tokenizer);

    skipSpaces(tokenizer);

    while (!finished(tokenizer))
    {
        if (!processQuotedString(tokenizer) && 
            !processKeyword(tokenizer) && 
            !processNumeric(tokenizer) && 
            !processId(tokenizer))
        {
            break;
        }
    }
}

bool finished(Tokenizer* tokenizer)
{
    ASSERT_TOKENIZER(tokenizer);

    return tokenizer->position > tokenizer->buffer + tokenizer->bufferSize;
}

void skipSpaces(Tokenizer* tokenizer)
{
    ASSERT_TOKENIZER(tokenizer);

    tokenizer->position += strspn(tokenizer->position, " \t");
}

void proceed(Tokenizer* tokenizer, size_t step)
{
    ASSERT_TOKENIZER(tokenizer);
    
    tokenizer->position += step;
    skipSpaces(tokenizer);
}

void addToken(Tokenizer* tokenizer, Token token)
{
    ASSERT_TOKENIZER(tokenizer);

    tokenizer->tokens[tokenizer->tokensCount++] = token;
}

bool processQuotedString(Tokenizer* tokenizer)
{
    ASSERT_TOKENIZER(tokenizer);

    if (*(tokenizer->position) != '\"') { return false; }

    addToken(tokenizer, {KEYWORD_TOKEN_TYPE, 
                         {.keywordCode = STR_QUOTE_KEYWORD}, 
                         tokenizer->currentLine, 
                         tokenizer->position});
    tokenizer->position++;

    const char* quote   = strchr(tokenizer->position, '\"');
    const char* newLine = strchr(tokenizer->position, '\n');

    const char* end = quote < newLine ? quote : newLine;
    size_t length = end - tokenizer->position;

    if (length == 0) 
    {
        addToken(tokenizer, {QUOTED_STRING_TOKEN_TYPE, 
                             {.quotedString = nullptr}, 
                             tokenizer->currentLine, 
                             tokenizer->position});
    }
    else 
    {
        addToken(tokenizer, {QUOTED_STRING_TOKEN_TYPE, 
                             {.quotedString = copyString(tokenizer->position, length)}, 
                             tokenizer->currentLine, 
                             tokenizer->position});
    }

    proceed(tokenizer, length);

    if (end == quote)
    {
        addToken(tokenizer, {KEYWORD_TOKEN_TYPE, 
                             {.keywordCode = STR_QUOTE_KEYWORD}, 
                             tokenizer->currentLine, 
                             tokenizer->position});
        
        proceed(tokenizer, 1);
    }

    return true;
}

bool processKeyword(Tokenizer* tokenizer)
{
    ASSERT_TOKENIZER(tokenizer);

    for (size_t i = 0; i < KEYWORDS_COUNT; i++)
    {
        Keyword keyword = KEYWORDS[i];

        if (tokenizer->position + keyword.length > tokenizer->buffer + tokenizer->bufferSize)
        {
            continue;
        }

        if (strncmp(tokenizer->position, keyword.string, keyword.length) == 0)
        {
            if (isKeywordNumber(keyword))
            {
                addToken(tokenizer, {NUMBER_TOKEN_TYPE, {.number = keywordToNumber(keyword)}, tokenizer->currentLine, tokenizer->position});
            }
            else if (keyword.code == COMMENT_KEYWORD)
            {
                const char* newLine = strchr(tokenizer->position, '\n');

                addToken(tokenizer, {KEYWORD_TOKEN_TYPE, {.keywordCode = NEW_LINE_KEYWORD}, tokenizer->currentLine, tokenizer->position});
                    
                if (newLine != nullptr)
                {
                    proceed(tokenizer, newLine - tokenizer->position + 1);
                } 
                else
                {
                    proceed(tokenizer, tokenizer->bufferSize);
                }

                tokenizer->currentLine++;

                return true;
            }
            else
            {
                addToken(tokenizer, {KEYWORD_TOKEN_TYPE, {.keywordCode = keyword.code}, tokenizer->currentLine, tokenizer->position});
            }

            if (*(tokenizer->position) == '\n')
            {
                tokenizer->currentLine++;
            }

            proceed(tokenizer, keyword.length);

            return true;
        }
    }

    return false;
}

bool isKeywordNumber(Keyword keyword)
{
    return keyword.code >= ZERO_KEYWORD && keyword.code <= SIX_KEYWORD;  
}

int64_t keywordToNumber(Keyword keyword)
{
    switch (keyword.code)
    {
        case ZERO_KEYWORD:  { return 0;               }
        case TWO_KEYWORD:   { return 2;               }
        case THREE_KEYWORD: { return 3;               }
        case SIX_KEYWORD:   { return 6;               }
        default:            { return INVALID_KEYWORD; }
    }
}

bool processNumeric(Tokenizer* tokenizer)
{
    ASSERT_TOKENIZER(tokenizer);

    if (!(tokenizer->useNumericNumbers)) { return false; }

    char*   numberEnd = nullptr;
    int64_t value     = strtoll(tokenizer->position, &numberEnd, 10);

    if (numberEnd == tokenizer->position) { return false; }

    addToken(tokenizer, {NUMBER_TOKEN_TYPE, {.number = value}, tokenizer->currentLine, tokenizer->position});
    proceed(tokenizer, numberEnd - tokenizer->position);

    return true;
}

bool processId(Tokenizer* tokenizer)
{
    ASSERT_TOKENIZER(tokenizer);

    // FIXME: add ID_ALLOWED_SYMBOLS into syntax
    size_t length = strspn(tokenizer->position, LETTERS); 

    if (tokenizer->position + length > tokenizer->buffer + tokenizer->bufferSize || length == 0)
    {
        return false;
    }

    addToken(tokenizer, {ID_TOKEN_TYPE, {.id = copyString(tokenizer->position, length)}, tokenizer->currentLine, tokenizer->position});
    proceed(tokenizer, length);

    return true;
}

void dumpTokens(const Token* tokens, size_t count, FILE* file)
{
    assert(tokens);
    assert(file);

    for (size_t i = 0; i < count; i++)
    {
        fprintf(file, "Token %zu:\n"
                      "\ttype = %s[%d]\n"
                      "\tdata = ", 
                      i,
                      tokenTypeToString(tokens[i].type),
                      tokens[i].type);

        switch (tokens[i].type)
        {
            case QUOTED_STRING_TOKEN_TYPE:
            {
                fprintf(file, "(quotedString) \"%s\"\n", tokens[i].data.id);
                break;
            }

            case KEYWORD_TOKEN_TYPE: 
            { 
                if (tokens[i].data.keywordCode == NEW_LINE_KEYWORD)
                {
                    fprintf(file, "(keywordCode) %s[%d] \\n\n", 
                                  keywordCodeToString(tokens[i].data.keywordCode),
                                  tokens[i].data.keywordCode); 
                }
                else 
                {
                    fprintf(file, "(keywordCode) %s[%d] %s\n", 
                                  keywordCodeToString(tokens[i].data.keywordCode),
                                  tokens[i].data.keywordCode, 
                                  KEYWORDS[tokens[i].data.keywordCode].string); 
                }

                break; 
            }

            case NUMBER_TOKEN_TYPE: 
            { 
                fprintf(file, "(number) %" PRId64 "\n", tokens[i].data.number);
                break; 
            }

            case ID_TOKEN_TYPE: 
            { 
                fprintf(file, "(id) %s\n", tokens[i].data.id);
                break; 
            }
        }

        fprintf(file, "\tline = %zu\n", tokens[i].line);

        if (tokens[i].pos[0] == '\n')
        {
            fprintf(file, "\tpos  = '\\n'\n\n");
        }
        else
        {
            size_t length = strcspn(tokens[i].pos, "\n");
            fprintf(file, "\tpos  = '%.*s'\n\n", (int) length, tokens[i].pos);
        }
    }
}

const char* tokenTypeToString(TokenType type)
{
    switch (type)
    {
        case QUOTED_STRING_TOKEN_TYPE: { return TO_STR(QUOTED_STRING_TOKEN_TYPE); }
        case KEYWORD_TOKEN_TYPE:       { return TO_STR(KEYWORD_TOKEN_TYPE);       }
        case NUMBER_TOKEN_TYPE:        { return TO_STR(NUMBER_TOKEN_TYPE);        }
        case ID_TOKEN_TYPE:            { return TO_STR(ID_TOKEN_TYPE);            }
    }

    return nullptr;
}