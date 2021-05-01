#include <assert.h>
#include <string.h>
#include <inttypes.h>

#include "nasm_compilation.h"
#include "../../libs/file_manager.h"

#define CUR_FUNC compiler->curFunction

const size_t HORIZONTAL_LINE_LENGTH     = 50;
const char*  INDENTATION                = "                ";
const size_t MAX_INDENTED_STRING_LENGTH = 512;
const size_t IO_BUFFER_SIZE             = 256;

//===================================Compiler===================================
int32_t nextLabelNumber  (Compiler* compiler, LabelPurposeType labelType);
Label   getExistingLabel (Compiler* compiler, Label label);
void    writeLabel       (Compiler* compiler, Label label);
void    compileError     (Compiler* compiler, CompilerError error); 
//===================================Compiler===================================

//==================================Write data==================================
void writeFileHeader   (Compiler* compiler);
void writeStdFunctions (Compiler* compiler); 
void writeStdData      (Compiler* compiler);
void writeStringsData  (Compiler* compiler);
void writeStdBSS       (Compiler* compiler);
//==================================Write data==================================

//==============================Write NASM comments==============================
void writeHorizontalLine (Compiler* compiler);
void writeFunctionHeader (Compiler* compiler);
//==============================Write NASM comments==============================

//==================================Compilation==================================
void compileFunction         (Compiler* compiler, Node* node);
void compileBlock            (Compiler* compiler, Node* node);
void compileStatement        (Compiler* compiler, Node* node);

void compileCondition        (Compiler* compiler, Node* node);
void compileLoop             (Compiler* compiler, Node* node);
void compileAssignment       (Compiler* compiler, Node* node);
void compileAssignmentVar    (Compiler* compiler, Node* node);
void compileAssignmentArray  (Compiler* compiler, Node* node);
void compileArrayDeclaration (Compiler* compiler, Node* node);
void compileReturn           (Compiler* compiler, Node* node);

void compileExpression       (Compiler* compiler, Node* node);
void compileMath             (Compiler* compiler, Node* node);
void compileCompare          (Compiler* compiler, Node* node);
void compileMemAccess        (Compiler* compiler, Node* node);
void compileString           (Compiler* compiler, Node* node);
void compileNumber           (Compiler* compiler, Node* node);
void compileVar              (Compiler* compiler, Node* node);

void compileParamList        (Compiler* compiler, Node* node, const Function* function);
void compileCall             (Compiler* compiler, Node* node);
bool compileStdCall          (Compiler* compiler, Node* node);
//==================================Compilation=================================


//===================================Compiler===================================
void construct(Compiler* compiler, Node* tree, SymbolTable* table)
{
    assert(compiler);
    assert(tree);
    assert(table);

    compiler->table = table; 
    compiler->tree  = tree;
    construct(&compiler->labelManager);
}

void destroy(Compiler* compiler)
{
    assert(compiler);

    compiler->table = nullptr;
    compiler->tree  = nullptr;
    destroy(&compiler->labelManager);
}

const char* errorString(CompilerError error)
{
    if (error < COMPILER_ERRORS_COUNT)
    {
        return COMPILER_ERROR_STRINGS[error];
    }

    return "UNDEFINED error";
}

int32_t nextLabelNumber(Compiler* compiler, LabelPurposeType labelType)
{
    ASSERT_COMPILER(compiler);

    return compiler->labelManager.curLabelNumbers[labelType]++;
}

//------------------------------------------------------------------------------
//! If a label with the same (optionally) functionName name and (optionally) 
//! number already contains in the compiler's LabelManager, then return it, 
//! otherwise return the same label without any changes. This is needed to get 
//! the valid offset of the label (which is not always possible on the first 
//! pass of the compiler), based on its (optionally) functionName, name and 
//! (optionally) number. 
//! 
//! @param compiler
//! @param label
//! 
//! @return Label with a changed offset, if the label already exists, or the 
//!         same label. 
//------------------------------------------------------------------------------
Label getExistingLabel(Compiler* compiler, Label label)
{
    ASSERT_COMPILER(compiler);

    int idx = findLabel(&compiler->labelManager.labelArray, label);

    if (idx != -1) { return compiler->labelManager.labelArray.labels[idx]; }
    else           { return label; }
}

void writeLabel(Compiler* compiler, Label label)
{
    ASSERT_COMPILER(compiler);

    if (findLabel(&compiler->labelManager.labelArray, label) == -1)
    {
        insertLabel(&compiler->labelManager.labelArray, label);
    }

    if (label.number >= 0)
    {
        write(compiler, "%s%" PRId32 ":\n", label.name, label.number);
    }
    else 
    {
        write(compiler, "%s:\n", label.name);
    }
}

void compileError(Compiler* compiler, CompilerError error)
{
    ASSERT_COMPILER(compiler);

    compiler->status = error;

    printf("COMPILATION ERROR: %s\n", errorString(error));
}

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
    writeStdFunctions(compiler);

    // Skipping strings declarations
    Node* curDeclaration = compiler->tree;
    while (curDeclaration != nullptr && curDeclaration->type == SDECL_TYPE)
    {
        curDeclaration = curDeclaration->left;
    }

    CUR_FUNC = compiler->table->functionsData.functions;

    while (curDeclaration != nullptr)
    {
        compileFunction(compiler, curDeclaration->right);
        curDeclaration = curDeclaration->left;
        CUR_FUNC++;

        write(compiler, "\n\n");
    }

    write(compiler, "section .data\n");
    writeStdData(compiler);
    writeStringsData(compiler);
    writeNewLine(compiler);

    write(compiler, "section .bss\n");
    writeStdBSS(compiler);

    fclose(compiler->file);

    return compiler->status;
}
//===================================Compiler===================================


//==============================Write NASM comments=============================
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

void writeIndented(Compiler* compiler, const char* format, ...)
{
    ASSERT_COMPILER(compiler);
    assert(format);

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
        write(compiler, "%s", CUR_FUNC->varsData.vars[i]);

        if (i < CUR_FUNC->paramsCount - 1)
        {
            write(compiler, ", ");
        }
    }

    write(compiler, "\n; vars:   ");

    for (size_t i = CUR_FUNC->paramsCount; i < CUR_FUNC->varsData.count; i++)
    {
        write(compiler, "%s", CUR_FUNC->varsData.vars[i]);

        if (i < CUR_FUNC->varsData.count - 1)
        {
            write(compiler, ", ");
        }
    }

    write(compiler, "\n");
    writeHorizontalLine(compiler);

    write(compiler, "%s:\n", CUR_FUNC->name);
}
//==============================Write NASM comments=============================


//==================================Write data==================================
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

// FIXME: rdi, rsi, rdx, rcx, r8, r9

// FIXME: make filenames parameters
void writeStdFunctions(Compiler* compiler)
{
    ASSERT_COMPILER(compiler);
    
    char*  stdioNasm      = nullptr;
    size_t stdioNasmBytes = 0;

    if (!loadFile("../potter_tongue_libs/io.nasm", &stdioNasm, &stdioNasmBytes))
    {
        compileError(compiler, COMPILER_ERROR_NO_STDIO_NASM_CODE);
        return;
    }

    fwrite(stdioNasm, 1, stdioNasmBytes, compiler->file);
    writeNewLine(compiler);

    free(stdioNasm);

    // Adding standard functions to the compiler's SymbolTable and LabelManager
    for (size_t i = 0; i < STANDARD_FUNCTIONS_COUNT; i++)
    {
        const StdFunctionInfo* functionInfo = &STANDARD_FUNCTIONS[i];
        Function* function = pushFunction(compiler->table, functionInfo->workingName);

        Label label = getExistingLabel(compiler, {0, nullptr, function->name, -1});
        insertLabel(&compiler->labelManager.labelArray, label);

        for (size_t param = 0; param < functionInfo->parametersCount; param++)
        {
            pushParameter(function, functionInfo->parameters[param]);
        }
    }
}

void writeStdData(Compiler* compiler)
{
    ASSERT_COMPILER(compiler);

    write(compiler, "IO_BUFFER_SIZE equ %zu\n",
                    IO_BUFFER_SIZE);
}

void writeStringsData(Compiler* compiler)
{
    ASSERT_COMPILER(compiler);

    StringsData* stringsData = &compiler->table->stringsData;

    for (size_t i = 0; i < stringsData->count; i++)
    {
        if (stringsData->strings[i].name != nullptr)
        {
            writeLabel(compiler, {0, nullptr, stringsData->strings[i].name, -1});
        } 
        else 
        {
            writeLabel(compiler, {0, 
                                  nullptr, 
                                  "STR", 
                                  getStringNumber(compiler->table, stringsData->strings + i)});
        }

        if (stringsData->strings[i].content[0] == '\n')
        {
            writeIndented(compiler, "db `\\n`, 0\n");
        }
        else 
        {
            writeIndented(compiler, "db \"%s\", 0\n", stringsData->strings[i].content);
        }
    }
}

void writeStdBSS(Compiler* compiler)
{
    ASSERT_COMPILER(compiler);

    write(compiler, "IO_BUFFER:\n");
    writeIndented(compiler, "resb %zu\n", IO_BUFFER_SIZE);
    write(compiler, "IO_BUFFER_END:\n");
}
//==================================Write data==================================


//==================================Compilation==================================
void compileFunction(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node);

    writeFunctionHeader(compiler);

    write_push_r64(compiler, RBP);
    write_mov_r64_r64(compiler, RBP, RSP);
    
    // FIXME: local vars change
    if (CUR_FUNC->varsData.count != CUR_FUNC->paramsCount)
    {
        write_sub_r64_imm32(compiler, RSP, 8 * (CUR_FUNC->varsData.count - CUR_FUNC->paramsCount)); 
    }

    write(compiler, "\n");
    compileBlock(compiler, node->left);

    write(compiler, ".RETURN:\n");
    write_mov_r64_r64(compiler, RSP, RBP);
    write_pop_r64(compiler, RBP);
    write_ret(compiler);
}

void compileBlock(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node);

    Node* curStatement = node->right;
    while (curStatement != nullptr)
    {
        compileStatement(compiler, curStatement);
        curStatement = curStatement->right;
    }
}

void compileStatement(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node);
    assert(node->left);

    switch (node->left->type)
    {
        case COND_TYPE:   { compileCondition        (compiler, node->left); break; }
        case LOOP_TYPE:   { compileLoop             (compiler, node->left); break; }
        case VDECL_TYPE:  { compileAssignment       (compiler, node->left); break; }
        case ASSIGN_TYPE: { compileAssignment       (compiler, node->left); break; }
        case ADECL_TYPE:  { compileArrayDeclaration (compiler, node->left); break; }
        case JUMP_TYPE:   { compileReturn           (compiler, node->left); break; }
        default:          { compileExpression       (compiler, node->left); break; }
    }
}

void compileCondition(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node);

    writeIndented(compiler, "; ==== if-else statement ====\n");

    writeIndented(compiler, "; condition's expression\n");
    compileExpression(compiler, node->left);             

    int32_t labelNum  = nextLabelNumber(compiler, LABEL_COND);
    Label   elseLabel = getExistingLabel(compiler, {0, CUR_FUNC->name, ".ELSE_",       labelNum});
    Label   endLabel  = getExistingLabel(compiler, {0, CUR_FUNC->name, ".END_IF_ELSE", labelNum});
    
    write_test_r64_r64(compiler, RAX, RAX);            
    write_jz_rel32(compiler, elseLabel);

    writeNewLine(compiler);

    writeIndented(compiler, "; if true\n");
    compileBlock(compiler, node->right->left);    

    write_jmp_rel32(compiler, endLabel);
    writeNewLine(compiler);

    writeLabel(compiler, elseLabel);

    if (node->right->right != nullptr)
    {
        compileBlock(compiler, node->right->right);
    }

    writeLabel(compiler, endLabel);
}

void compileLoop(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node);

    int32_t labelNum   = nextLabelNumber(compiler, LABEL_LOOP);
    Label   whileLabel = getExistingLabel(compiler, {0, CUR_FUNC->name, ".WHILE_",     labelNum});
    Label   endLabel   = getExistingLabel(compiler, {0, CUR_FUNC->name, ".END_WHILE_", labelNum});

    writeIndented(compiler, "; ==== while ====\n");
    writeLabel(compiler, whileLabel);

    writeIndented(compiler, "; exit condition\n");
    compileExpression(compiler, node->left);
    
    write_test_r64_r64(compiler, RAX, RAX);

    write_jz_rel32(compiler, endLabel);
    writeNewLine(compiler);

    writeIndented(compiler, "; loop body\n");
    compileBlock(compiler, node->right);

    write_jmp_rel32(compiler, whileLabel);
    writeLabel(compiler, endLabel);
}

void compileAssignment(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node);

    if (node->left->type == ID_TYPE)
    {
        compileAssignmentVar(compiler, node);
    }
    else
    {
        compileAssignmentArray(compiler, node);
    }
}

void compileAssignmentVar(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node);

    const char* var       = node->left->data.id;
    Mem64       varMemory = mem64BaseDisp(RBP, getVarOffset(CUR_FUNC, var)); 

    writeIndented(compiler, "; --- assignment to %s ---\n", var);

    writeIndented(compiler, "; evaluating expression\n");
    compileExpression(compiler, node->right);      

    write_mov_m64_r64(compiler, varMemory, RAX); 
    writeIndented(compiler, "; --- assignment to %s ---\n\n", var);
}

void compileAssignmentArray(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node);

    const char* var       = node->left->left->data.id;
    Mem64       varMemory = mem64BaseDisp(RBP, getVarOffset(CUR_FUNC, var)); 

    writeIndented(compiler, "; --- assignment to %s ---\n", var);
    write_mov_r64_m64(compiler, RAX, varMemory);             

    write_push_r64(compiler, RAX, "save variable");          
    compileExpression(compiler, node->left->right);                  
    write_neg_r64(compiler, RAX, "addressing in memory is from right to left");

    write_push_r64(compiler, RAX, "save index");
    compileExpression(compiler, node->right);

    write_pop_r64(compiler, RCX, "restore index to rcx");
    write_pop_r64(compiler, RBX, "restore variable to rbx"); 

    Mem64 arrayElementMemory = {};
    arrayElementMemory.base  = RBX;
    arrayElementMemory.index = RCX;
    arrayElementMemory.scale = 8;
    write_mov_m64_r64(compiler, arrayElementMemory, RAX); 

    writeIndented(compiler, "; --- assignment to %s ---\n", var);
    writeNewLine(compiler);
}

void compileArrayDeclaration(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node);

    const char* var       = node->left->data.id;
    Mem64       varMemory = mem64BaseDisp(RBP, getVarOffset(CUR_FUNC, var)); 

    writeIndented(compiler, "; --- declaring array %s ---\n", var);
    
    writeIndented(compiler, "; evaluating expression (array's size)\n");
    compileExpression(compiler, node->right);     
    write_sal_r64_imm8(compiler, RAX, 3, "*8 to get array's size in bytes");
    write_sub_r64_imm32(compiler, RAX, 8, "to mitigate the next instruction");

    write_sub_r64_imm32(compiler, RSP, 8);
    write_mov_m64_r64(compiler, varMemory, RSP);
    write_sub_r64_r64(compiler, RSP, RAX);

    writeNewLine(compiler);
}

void compileReturn(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node);

    compileExpression(compiler, node->right);
    
    Label label = getExistingLabel(compiler, {0, CUR_FUNC->name, ".RETURN", -1});
    write_jmp_rel32(compiler, label);
}

void compileExpression(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node);

    switch (node->type)
    {
        case MATH_TYPE:       { compileMath      (compiler, node); break; }
        case MEM_ACCESS_TYPE: { compileMemAccess (compiler, node); break; }
        case STRING_TYPE:     { compileString    (compiler, node); break; }
        case NUMBER_TYPE:     { compileNumber    (compiler, node); break; }
        case ID_TYPE:         { compileVar       (compiler, node); break; }
        case CALL_TYPE:       { compileCall      (compiler, node); break; }

        default:              { assert(!"Valid node type");   break; }
    }
}

void compileMath(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node);

    MathOp operation = node->data.operation;

    compileExpression(compiler, node->left);
    
    writeNewLine(compiler);
    write_push_r64(compiler, RAX, "save rax");
    writeNewLine(compiler);

    compileExpression(compiler, node->right);

    write_mov_r64_r64(compiler, RBX, RAX);
    write_pop_r64(compiler, RAX, "restore rax");

    writeNewLine(compiler);

    // FIXME: add isBinaryOperation function
    if (operation > DIV_OP)
    {
        compileCompare(compiler, node);
        return;
    }

    switch (node->data.operation)
    {
        case ADD_OP: { write_add_r64_r64  (compiler, RAX, RBX); break; }
        case SUB_OP: { write_sub_r64_r64  (compiler, RAX, RBX); break; }
        case MUL_OP: { write_imul_r64_r64 (compiler, RAX, RBX); break; }
        
        case DIV_OP: 
        { 
            write_xor_r64_r64(compiler, RDX, RDX, "; rdx = 0 (for the division below)");
            write_idiv_r64(compiler, RBX);
            break; 
        }

        default: { assert(!"Valid math op"); break; }
    }
}

void compileCompare(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node);

    int32_t     labelNum  = nextLabelNumber(compiler, LABEL_CMP);
    const char* funcName  = CUR_FUNC->name; 
    Label       labelTrue = getExistingLabel(compiler, {0, funcName, ".CMP_TRUE_", labelNum});
    Label       labelEnd  = getExistingLabel(compiler, {0, funcName, ".CMP_END_",  labelNum});

    write_cmp_r64_r64(compiler, RAX, RBX);

    switch (node->data.operation)
    {
        case EQUAL_OP:         { write_je_rel32  (compiler, labelTrue); break; }
        case NOT_EQUAL_OP:     { write_jne_rel32 (compiler, labelTrue); break; }
        case LESS_OP:          { write_jl_rel32  (compiler, labelTrue); break; }
        case GREATER_OP:       { write_jg_rel32  (compiler, labelTrue); break; }
        case LESS_EQUAL_OP:    { write_jle_rel32 (compiler, labelTrue); break; }
        case GREATER_EQUAL_OP: { write_jge_rel32 (compiler, labelTrue); break; }

        default:               { assert(!"Invalid cmp op"); break; }
    }

    write_xor_r64_r64(compiler, RAX, RAX, "false");
    write_jmp_rel32(compiler, labelEnd);

    writeLabel(compiler, labelTrue);

    write_mov_r64_imm64(compiler, RAX, 1, "true");

    writeLabel(compiler, labelEnd);
}

void compileMemAccess(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node);

    Mem64 varMemory = mem64BaseDisp(RBP, getVarOffset(CUR_FUNC, node->left->data.id)); 
    write_mov_r64_m64(compiler, RAX, varMemory);             

    write_push_r64(compiler, RAX, "save variable");          
    compileExpression(compiler, node->right);                  
    write_neg_r64(compiler, RAX, "addressing in memory is from right to left");
    write_pop_r64(compiler, RBX, "restore variable to rbx"); 

    Mem64 arrayElementMemory = {};
    arrayElementMemory.base  = RBX;
    arrayElementMemory.index = RAX;
    arrayElementMemory.scale = 8;
    write_mov_r64_m64(compiler, RAX, arrayElementMemory);    
}

void compileString(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node);

    String* string = getStringByContent(compiler->table, node->data.string);

    Label label = getExistingLabel(compiler, {0, 
                                              nullptr, 
                                              "STR", 
                                              getStringNumber(compiler->table, string)});
    write_mov_r64_imm64(compiler, RAX, label);
}

void compileNumber(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node);

    write_mov_r64_imm64(compiler, RAX, node->data.number);
}

void compileVar(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node);

    int varOffset = getVarOffset(CUR_FUNC, node->data.id);
    if (varOffset != -1)
    {
        Mem64 varMemory = mem64BaseDisp(RBP, varOffset); 
        write_mov_r64_m64(compiler, RAX, varMemory);
    }
    else
    {   
        Label label = getExistingLabel(compiler, {0, nullptr, node->data.id, -1});
        write_mov_r64_imm64(compiler, RAX, label);
    }
}

void compileParamList(Compiler* compiler, Node* node, const Function* function)
{
    ASSERT_COMPILER(compiler);
    assert(node);

    Node*  curParamExpr = node->right;
    size_t paramNumber  = function->paramsCount;
    while (curParamExpr != nullptr)
    {
        writeIndented(compiler, "; param %zu\n", paramNumber);

        compileExpression(compiler, curParamExpr->left); // (rax = expression)
        write_push_r64(compiler, RAX);                 // push rax

        curParamExpr = curParamExpr->right;
        paramNumber--;
    }
}

void compileCall(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler); 
    assert(node);

    writeIndented(compiler, "; --- calling %s() ---\n", node->left->data.id);

    // if (compileStdCall(compiler, node)) { return; } // FIXME: 

    Function* function = getFunction(compiler->table, node->left->data.id);
    if (function == nullptr) 
    {
        compileError(compiler, COMPILER_ERROR_CALL_UNDEFINED_FUNCTION);
        return; 
    }

    compileParamList(compiler, node, function);
    writeNewLine(compiler);

    Label label = getExistingLabel(compiler, {0, nullptr, function->name, -1});
    write_call_rel32(compiler, label);

    if (function->paramsCount > 0)
    {
        write_add_r64_imm32(compiler, RSP, function->paramsCount * 8);
    }

    writeNewLine(compiler);
}

bool compileStdCall(Compiler* compiler, Node* node)
{
    ASSERT_COMPILER(compiler);
    assert(node);

    const char* name = node->left->data.id;
    if (strcmp(name, KEYWORDS[PRINT_KEYWORD].string) == 0)
    {
        compileParamList(compiler, node, getFunction(compiler->table, "flagrate"));
        // compileExpression(compiler, node->right->left);
        
    } 
    // else if (strcmp(name, KEYWORDS[SCAN_KEYWORD].string) == 0)
    // {
    //     fprintf(OUTPUT, "in\n");
    // } 
    // else if (strcmp(name, KEYWORDS[FLOOR_KEYWORD].string) == 0)
    // {
    //     compileExpression(compiler, node->right->left);
    //     fprintf(OUTPUT, "flr\n");
    // }
    // else if (strcmp(name, KEYWORDS[SQRT_KEYWORD].string) == 0)
    // {
    //     compileExpression(compiler, node->right->left);
    //     fprintf(OUTPUT, "sqrt\n");
    // }  
    // else if (strcmp(name, KEYWORDS[RAND_JUMP_KEYWORD].string) == 0)
    // {
    //     fprintf(OUTPUT, "rndjmp\n");
    // }
    else 
    {
        return false;
    }

    return true; // FIXME: switch back to true
}
//==================================Compilation==================================