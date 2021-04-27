#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "expression_tree.h"
#include "../../libs/utilib.h"
#include "../../libs/file_manager.h"

const size_t MAX_COMMAND_LENGTH = 256;

const char* NODE_GRAPH_STYLES[TYPES_COUNT] = {
        ", color=\"#000000\", fillcolor=\"#FFFFFF\", fontcolor=\"#000000\"", /* FDECL_TYPE      */
        ", color=\"#000000\", fillcolor=\"#FFFFFF\", fontcolor=\"#000000\"", /* VDECL_TYPE      */
        ", color=\"#000000\", fillcolor=\"#FFFFFF\", fontcolor=\"#000000\"", /* ADECL_TYPE      */

        ", color=\"#000000\", fillcolor=\"#FFFFFF\", fontcolor=\"#000000\"", /* MEM_ACCESS_TYPE */
        ", color=\"#F06292\", fillcolor=\"#fff9fa\", fontcolor=\"#F06292\"", /* ID_TYPE         */
        "",                                                                  /* EXPR_LIST_TYPE  */

        ", color=\"#000000\", fillcolor=\"#FFFFFF\", fontcolor=\"#000000\"", /* BLOCK_TYPE      */
        ", color=\"#000000\", fillcolor=\"#FFFFFF\", fontcolor=\"#000000\"", /* STATEMENT_TYPE  */

        ", color=\"#367ACC\", fillcolor=\"#E0F5FF\", fontcolor=\"#4881CC\"", /* COND_TYPE       */ 
        ", color=\"#367ACC\", fillcolor=\"#E0F5FF\", fontcolor=\"#4881CC\"", /* IFELSE_TYPE     */
        ", color=\"#367ACC\", fillcolor=\"#E0F5FF\", fontcolor=\"#4881CC\"", /* LOOP_TYPE       */
        "",                                                                  /* ASSIGN_TYPE     */

        "",                                                                  /* CALL_TYPE       */
        "",                                                                  /* JUMP_TYPE       */

        "",                                                                  /* MATH_TYPE       */
        ", color=\"#D2691E\", fillcolor=\"#FFFAEF\", fontcolor=\"#FF7F50\"", /* NUMBER_TYPE     */

        ", color=\"#000000\", fillcolor=\"#FFFFFF\", fontcolor=\"#000000\"", /* SDECL_TYPE      */
        ", color=\"#75A673\", fillcolor=\"#EDFFED\", fontcolor=\"#75A673\""  /* STRING_TYPE     */
    };

const char*  UNIVERSAL_MAIN_NAME  = "main";
const char*  UNIVERSAL_PRINT_NAME = "print";
const char*  UNIVERSAL_SCAN_NAME  = "scan";
const char*  UNIVERSAL_FLOOR_NAME = "floor";
const char*  UNIVERSAL_SQRT_NAME  = "sqrt";

const size_t FIRST_FDECL_LENGTH   = strlen("0 | 0");
const size_t LEAF_CHILDREN_LENGTH = strlen("{ } { }");
const size_t LEFT_CHILD_LENGTH    = strlen("{ } { ");

#define CHECK_NULL(value, action) if (value == nullptr) { action; }

int  counterFileUpdate (const char* filename);
void graphDumpSubtree  (FILE* file, const Node* node, bool detailed);

void destroySubtree(Node* root)
{
    CHECK_NULL(root, return);

    destroySubtree(root->left);
    destroySubtree(root->right);

    deleteNode(root);
}

Node* newNode()
{
    return (Node*) calloc(1, sizeof(Node));
}

Node* newNode(NodeType type, NodeData data, Node* left, Node* right)
{
    Node* node = newNode();
    CHECK_NULL(node, return nullptr);

    node->type   = type;
    node->data   = data;

    node->parent = nullptr;
    node->left   = left;
    node->right  = right;

    if (left  != nullptr) { left->parent  = node; }
    if (right != nullptr) { right->parent = node; }

    return node;
}

void deleteNode(Node* node)
{
    assert(node);

    node->parent = nullptr;
    node->left   = nullptr;
    node->right  = nullptr;

    free(node);
}

void setLeft(Node* root, Node* left)
{
    assert(root);

    root->left = left;
    if (left != nullptr) { left->parent = root; } 
}

void setRight(Node* root, Node* right)
{
    assert(root);

    root->right = right;
    if (right != nullptr) { right->parent = root; } 
}

void copyNode(Node* dest, const Node* src)
{
    assert(dest);
    assert(src);

    dest->type   = src->type;
    dest->data   = src->data;
    dest->parent = src->parent;
    dest->left   = src->left;
    dest->right  = src->right;

    if (src->left  != nullptr) { src->left->parent  = dest; }
    if (src->right != nullptr) { src->right->parent = dest; }
}

Node* copyTree(const Node* node)
{
    if (node == nullptr) { return nullptr; }

    return newNode(node->type, node->data, copyTree(node->left), copyTree(node->right));
}

bool isLeft(const Node* node)
{
    assert(node);
    assert(node->parent);

    return node == node->parent->left;
}

void setData(Node* node, NodeType type, NodeData data)
{
    assert(node);

    node->type = type;
    node->data = data;
}

void setDataNumber(Node* node, int64_t number)
{
    assert(node);

    node->type        = NUMBER_TYPE;
    node->data.number = number;
}

void setDataOperation(Node* node, MathOp op)
{
    assert(node);

    node->type           = MATH_TYPE;
    node->data.operation = op;
}

void setDataId(Node* node, const char* id)
{
    assert(node);

    node->type    = ID_TYPE;
    node->data.id = id;
}

void setDataIsVoidFunction(Node* node, bool isVoidFunction)
{
    assert(node);

    node->type = FDECL_TYPE; // FIXME: Needed?
    node->data.isVoidFunction = isVoidFunction;
}

int counterFileUpdate(const char* filename)
{
    assert(filename);

    int count = 0;

    FILE* counterFile = fopen(filename, "r+");
    if (counterFile == nullptr)
    {
        counterFile = fopen(filename, "w");
        count = -1;
    }
    else
    {
        fscanf(counterFile, "%d", &count); 
        fseek(counterFile, 0, SEEK_SET);
    }

    fprintf(counterFile, "%u", count + 1);
    fclose(counterFile);

    return count;
}

void graphDump(const Node* root, const char* treeFilename, const char* outputFilename, bool detailed)
{
    assert(root);
    assert(treeFilename);
    assert(outputFilename);

    FILE* file = fopen(treeFilename, "w");
    assert(file);

    fprintf(file, "digraph structs {\n"
                  "\tnode [shape=\"%s\", "
                  "style=\"filled\", "
                  "color=\"#52527A\", "
                  "fontcolor=\"#52527A\", "
                  "fillcolor=\"#E1E1EA\"];\n\n",
                  detailed ? "record" : "circle");

    if (root == nullptr) 
    { 
        fprintf(file, "\t\"ROOT\" [label = \"Empty database\"];\n"); 
    }
    else                 
    { 
        graphDumpSubtree(file, root, detailed);
    }

    fprintf(file, "}");
    fclose(file);

    char dotCmd[MAX_COMMAND_LENGTH] = {};
    snprintf(dotCmd, sizeof(dotCmd), "dot -Tsvg %s -o %s", treeFilename, outputFilename);
    system(dotCmd);
}

void graphDumpSubtree(FILE* file, const Node* node, bool detailed)
{
    assert(file);
    
    if (node == nullptr) { return; }

    fprintf(file, "\t\"%p\" [label=\"", (void*) node);
    if (detailed) { fprintf(file, "{ "); }

    NodeData data = node->data;

    switch (node->type)
    {
        case FDECL_TYPE:     
        { 
            fprintf(file, "FDECL (%s)", data.isVoidFunction ? "void" : "non-void"); 
            break; 
        }
        
        case VDECL_TYPE:      { fprintf(file, "=");           break; }
        case ADECL_TYPE:      { fprintf(file, "Array");       break; }

        case MEM_ACCESS_TYPE: { fprintf(file, "[ ]");         break; }
        case ID_TYPE:         { fprintf(file, "%s", data.id); break; } 
        case EXPR_LIST_TYPE:  { fprintf(file, "param");       break; } 
        
        case BLOCK_TYPE:      { fprintf(file, "Block");       break; }
        case STATEMENT_TYPE:  { fprintf(file, "S");           break; }

        case COND_TYPE:       { fprintf(file, "if");          break; } 
        case IFELSE_TYPE:     { fprintf(file, "if-else");     break; } 
        case LOOP_TYPE:       { fprintf(file, "while");       break; } 
        case ASSIGN_TYPE:     { fprintf(file, "=");           break; } 
        
        case CALL_TYPE:       { fprintf(file, "call");        break; } 
        case JUMP_TYPE:       { fprintf(file, "return");      break; } 
        
        case MATH_TYPE:       { fprintf(file, "\\%s", mathOpToString(data.operation)); break; } 
        case NUMBER_TYPE:     { fprintf(file, "%" PRId64 "", data.number);             break; } 
        case SDECL_TYPE:      { fprintf(file, "SDECL");                                break; } 
        
        case STRING_TYPE:    
        { 
            // this is needed when 'circumrota' is used
            if (data.string[0] == '\n') 
            { 
                fprintf(file, KEYWORDS[STR_NEW_LINE_KEYWORD].string);             
            }
            else                        
            { 
                fprintf(file, "\\\"%s\\\"",  data.string); 
            }
            
            break; 
        } 

        default:    
        { 
            assert(!"VALID TYPE"); 
            break; 
        } 
    }

    if (detailed) { fprintf(file, " | %s}", nodeTypeToString(node->type)); }

    fprintf(file, "\" %s];\n", NODE_GRAPH_STYLES[node->type]);

    if (node->parent != nullptr)
    {
        if (isLeft(node))
        {
            fprintf(file, "\t\"%p\":sw->\"%p\";\n", (void*) node->parent, (void*) node);
        }
        else
        {
            fprintf(file, "\t\"%p\":se->\"%p\";\n", (void*) node->parent, (void*) node);
        }
    }   

    graphDumpSubtree(file, node->left,  detailed);
    graphDumpSubtree(file, node->right, detailed);
}

// TODO: add new features support
void dumpToFile(FILE* file, const Node* node)
{
    assert(file);
    if (node == nullptr) { return; }

    fprintf(file, "%d | ", node->type);

    if (node->type == NUMBER_TYPE)
    {
        fprintf(file, "%" PRId64 " ", node->data.number);
    }
    else if (node->type == ID_TYPE)
    {
        if (strcmp(node->data.id, MAIN_FUNCTION_NAME) == 0)
        {
            fprintf(file, "%s ", UNIVERSAL_MAIN_NAME);
        }
        else if (strcmp(node->data.id, KEYWORDS[PRINT_KEYWORD].string) == 0)
        {
            fprintf(file, "%s ", UNIVERSAL_PRINT_NAME);   
        }
        else if (strcmp(node->data.id, KEYWORDS[SCAN_KEYWORD].string) == 0)
        {
            fprintf(file, "%s ", UNIVERSAL_SCAN_NAME);   
        }
        else if (strcmp(node->data.id, KEYWORDS[FLOOR_KEYWORD].string) == 0)
        {
            fprintf(file, "%s ", UNIVERSAL_FLOOR_NAME);   
        }
        else if (strcmp(node->data.id, KEYWORDS[SQRT_KEYWORD].string) == 0)
        {
            fprintf(file, "%s ", UNIVERSAL_SQRT_NAME);   
        }
        else
        {
            fprintf(file, "%s ", node->data.id);
        }
    }
    else 
    {
        fprintf(file, "%d ", node->data.operation);
    }

    fprintf(file, "{ ");
    dumpToFile(file, node->left);
    fprintf(file, "} ");

    fprintf(file, "{ ");
    dumpToFile(file, node->right);
    fprintf(file, "} ");
}

// TODO: add new features support
Node* readTreeFromFile(const char* filename)
{
    char*  buffer     = nullptr;
    size_t bufferSize = 0;
    size_t ofs        = 0;

    if (!loadFile(filename, &buffer, &bufferSize))
    {
        return nullptr;
    }

    Node* node = newNode(FDECL_TYPE, {}, nullptr, nullptr);
    ofs += FIRST_FDECL_LENGTH; // to skip the first decl info

    while (ofs < bufferSize)
    {
        while (buffer[ofs] == ' ') { ofs++; }

        if (buffer[ofs] == '{')
        {
            if (ofs + LEAF_CHILDREN_LENGTH < bufferSize && strncmp(buffer + ofs, "{ } { }", LEAF_CHILDREN_LENGTH) == 0)
            {
                ofs += LEAF_CHILDREN_LENGTH;
                continue;
            }

            if (ofs + LEFT_CHILD_LENGTH < bufferSize && strncmp(buffer + ofs, "{ } { ", LEFT_CHILD_LENGTH) == 0)
            {
                setRight(node, newNode());
                node = node->right;
                ofs += LEFT_CHILD_LENGTH;
                continue;
            }

            if (node->left == nullptr)
            {
                setLeft(node, newNode());
                node = node->left;
            }
            else if (buffer[ofs + 2] == '}') // in case '{ }'
            {
                ofs += 3; // to skip '{ }'
                continue;
            }
            else
            {
                setRight(node, newNode());
                node = node->right;
            }

            ofs++;
            continue;
        } 

        if (buffer[ofs] == '}')
        {
            node = node->parent;
            ofs++;
            continue;
        }

        int len = 0;
        sscanf(buffer + ofs, "%d | %n", (int*) &node->type, &len);
        ofs += len;

        if (node->type == NUMBER_TYPE)
        {
            sscanf(buffer + ofs, "%" PRId64 " %n", &(node->data.number), &len);
        }
        else if (node->type == ID_TYPE)
        {
            len = strspn(buffer + ofs, LETTERS);

            if (strncmp(buffer + ofs, UNIVERSAL_MAIN_NAME, len) == 0)
            {
                node->data.id = MAIN_FUNCTION_NAME;
            }
            else if (strncmp(buffer + ofs, UNIVERSAL_PRINT_NAME, len) == 0)
            {
                node->data.id = KEYWORDS[PRINT_KEYWORD].string;
            }
            else if (strncmp(buffer + ofs, UNIVERSAL_SCAN_NAME, len) == 0)
            {
                node->data.id = KEYWORDS[SCAN_KEYWORD].string;
            }
            else if (strncmp(buffer + ofs, UNIVERSAL_FLOOR_NAME, len) == 0)
            {
                node->data.id = KEYWORDS[FLOOR_KEYWORD].string;
            }
            else if (strncmp(buffer + ofs, UNIVERSAL_SQRT_NAME, len) == 0)
            {
                node->data.id = KEYWORDS[SQRT_KEYWORD].string;
            }
            else
            {
                node->data.id = copyString(buffer + ofs, len);
            }
        }
        else
        {
            sscanf(buffer + ofs, "%d %n", (int*) &(node->data.operation), &len);
        }

        ofs += len;
    }

    free(buffer);

    return node;
}

const char* nodeTypeToString(NodeType type)
{
    switch (type)
    {
        case FDECL_TYPE:      { return TO_STR(FDECL_TYPE);      }
        case VDECL_TYPE:      { return TO_STR(VDECL_TYPE);      }
        case ADECL_TYPE:      { return TO_STR(ADECL_TYPE);      }

        case MEM_ACCESS_TYPE: { return TO_STR(MEM_ACCESS_TYPE); }
        case ID_TYPE:         { return TO_STR(ID_TYPE);         }
        case EXPR_LIST_TYPE:  { return TO_STR(EXPR_LIST_TYPE);  }
        
        case BLOCK_TYPE:      { return TO_STR(BLOCK_TYPE);      }
        case STATEMENT_TYPE:  { return TO_STR(STATEMENT_TYPE);  }

        case COND_TYPE:       { return TO_STR(COND_TYPE);       }
        case IFELSE_TYPE:     { return TO_STR(IFELSE_TYPE);     }
        case LOOP_TYPE:       { return TO_STR(LOOP_TYPE);       }
        case ASSIGN_TYPE:     { return TO_STR(ASSIGN_TYPE);     }

        case CALL_TYPE:       { return TO_STR(CALL_TYPE);       }
        case JUMP_TYPE:       { return TO_STR(JUMP_TYPE);       }

        case MATH_TYPE:       { return TO_STR(MATH_TYPE);       }
        case NUMBER_TYPE:     { return TO_STR(NUMBER_TYPE);     }

        case SDECL_TYPE:      { return TO_STR(SDECL_TYPE);      }
        case STRING_TYPE:     { return TO_STR(STRING_TYPE);     }

        default:              { return nullptr; }          
    };

    return nullptr;
}