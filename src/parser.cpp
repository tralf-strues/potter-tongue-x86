#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "parser.h"
#include "../libs/utilib.h"

#define ASSERT_PARSER(parser) assert(parser);                      \
                              assert(parser->tokenizer);           \
                              assert(parser->tokenizer);           \
                              assert(parser->tokenizer->buffer);   \
                              assert(parser->tokenizer->position); \
                              assert(parser->tokenizer->tokens); 

#define CHECK_END_REACHED(returnValue) if (tokensLeft(parser) <= 0) { return returnValue; } 

#define SYNTAX_ERROR(error) syntaxError(parser, error); return nullptr;

#define REQUIRE_ID(id)                  if (!requireIdToken(parser, id))                  { return nullptr; }
#define REQUIRE_KEYWORD(keyword, error) if (!requireKeywordToken(parser, keyword, error)) { return nullptr; }
#define REQUIRE_NEW_LINES()             if (!requireNewLines(parser))                     { return nullptr; }
#define REQUIRE_VAR_DECLARED(var)       if (getVarOffset(parser->curFunction, var) == -1)        \
                                        {                                                        \
                                            proceed(parser, -1);                                 \
                                            SYNTAX_ERROR(PARSE_ERROR_VARIABLE_UNDECLARED_USAGE); \
                                        }

Token* curToken            (Parser* parser);
void   proceed             (Parser* parser, int step);
void   proceed             (Parser* parser);
int    tokensLeft          (Parser* parser);
bool   isEndReached        (Parser* parser);

bool   requireIdToken      (Parser* parser, const char* id);
bool   requireKeywordToken (Parser* parser, KeywordCode keywordCode, ParseError error);
bool   requireNewLines     (Parser* parser);

void   syntaxError         (Parser* parser, ParseError error);

Node*  parseProgramBody    (Parser* parser);
// Node*  parseSDeclaration   (Parser* parser);
Node*  parseFDeclaration   (Parser* parser);

Node*  parseBlock          (Parser* parser);
Node*  parseStatement      (Parser* parser);
Node*  parseCmdLine        (Parser* parser);
Node*  parseJump           (Parser* parser);

Node*  parseExpression     (Parser* parser);
Node*  parseComparand      (Parser* parser);
Node*  parseTerm           (Parser* parser);
Node*  parseFactor         (Parser* parser);

Node*  parseCondition      (Parser* parser);
Node*  parseLoop           (Parser* parser);

Node*  parseVDeclaration   (Parser* parser);

Node*  parseAssignment     (Parser* parser);

Node*  parseCall           (Parser* parser);
Node*  parsePrint          (Parser* parser);
Node*  parseStandardFunc   (Parser* parser, KeywordCode keywordCode);

Node*  parseExprList       (Parser* parser);
Node*  parseParamList      (Parser* parser);

Node*  parseQuotedString   (Parser* parser);
Node*  parseId             (Parser* parser);
Node*  parseNumber         (Parser* parser);

void construct(Parser* parser, Tokenizer* tokenizer)
{
    assert(parser);
    assert(tokenizer);
    assert(tokenizer->tokens);

    parser->tokenizer = tokenizer;
    parser->offset    = 0;
    parser->status    = PARSE_NO_ERROR;
}

void destroy(Parser* parser)
{
    assert(parser);

    parser->tokenizer = nullptr;
    parser->offset    = 0;
    parser->status    = PARSE_NO_ERROR;
}

const char* errorString(ParseError error)
{
    if (error < TOTAL_PARSE_ERRORS)
    {
        return PARSE_ERROR_STRINGS[error];
    }

    return "UNDEFINED error";
}

Token* curToken(Parser* parser)
{
    ASSERT_PARSER(parser);
    return parser->tokenizer->tokens + parser->offset;
}

void proceed(Parser* parser, int step)
{
    ASSERT_PARSER(parser);

    assert(step >= 0 || (step < 0 && ((int) parser->offset) >= -step));

    parser->offset += step;

    if (parser->offset > parser->tokenizer->tokensCount)
    {
        parser->offset = parser->tokenizer->tokensCount;
    }
}

void proceed(Parser* parser)
{
    ASSERT_PARSER(parser);

    proceed(parser, 1);
}

int tokensLeft(Parser* parser)
{
    assert(parser);
    assert(parser->tokenizer);

    return parser->tokenizer->tokensCount - parser->offset;
}

bool isEndReached(Parser* parser)
{
    assert(parser);

    return tokensLeft(parser) <= 1;
}

bool requireIdToken(Parser* parser, const char* id)
{
    ASSERT_PARSER(parser);
    CHECK_END_REACHED(false);

    if (!isIdType(curToken(parser)))
    {
        syntaxError(parser, PARSE_ERROR_ID_NEEDED);
        return false;
    }

    if (id != nullptr && strcmp(id, curToken(parser)->data.id) != 0)
    {
        syntaxError(parser, PARSE_ERROR_INVALID_ID);
        return false;
    }

    proceed(parser);

    return true;
}

bool requireKeywordToken(Parser* parser, KeywordCode keywordCode, ParseError error)
{
    ASSERT_PARSER(parser);
    CHECK_END_REACHED(false);

    Token* token = curToken(parser);

    if (!isKeyword(token, keywordCode))
    {
        syntaxError(parser, error);
        return false;
    }

    proceed(parser);

    return true;
}

bool requireNewLines(Parser* parser)
{
    ASSERT_PARSER(parser);
    CHECK_END_REACHED(false);

    Token* token = curToken(parser);

    if (!isKeyword(token, NEW_LINE_KEYWORD))
    {
        syntaxError(parser, PARSE_ERROR_NEW_LINE_NEEDED);
        return false;
    }

    do
    {
        proceed(parser);
    }
    while (isKeyword(curToken(parser), NEW_LINE_KEYWORD));

    return true;
}

void syntaxError(Parser* parser, ParseError error)
{
    if (error == PARSE_NO_ERROR) { return; }

    parser->status = error;

    printf("SYNTAX ERROR: %s\n", errorString(error));

    Token       token      = *curToken(parser);
    const char* lineStart  = token.pos;
    const char* lineEnd    = token.pos;

    const char* buffer     = parser->tokenizer->buffer;
    size_t      bufferSize = parser->tokenizer->bufferSize;

    while (lineStart > buffer && *(lineStart - 1) != '\n' && *lineStart != '\n')
    {
        lineStart--;
    }

    while (lineEnd < buffer + bufferSize && *(lineEnd + 1) != '\n' && *lineEnd != '\n')
    {
        lineEnd++;
    }

    int lineOffset = digitsCount(token.line + 1) + 1;
    printf("%zu|%.*s\n", token.line + 1, (int) (lineEnd - lineStart + 1), lineStart);

    for (size_t i = 0; i + lineStart < token.pos + lineOffset; i++)
    {
        putchar(' ');
    }

    printf("^\n");
}

ParseError parseProgram(Parser* parser, SymbolTable* table, Node** root)
{
    ASSERT_PARSER(parser);
    assert(table);
    assert(root);

    parser->table = table;

    requireKeywordToken(parser, PROG_START_KEYWORD, PARSE_ERROR_NO_PROG_START);
    requireIdToken(parser, nullptr);
    requireNewLines(parser);

    *root = parseProgramBody(parser);

    requireKeywordToken(parser, PROG_END_KEYWORD, PARSE_ERROR_NO_PROG_END);

    return parser->status;
}

Node* parseProgramBody(Parser* parser)
{
    ASSERT_PARSER(parser);
    CHECK_END_REACHED(nullptr);

    Node* root = parseFDeclaration(parser);

    if (root == nullptr) { SYNTAX_ERROR(PARSE_ERROR_FUNCTION_DECLARATION_NEEDED); }

    Node* prevFDeclaration = root;
    Node* nextFDeclaration = nullptr;

    while ((nextFDeclaration = parseFDeclaration(parser)) != nullptr)
    {
        setLeft(prevFDeclaration, nextFDeclaration);
        
        prevFDeclaration = nextFDeclaration;
    }

    return root;
}

// Node* parseSDeclaration(Parser* parser)
// {
//     ASSERT_PARSER(parser);
//     CHECK_END_REACHED(nullptr);

//     if (!isKeyword(curToken(parser), STR_QUOTE_KEYWORD)) { return nullptr; }
//     proceed(parser);

//     Node* stringQuoted = parse
// }

Node* parseFDeclaration(Parser* parser)
{
    ASSERT_PARSER(parser);
    CHECK_END_REACHED(nullptr);

    if (!isKeyword(curToken(parser), FDECL_KEYWORD)) { return nullptr; }
    proceed(parser);

    Node* functionDeclaration = newNode(FDECL_TYPE, { .isVoidFunction = false }, nullptr, nullptr);
    
    if (isNumber(curToken(parser), 0))
    {
        functionDeclaration->data.isVoidFunction = true;
        proceed(parser); 
    }

    setRight(functionDeclaration, parseId(parser));
    if (functionDeclaration->right == nullptr) { SYNTAX_ERROR(PARSE_ERROR_ID_NEEDED); }

    if (getFunction(parser->table, functionDeclaration->right->data.id) != nullptr)
    {
        SYNTAX_ERROR(PARSE_ERROR_FUNCTION_SECOND_DECLARATION);
    }

    parser->curFunction = pushFunction(parser->table, functionDeclaration->right->data.id);

    Node* params = parseParamList(parser);

    if (params == nullptr && !isNumber(curToken(parser), 0))
    {
        SYNTAX_ERROR(PARSE_ERROR_FUNCTION_PARAMS_NEEDED);
    }

    if (isNumber(curToken(parser), 0))
    {
        proceed(parser);
    }

    setRight(functionDeclaration->right, params);
    setLeft(functionDeclaration->right, parseBlock(parser));

    if (functionDeclaration->right->left == nullptr)
    {
        SYNTAX_ERROR(PARSE_ERROR_FUNCTION_BODY_NEEDED);
    }

    return functionDeclaration;
}

Node* parseBlock(Parser* parser)
{
    ASSERT_PARSER(parser);
    
    REQUIRE_NEW_LINES();
    REQUIRE_KEYWORD(OPEN_BRACE_KEYWORD, PARSE_ERROR_OPEN_BRACE_NEEDED);
    REQUIRE_NEW_LINES();

    Node* block = newNode(BLOCK_TYPE, {}, nullptr, parseStatement(parser));
    Node* statement = block->right;

    while (statement != nullptr)
    {
        setRight(statement, parseStatement(parser));
        statement = statement->right;
    }

    REQUIRE_KEYWORD(CLOSE_BRACE_KEYWORD, PARSE_ERROR_CLOSE_BRACE_NEEDED);
    REQUIRE_NEW_LINES();

    return block;
}

Node* parseStatement(Parser* parser)
{
    ASSERT_PARSER(parser);

    Node* node = parseCmdLine(parser);

    if (node == nullptr) { node = parseCondition(parser); }
    if (node == nullptr) { node = parseLoop(parser);      }
    if (node == nullptr) { return nullptr;                }

    Node* statement = newNode(STATEMENT_TYPE, {}, nullptr, nullptr);
    setLeft(statement, node);

    return statement;
}
    
Node* parseCmdLine(Parser* parser)
{
    ASSERT_PARSER(parser);

    if (!isKeyword(curToken(parser), CMD_LINE_KEYWORD))
    {
        return nullptr;
    }

    proceed(parser);

    Node* node = parseExpression(parser);

    if (node == nullptr) { node = parseVDeclaration(parser); }
    if (node == nullptr) { node = parseAssignment(parser);   }
    if (node == nullptr) { node = parseJump(parser);         }
    if (node == nullptr) { node = parsePrint(parser);        }
    if (node == nullptr) { return nullptr;                   }

    REQUIRE_NEW_LINES();

    return node;
}

Node* parseJump(Parser* parser)
{
    ASSERT_PARSER(parser);
    
    if (!isKeyword(curToken(parser), RETURN_KEYWORD)) { return nullptr; }

    proceed(parser);

    Node* expression = parseExpression(parser);
    if (expression == nullptr) { SYNTAX_ERROR(PARSE_ERROR_RETURN_EXPRESSION_NEEDED); } 

    return newNode(JUMP_TYPE, {}, nullptr, expression);
}

Node* parseExpression(Parser* parser)
{
    ASSERT_PARSER(parser);
    
    Node* comparand1 = parseComparand(parser);

    if (comparand1 == nullptr) { return nullptr; }

    while (isComparand(curToken(parser)))
    {
        KeywordCode operation = curToken(parser)->data.keywordCode;

        proceed(parser);

        Node* comparand2 = parseComparand(parser);
        if (comparand2 == nullptr) { SYNTAX_ERROR(PARSE_ERROR_COMPARAND_NOT_FOUND); }

        switch (operation)
        {
            case EQUAL_KEYWORD:         { comparand1 = BINARY_OP(EQUAL,         comparand1, comparand2); break; }
            case NOT_EQUAL_KEYWORD:     { comparand1 = BINARY_OP(NOT_EQUAL,     comparand1, comparand2); break; }
            case LESS_KEYWORD:          { comparand1 = BINARY_OP(LESS,          comparand1, comparand2); break; }
            case GREATER_KEYWORD:       { comparand1 = BINARY_OP(GREATER,       comparand1, comparand2); break; }
            case LESS_EQUAL_KEYWORD:    { comparand1 = BINARY_OP(LESS_EQUAL,    comparand1, comparand2); break; }
            case GREATER_EQUAL_KEYWORD: { comparand1 = BINARY_OP(GREATER_EQUAL, comparand1, comparand2); break; }

            default: SYNTAX_ERROR(PARSE_ERROR_INVALID_COMPARISON_OPERATION);
        }
    }

    return comparand1;
}

Node* parseComparand(Parser* parser)
{
    ASSERT_PARSER(parser);
    
    Node* term1 = parseTerm(parser);
    if (term1 == nullptr) { return nullptr; }

    while (isKeyword(curToken(parser), PLUS_KEYWORD) || isKeyword(curToken(parser), MINUS_KEYWORD))
    {
        KeywordCode operation = curToken(parser)->data.keywordCode;

        proceed(parser);

        Node* term2 = parseTerm(parser);
        if (term2 == nullptr) { SYNTAX_ERROR(PARSE_ERROR_TERM_NOT_FOUND); }

        switch (operation)
        {
            case PLUS_KEYWORD:  { term1 = BINARY_OP(ADD, term1, term2); break; }
            case MINUS_KEYWORD: { term1 = BINARY_OP(SUB, term1, term2); break; }

            default: SYNTAX_ERROR(PARSE_ERROR_INVALID_TERM_OPERATION);
        }
    }

    return term1;
}

Node* parseTerm(Parser* parser)
{
    ASSERT_PARSER(parser);
    
    Node* factor1 = parseFactor(parser);
    if (factor1 == nullptr) { return nullptr; }

    while (isKeyword(curToken(parser), MUL_KEYWORD) || isKeyword(curToken(parser), DIV_KEYWORD))
    {
        KeywordCode operation = curToken(parser)->data.keywordCode;

        proceed(parser);

        Node* factor2 = parseFactor(parser);
        if (factor2 == nullptr) { SYNTAX_ERROR(PARSE_ERROR_FACTOR_NOT_FOUND); }

        switch (operation)
        {
            case MUL_KEYWORD: { factor1 = BINARY_OP(MUL, factor1, factor2); break; }
            case DIV_KEYWORD: { factor1 = BINARY_OP(DIV, factor1, factor2); break; }

            default: SYNTAX_ERROR(PARSE_ERROR_INVALID_FACTOR_OPERATION);
        }
    }

    return factor1;
}

Node* parseFactor(Parser* parser)
{
    ASSERT_PARSER(parser);
    
    Node* factor = nullptr;

    if (isNumberType(curToken(parser)))
    {
        factor = parseNumber(parser);
    }
    else
    {
        switch (curToken(parser)->data.keywordCode)
        {
            case BRACKET_KEYWORD:
            {
                proceed(parser);
                factor = parseExpression(parser);
                if (factor == nullptr) { SYNTAX_ERROR(PARSE_ERROR_NO_EXPRESSION_INSIDE_BRACKETS); }
        
                REQUIRE_KEYWORD(BRACKET_KEYWORD, PARSE_ERROR_BRACKET_NEEDED);
                break;
            }

            case DEREF_KEYWORD:
            {
                proceed(parser);
                factor = parseId(parser);
                if (factor == nullptr) { SYNTAX_ERROR(PARSE_ERROR_DEREFERENCING_NO_VARIABLE); }
        
                REQUIRE_VAR_DECLARED(factor->data.id);

                break;
            }

            case CALL_KEYWORD:  { factor = parseCall         (parser);                break; }
            case FLOOR_KEYWORD: { factor = parseStandardFunc (parser, FLOOR_KEYWORD); break; }
            case SQRT_KEYWORD:  { factor = parseStandardFunc (parser, SQRT_KEYWORD);  break; }

            case SCAN_KEYWORD:
            {
                factor = newNode(CALL_TYPE, {}, ID(KEYWORDS[SCAN_KEYWORD].string), nullptr);
                proceed(parser);
                break;
            }

            case RAND_JUMP_KEYWORD:
            {
                factor = newNode(CALL_TYPE, {}, ID(KEYWORDS[RAND_JUMP_KEYWORD].string), nullptr);
                proceed(parser);
                break;
            }

            default: { break; }
        }
    }

    return factor;
}

Node* parseCondition(Parser* parser)
{
    ASSERT_PARSER(parser);
    
    if (!isKeyword(curToken(parser), IF_KEYWORD)) { return nullptr; }

    proceed(parser);

    REQUIRE_KEYWORD(BRACKET_KEYWORD, PARSE_ERROR_BRACKET_NEEDED);

    Node* expression = parseExpression(parser);
    if (expression == nullptr) { SYNTAX_ERROR(PARSE_ERROR_IF_EXPRESSION_NEEDED); }

    REQUIRE_KEYWORD(BRACKET_KEYWORD, PARSE_ERROR_BRACKET_NEEDED);

    Node* trueBlock = parseBlock(parser);
    if (trueBlock == nullptr) { SYNTAX_ERROR(PARSE_ERROR_IF_BLOCK_NEEDED); }
    
    Node* conditionRoot = newNode(COND_TYPE, {}, expression, newNode(IFELSE_TYPE, {}, trueBlock, nullptr));

    if (isKeyword(curToken(parser), ELSE_KEYWORD))
    {
        proceed(parser);

        Node* falseBlock = parseBlock(parser);
        if (falseBlock == nullptr) { SYNTAX_ERROR(PARSE_ERROR_ELSE_BLOCK_NEEDED); }

        setRight(conditionRoot->right, falseBlock);
    }

    return conditionRoot;
}  

Node* parseLoop(Parser* parser)
{
    ASSERT_PARSER(parser);
    
    if (!isKeyword(curToken(parser), LOOP_KEYWORD)) { return nullptr; }

    proceed(parser);

    REQUIRE_KEYWORD(BRACKET_KEYWORD, PARSE_ERROR_BRACKET_NEEDED);

    Node* expression = parseExpression(parser);
    if (expression == nullptr) { SYNTAX_ERROR(PARSE_ERROR_LOOP_EXPRESSION_NEEDED); }

    REQUIRE_KEYWORD(BRACKET_KEYWORD, PARSE_ERROR_BRACKET_NEEDED);

    Node* block = parseBlock(parser);
    if (block == nullptr) { SYNTAX_ERROR(PARSE_ERROR_LOOP_BLOCK_NEEDED); }

    return newNode(LOOP_TYPE, {}, expression, block);
}

Node* parseVDeclaration(Parser* parser)
{
    ASSERT_PARSER(parser);
    
    if (!isKeyword(curToken(parser), VDECL_KEYWORD)) { return nullptr; }

    proceed(parser);

    if (!isIdType(curToken(parser))) { SYNTAX_ERROR(PARSE_ERROR_VARIABLE_DECLARATION_NO_ASSIGNMENT); }

    const char* id = curToken(parser)->data.id;
    if (getVarOffset(parser->curFunction, id) != -1) { SYNTAX_ERROR(PARSE_ERROR_VARIABLE_SECOND_DECLARATION); }

    pushVariable(parser->curFunction, id);

    Node* declaration = parseAssignment(parser);
    if (declaration == nullptr) { SYNTAX_ERROR(PARSE_ERROR_VARIABLE_DECLARATION_NO_ASSIGNMENT); }

    declaration->type = VDECL_TYPE;

    return declaration;
}

Node* parseAssignment(Parser* parser)
{
    ASSERT_PARSER(parser);
    
    if (!isIdType(curToken(parser)) || isEndReached(parser) || 
        (!isEndReached(parser) && !isKeyword(curToken(parser) + 1, ASSIGN_KEYWORD)))
    {
        return nullptr;
    }

    Node* variable = parseId(parser);
    REQUIRE_VAR_DECLARED(variable->data.id);
    REQUIRE_KEYWORD(ASSIGN_KEYWORD, PARSE_ERROR_VARIABLE_DECLARATION_NO_ASSIGNMENT);

    Node* expression = parseExpression(parser);
    if (expression == nullptr) { SYNTAX_ERROR(PARSE_ERROR_VARIABLE_ASSIGNMENT_NO_EXPRESSION); }

    return newNode(ASSIGN_TYPE, {}, variable, expression);
}

Node* parseCall(Parser* parser)
{
    ASSERT_PARSER(parser);
    
    if (!isKeyword(curToken(parser), CALL_KEYWORD)) { return nullptr; }
    proceed(parser);

    REQUIRE_ID(nullptr);
    proceed(parser, -1);

    Node* function = parseId(parser);

    REQUIRE_KEYWORD(BRACKET_KEYWORD, PARSE_ERROR_BRACKET_NEEDED);

    Node* exprList = nullptr;

    if (!isKeyword(curToken(parser), BRACKET_KEYWORD))
    {
        exprList = parseExprList(parser);

        if (exprList == nullptr) { SYNTAX_ERROR(PARSE_ERROR_FUNCTION_PARAMS_NEEDED); }
    }

    REQUIRE_KEYWORD(BRACKET_KEYWORD, PARSE_ERROR_BRACKET_NEEDED);

    return newNode(CALL_TYPE, {}, function, exprList);
}

Node* parsePrint(Parser* parser)
{
    ASSERT_PARSER(parser);
    
    if (!isKeyword(curToken(parser), PRINT_KEYWORD)) { return nullptr; }
    proceed(parser);

    Node* quotedString = parseQuotedString(parser);
    if (quotedString != nullptr)
    {
        return newNode(CALL_TYPE, {}, ID(KEYWORDS[PRINT_KEYWORD].string), quotedString);
    }

    Node* expression = parseExpression(parser);
    if (expression == nullptr) { SYNTAX_ERROR(PARSE_ERROR_PRINT_EXPRESSION_NEEDED); }

    Node* paramList = newNode(EXPR_LIST_TYPE, {}, expression, nullptr);

    return newNode(CALL_TYPE, {}, ID(KEYWORDS[PRINT_KEYWORD].string), paramList);
}

Node* parseStandardFunc(Parser* parser, KeywordCode keywordCode)
{
    ASSERT_PARSER(parser);
    
    if (!isKeyword(curToken(parser), keywordCode)) { return nullptr; }
    proceed(parser);

    REQUIRE_KEYWORD(BRACKET_KEYWORD, PARSE_ERROR_BRACKET_NEEDED);
    
    Node* expression = parseExpression(parser);
    if (expression == nullptr) { SYNTAX_ERROR(PARSE_ERROR_FLOOR_EXPRESSION_NEEDED); }

    REQUIRE_KEYWORD(BRACKET_KEYWORD, PARSE_ERROR_BRACKET_NEEDED);

    Node* paramList = newNode(EXPR_LIST_TYPE, {}, expression, nullptr);

    return newNode(CALL_TYPE, {}, ID(KEYWORDS[keywordCode].string), paramList);
}

Node* parseFloor(Parser* parser)
{
    ASSERT_PARSER(parser);
    
    if (!isKeyword(curToken(parser), FLOOR_KEYWORD)) { return nullptr; }
    proceed(parser);

    REQUIRE_KEYWORD(BRACKET_KEYWORD, PARSE_ERROR_BRACKET_NEEDED);
    
    Node* expression = parseExpression(parser);
    if (expression == nullptr) { SYNTAX_ERROR(PARSE_ERROR_FLOOR_EXPRESSION_NEEDED); }

    REQUIRE_KEYWORD(BRACKET_KEYWORD, PARSE_ERROR_BRACKET_NEEDED);

    Node* paramList = newNode(EXPR_LIST_TYPE, {}, expression, nullptr);

    return newNode(CALL_TYPE, {}, ID(KEYWORDS[FLOOR_KEYWORD].string), paramList);
}

Node* parseSqrt(Parser* parser)
{
    ASSERT_PARSER(parser);
    
    if (!isKeyword(curToken(parser), SQRT_KEYWORD)) { return nullptr; }
    proceed(parser);

    REQUIRE_KEYWORD(BRACKET_KEYWORD, PARSE_ERROR_BRACKET_NEEDED);
    
    Node* expression = parseExpression(parser);
    if (expression == nullptr) { SYNTAX_ERROR(PARSE_ERROR_FLOOR_EXPRESSION_NEEDED); }

    REQUIRE_KEYWORD(BRACKET_KEYWORD, PARSE_ERROR_BRACKET_NEEDED);

    Node* paramList = newNode(EXPR_LIST_TYPE, {}, expression, nullptr);

    return newNode(CALL_TYPE, {}, ID(KEYWORDS[SQRT_KEYWORD].string), paramList);
}

Node* parseExprList(Parser* parser)
{
    ASSERT_PARSER(parser);

    Node* expression = parseExpression(parser);
    if (expression == nullptr) { return nullptr; }

    Node* paramList = newNode(EXPR_LIST_TYPE, {}, expression, nullptr);

    while (isKeyword(curToken(parser), COMMA_KEYWORD))
    {
        proceed(parser);

        Node* nextExpression = parseExpression(parser);
        if (nextExpression == nullptr) { SYNTAX_ERROR(PARSE_ERROR_FUNCTION_PARAMS_NEEDED); }

        paramList = newNode(EXPR_LIST_TYPE, {}, nextExpression, paramList);
    }

    return paramList;
}

Node* parseParamList(Parser* parser)
{
    ASSERT_PARSER(parser);
        
    Node* param = parseId(parser);
    if (param == nullptr) { return nullptr; }

    Node* prevParam = param;
    pushParameter(parser->curFunction, prevParam->data.id);

    while (isKeyword(curToken(parser), COMMA_KEYWORD))
    {
        proceed(parser);

        setRight(prevParam, parseId(parser));
        if (prevParam->right == nullptr) { SYNTAX_ERROR(PARSE_ERROR_FUNCTION_PARAMS_NEEDED); }

        prevParam = prevParam->right;
        pushParameter(parser->curFunction, prevParam->data.id);
    }

    return param;
}

Node* parseQuotedString(Parser* parser)
{
    ASSERT_PARSER(parser);

    if (!isKeyword(curToken(parser), STR_QUOTE_KEYWORD)) { return nullptr; }
    proceed(parser);

    if (!isQuotedStringType(curToken(parser)))
    {
        SYNTAX_ERROR(PARSE_ERROR_NO_STRING_AFTER_QUOTE);
    }

    Node* string = newNode(STRING_TYPE, 
                           {.string = curToken(parser)->data.quotedString}, 
                           nullptr, 
                           nullptr);

    proceed(parser);
    if (!isKeyword(curToken(parser), STR_QUOTE_KEYWORD)) 
    { 
        SYNTAX_ERROR(PARSE_ERROR_NO_SECOND_QUOTE_AFTER_STRING);
    }
    
    proceed(parser);
    return string;
}

Node* parseId(Parser* parser)
{
    ASSERT_PARSER(parser);

    if (!isIdType(curToken(parser))) { return nullptr; }

    const char* id = curToken(parser)->data.id;
    proceed(parser);

    return newNode(ID_TYPE, { .id = id }, nullptr, nullptr);
}

Node* parseNumber(Parser* parser)
{
    ASSERT_PARSER(parser);

    if (!isNumberType(curToken(parser))) { return nullptr; }

    int64_t number = curToken(parser)->data.number;
    proceed(parser);

    return newNode(NUMBER_TYPE, { .number = number }, nullptr, nullptr);
}