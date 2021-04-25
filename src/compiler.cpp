#include <assert.h>
#include <string.h>

#include "nasm_compilation.h"

#define CUR_FUNC compiler->curFunction

const size_t HORIZONTAL_LINE_LENGTH     = 50;
const char*  INDENTATION                = "        ";
const size_t MAX_INDENTED_STRING_LENGTH = 512;

void compileError        (Compiler* compiler, CompilerError error); 

void writeHorizontalLine (Compiler* compiler);
void writeFileHeader     (Compiler* compiler);
void writeFunctionHeader (Compiler* compiler);

void writeFunction       (Compiler* compiler, Node* node);
void writeBlock          (Compiler* compiler, Node* node);
void writeStatement      (Compiler* compiler, Node* node);

void writeCondition      (Compiler* compiler, Node* node);
void writeLoop           (Compiler* compiler, Node* node);
void writeAssignment     (Compiler* compiler, Node* node);
void writeReturn         (Compiler* compiler, Node* node);

void writeExpression     (Compiler* compiler, Node* node);
void writeMath           (Compiler* compiler, Node* node);
void writeCompare        (Compiler* compiler, Node* node);
void writeNumber         (Compiler* compiler, Node* node);
void writeVar            (Compiler* compiler, Node* node);

void writeCall           (Compiler* compiler, Node* node);
bool writeStdCall        (Compiler* compiler, Node* node);

void construct(Compiler* compiler, Node* tree, SymbolTable* table)
{
    assert(compiler);
    assert(tree);
    assert(table);

    compiler->table = table; 
    compiler->tree  = tree;
}

void destroy(Compiler* compiler)
{
    assert(compiler);

    compiler->table = nullptr;
    compiler->tree  = nullptr;
}

const char* errorString(CompilerError error)
{
    if (error < COMPILER_ERRORS_COUNT)
    {
        return COMPILER_ERROR_STRINGS[error];
    }

    return "UNDEFINED error";
}

void compileError(Compiler* compiler, CompilerError error)
{
    ASSERT_COMPILER(compiler);

    compiler->status = error;

    printf("COMPILATION ERROR: %s\n", errorString(error));
}

// FIXME: rdi, rsi, rdx, rcx, r8, r9

CompilerError compile(Compiler* compiler, const char* outputFile)
{
    assert(compiler);
    assert(outputFile);

    compiler->file = fopen(outputFile, "w");
    if (compiler->file == nullptr)
    {
        compileError(compiler, COMPILER_ERROR_FILE_OPEN_FAILURE);
        return compiler->status;
    }

    if (getFunction(compiler->table, MAIN_FUNCTION_NAME) == nullptr)
    {
        compileError(compiler, COMPILER_ERROR_NO_MAIN_FUNCTION);
        return compiler->status;
    }

    writeFileHeader(compiler);

    CUR_FUNC = compiler->table->functions;

    Node* curDeclaration = compiler->tree;
    while (curDeclaration != nullptr)
    {
        writeFunction(compiler, curDeclaration->right);
        curDeclaration = curDeclaration->left;
        CUR_FUNC++;

        write(compiler, "\n\n");
    }

    fclose(compiler->file);

    return compiler->status;
}

void write(Compiler* compiler, const char* format, ...)
{
    ASSERT_COMPILER(compiler);
    assert(format);

    va_list args;
    va_start(args, format);

    vfprintf(compiler->file, format, args);
}

void writeNewLine(Compiler* compiler)
{
    ASSERT_COMPILER(compiler);

    write(compiler, "\n");
}

//------------------------------------------------------------------------------
//! Writes the formatted string with indentation. String INDENTATION is written
//! before the string and after each '\n' (unless it's the last character in
//! the string).
//! 
//! @param compiler
//! @param format
//! @param ...
//------------------------------------------------------------------------------
void writeIndented(Compiler* compiler, const char* format, ...)
{
    ASSERT_COMPILER(compiler);
    assert(format);

    static char string[MAX_INDENTED_STRING_LENGTH];

    va_list args;
    va_start(args, format);

    fprintf(compiler->file, INDENTATION);
    vfprintf(compiler->file, format, args);
}

void writeHorizontalLine(Compiler* compiler)
{
    ASSERT_COMPILER(compiler);

    write(compiler, "; ");

    for (size_t i = 0; i < HORIZONTAL_LINE_LENGTH; i++)
    {
        write(compiler, "=");
    }

    write(compiler, "\n");
}

void writeFileHeader(Compiler* compiler)
{
    ASSERT_COMPILER(compiler);

    write(compiler, "global _start   \n" 
                    "section .text   \n\n"
                    "_start:         \n"
                    "\tcall love     \n"
                    "\tmov rax, 0x3C \n"
                    "\txor rdi, rdi  \n"
                    "\tsyscall       \n\n");
}

// TODO:
// 1) for each variable write it's offset in the form [rbp - x]
// 2)
void writeFunctionHeader(Compiler* compiler)
{
    ASSERT_COMPILER(compiler);

    writeHorizontalLine(compiler);
    write(compiler, "; %s\n;\n; params: ", CUR_FUNC->name);

    for (size_t i = 0; i < CUR_FUNC->paramsCount; i++)
    {
        write(compiler, "%s", CUR_FUNC->vars[i]);

        if (i < CUR_FUNC->paramsCount - 1)
        {
            write(compiler, ", ");
        }
    }

    write(compiler, "\n; vars:   ");

    for (size_t i = CUR_FUNC->paramsCount; i < CUR_FUNC->varsCount; i++)
    {
        write(compiler, "%s", CUR_FUNC->vars[i]);

        if (i < CUR_FUNC->varsCount - 1)
        {
            write(compiler, ", ");
        }
    }

    write(compiler, "\n");
    writeHorizontalLine(compiler);

    write(compiler, "%s:\n", CUR_FUNC->name);
}

void writeFunction(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node);

    writeFunctionHeader(compiler);

    write_push_r64(compiler, RBP);          // push rbp
    write_mov_r64_r64(compiler, RBP, RSP) ; // mov  rbp, rsp
    
    // FIXME: local vars change
    if (CUR_FUNC->varsCount != CUR_FUNC->paramsCount)
    {
        write_sub_r64_imm32(compiler, RSP, 8 * (CUR_FUNC->varsCount - CUR_FUNC->paramsCount)); 
    }

    // for (size_t i = 0; i < CUR_FUNC->paramsCount; i++)
    // {
    //     writeIndented(compiler, "pop [rax+%zu]\n", 2 + i);
    // }

    write(compiler, "\n");
    writeBlock(compiler, node->left);

    write(compiler, ".RETURN:\n");
    write_mov_r64_r64(compiler, RSP, RBP); // mov rsp, rbp
    write_pop_r64(compiler, RBP);          // pop rbp
    write_ret(compiler);                   // ret
}

void writeBlock(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node);

    Node* curStatement = node->right;
    while (curStatement != nullptr)
    {
        writeStatement(compiler, curStatement);
        curStatement = curStatement->right;
    }
}

void writeStatement(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node);
    assert(node->left);

    switch (node->left->type)
    {
        case COND_TYPE:   { writeCondition  (compiler, node->left); break; }
        case LOOP_TYPE:   { writeLoop       (compiler, node->left); break; }
        case VDECL_TYPE:  { writeAssignment (compiler, node->left); break; }
        case ASSIGN_TYPE: { writeAssignment (compiler, node->left); break; }
        case JUMP_TYPE:   { writeReturn     (compiler, node->left); break; }
        default:          { writeExpression (compiler, node->left); break; }
    }
}

void writeCondition(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node);

    writeIndented(compiler, "; ==== if-else statement ====\n");

    writeIndented(compiler, "; condition's expression\n");
    writeExpression(compiler, node->left);             // (rax = condition)

    size_t label = compiler->curCondLabel++;
    write_test_r64_r64(compiler, RAX, RAX);            // test rax, rax 
    write_jz_rel32(compiler, ".ELSE_", label);         // jz .ELSE_x

    writeNewLine(compiler);

    writeIndented(compiler, "; if true\n");
    writeBlock(compiler, node->right->left);    
    write_jmp_rel32(compiler, ".END_IF_ELSE_", label); // jmp .END_IF_ELSE_x
    
    writeNewLine(compiler);
    write(compiler, ".ELSE_%zu:\n\n",label); 

    if (node->right->right != nullptr)
    {
        writeBlock(compiler, node->right->right);
    }

    write(compiler, ".END_IF_ELSE_%zu:\n\n", label);
}

void writeLoop(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node);

    size_t label = compiler->curLoopLabel++;

    writeIndented(compiler, "; ==== while ====\n");
    write(compiler, ".WHILE_%zu:\n");

    writeIndented(compiler, "; exit condition\n");
    writeExpression(compiler, node->left);           // (rax = condition)
    
    write_test_r64_r64(compiler, RAX, RAX);          // test rax, rax 
    write_jz_rel32(compiler, ".END_WHILE_", label);  // jz .END_WHILE_x
    writeNewLine(compiler);

    writeIndented(compiler, "; loop body\n");
    writeBlock(compiler, node->right);

    write_jmp_rel32(compiler, ".END_WHILE_", label); // jmp .END_WHILE_x

    write(compiler, ".END_WHILE_%zu:\n\n", label);
}

void writeAssignment(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node);

    const char* var       = node->left->data.id;
    Mem64       varMemory = mem64BaseDisp(RBP, getVarOffset(CUR_FUNC, var)); 

    writeIndented(compiler, "; --- assignment to %s ---\n", var);

    writeIndented(compiler, "; evaluating expression\n");
    writeExpression(compiler, node->right);      // (rax = expression)

    write_mov_m64_r64(compiler, varMemory, RAX); // mov [rbp + offset], rax
    writeIndented(compiler, "; --- assignment to %s ---\n\n", var);
}

void writeReturn(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node);

    writeExpression(compiler, node->right);   // (rax = expression)
    write_jmp_rel32(compiler, ".RETURN", -1); // jmp .RETURN
}

void writeExpression(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node);

    switch (node->type)
    {
        case MATH_TYPE:   { writeMath   (compiler, node); break; }
        case NUMBER_TYPE: { writeNumber (compiler, node); break; }
        case ID_TYPE:     { writeVar    (compiler, node); break; }
        case CALL_TYPE:   { writeCall   (compiler, node); break; }

        default:          { assert(!"Valid node type");   break; }
    }
}

void writeMath(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node);

    MathOp operation = node->data.operation;

    writeExpression(compiler, node->left);       // (rax = expression1)
    
    writeNewLine(compiler);
    write_push_r64(compiler, RAX, "save rax");   // push rax
    writeNewLine(compiler);

    writeExpression(compiler, node->right);      // (rax = expression2)

    write_mov_r64_r64(compiler, RBX, RAX);       // rbx = rax = expression2
    write_pop_r64(compiler, RAX, "restore rax"); // pop rax 

    writeNewLine(compiler);

    // FIXME: add isBinaryOperation function
    if (operation > DIV_OP)
    {
        writeCompare(compiler, node);
        return;
    }

    switch (node->data.operation)
    {
        case ADD_OP: { write_add_r64_r64  (compiler, RAX, RBX); break; } // add  rax, rbx
        case SUB_OP: { write_sub_r64_r64  (compiler, RAX, RBX); break; } // sub  rax, rbx
        case MUL_OP: { write_imul_r64_r64 (compiler, RAX, RBX); break; } // imul rax, rbx
        
        case DIV_OP: 
        { 
            write_xor_r64_r64(compiler, RDX, RDX, "; rdx = 0 (for the division below)");
            write_idiv_r64(compiler, RBX);
            break; 
        }

        default: { assert(!"Valid math op"); break; }
    }
}

void writeCompare(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node);

    size_t label = compiler->curCmpLabel++;

    write_cmp_r64_r64(compiler, RAX, RBX);

    switch (node->data.operation)
    {
        case EQUAL_OP:         { write_je_rel32  (compiler, ".COMPARISON_TRUE_", label); break; }
        case NOT_EQUAL_OP:     { write_jne_rel32 (compiler, ".COMPARISON_TRUE_", label); break; }
        case LESS_OP:          { write_jl_rel32  (compiler, ".COMPARISON_TRUE_", label); break; }
        case GREATER_OP:       { write_jg_rel32  (compiler, ".COMPARISON_TRUE_", label); break; }
        case LESS_EQUAL_OP:    { write_jle_rel32 (compiler, ".COMPARISON_TRUE_", label); break; }
        case GREATER_EQUAL_OP: { write_jge_rel32 (compiler, ".COMPARISON_TRUE_", label); break; }

        default:               { assert(!"Invalid cmp op"); break; }
    }

    write_mov_r64_imm64(compiler, RAX, 0, "false");
    write_jmp_rel32(compiler, ".COMPARISON_END_", label);

    write(compiler, ".COMPARISON_TRUE_%zu", label);

    write_mov_r64_imm64(compiler, RAX, 1, "true");

    write(compiler, ".COMPARISON_END_%zu", label);
}

void writeNumber(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node);

    write_mov_r64_imm64(compiler, RAX, node->data.number);
}

void writeVar(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node);

    Mem64 varMemory = mem64BaseDisp(RBP, getVarOffset(CUR_FUNC, node->data.id)); 

    write_mov_r64_m64(compiler, RAX, varMemory);
}

void writeCall(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler); 
    assert(node);

    writeIndented(compiler, "; --- calling %s() ---\n", node->left->data.id);

    if (writeStdCall(compiler, node)) { return; }

    Function* function = getFunction(compiler->table, node->left->data.id);
    if (function == nullptr) 
    {
        compileError(compiler, COMPILER_ERROR_CALL_UNDEFINED_FUNCTION);
        return; 
    }

    Node*  curParamExpr = node->right;
    size_t paramNumber  = function->paramsCount;
    while (curParamExpr != nullptr)
    {
        writeIndented(compiler, "; param %zu\n", paramNumber);

        writeExpression(compiler, curParamExpr->left); // (rax = expression)
        write_push_r64(compiler, RAX);                 // push rax

        curParamExpr = curParamExpr->right;
        paramNumber--;
    }

    writeNewLine(compiler);

    write_call_rel32(compiler, function->name, -1);

    if (function->paramsCount > 0)
    {
        write_add_r64_imm32(compiler, RSP, function->paramsCount * 8);
    }

    writeNewLine(compiler);
}

bool writeStdCall(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node);

    // const char* name = node->left->data.id;
    // if (strcmp(name, KEYWORDS[PRINT_KEYWORD].string) == 0)
    // {
    //     writeExpression(compiler, node->right->left);
    //     fprintf(OUTPUT, "out\n");
    // } 
    // else if (strcmp(name, KEYWORDS[SCAN_KEYWORD].string) == 0)
    // {
    //     fprintf(OUTPUT, "in\n");
    // } 
    // else if (strcmp(name, KEYWORDS[FLOOR_KEYWORD].string) == 0)
    // {
    //     writeExpression(compiler, node->right->left);
    //     fprintf(OUTPUT, "flr\n");
    // }
    // else if (strcmp(name, KEYWORDS[SQRT_KEYWORD].string) == 0)
    // {
    //     writeExpression(compiler, node->right->left);
    //     fprintf(OUTPUT, "sqrt\n");
    // }  
    // else if (strcmp(name, KEYWORDS[RAND_JUMP_KEYWORD].string) == 0)
    // {
    //     fprintf(OUTPUT, "rndjmp\n");
    // }
    // else 
    // {
    //     return false;
    // }

    return false; // FIXME: switch back to true
}