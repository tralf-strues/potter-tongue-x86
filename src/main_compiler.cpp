#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "parser/tokenizer.h"
#include "parser/parser.h"
#include "compiler/compiler.h"
#include <file_manager/file_manager.h>

#define UTB_DEFINITIONS
#include "../libs/utilib.h"

enum Error
{
    NO_ERROR,
    INPUT_UNSPECIFIED,
    OUTPUT_UNSPECIFIED,
    INPUT_LOAD_FAILED,
    COMPILATION_FAILED
};

enum Flag
{
    FLAG_TOKEN_DUMP,
    FLAG_SIMPLE_GRAPH_DUMP,
    FLAG_DETAILED_GRAPH_DUMP,
    FLAG_OPEN_GRAPH_DUMP,
    FLAG_TREE_DUMP,
    FLAG_SYMB_TABLE_DUMP,
    FLAG_USE_NUMERICS,
    FLAG_HELP,
    FLAG_OUTPUT,

    TOTAL_FLAGS
};

struct FlagManager
{
    int          curArg;
    int          argc;
    const char** argv;
    const char*  input;
    const char*  output;

    bool         tokenDumpEnabled;
    bool         simpleGraphDumpEnabled;
    bool         detailedGraphDumpEnabled;
    bool         openGraphDumpEnabled;
    bool         treeDumpEnabled;
    bool         symbTableDumpEnabled;
    bool         useNumerics;
};

struct FlagSpecification
{
    Flag        flag;
    const char* string;
    Error       (*process) (FlagManager* flagManager);
    const char* helpMessage;
};

Error processFlagTokenDump         (FlagManager* flagManager);
Error processFlagSimpleGraphDump   (FlagManager* flagManager);
Error processFlagDetailedGraphDump (FlagManager* flagManager);
Error processFlagOpenGraphDump     (FlagManager* flagManager);
Error processFlagTreeDump          (FlagManager* flagManager);
Error processFlagSymbTableDump     (FlagManager* flagManager);
Error processFlagUseNumerics       (FlagManager* flagManager);
Error processFlagHelp              (FlagManager* flagManager);
Error processFlagOutput            (FlagManager* flagManager);

Error processFlags  (FlagManager* flagManager);
void  printHelp     ();
Error compile       (const FlagManager* flagManager);
void  makeGraphDump (const FlagManager* flagManager, const Node* tree, bool detailed);

const char*  DEFAULT_OUTPUT      = "a.asm";
const size_t MAX_FILENAME_LENGTH = 128;
const size_t MAX_COMMAND_LENGTH  = 256;

const char* FLAGS_HELP_MESSAGES[TOTAL_FLAGS] = 
{
    /*====FLAG_TOKEN_DUMP====*/
    "\tPrint parsed tokens in the following format:\n"
    "\tToken <token_number>:\n"
    "\t\ttype = <type_number>\n"
    "\t\tdata = <token_data>\n", 

    /*====FLAG_SIMPLE_GRAPH_DUMP====*/
    "\tWrite simple graph dump in .svg format.\n",

    /*====FLAG_DETAILED_GRAPH_DUMP====*/
    "\tWrite detailed graph dump in .svg format.\n",

    /*====FLAG_OPEN_GRAPH_DUMP====*/
    "\tWhen written the graph dump, open it with the default program for viewing .svg format.\n",

    /*====FLAG_TREE_DUMP====*/
    "\tWrites the syntax tree to file.\n",

    // FIXME: outdated
    /*====FLAG_SYMB_TABLE_DUMP====*/
    "\tPrints the symbol table in the following format:\n"
    "\tSymbol table:\n"
    "\t\tfunctionsCapacity = <capacity of the dynamic array of functions>\n"
    "\t\tfunctionsCount    = <total number of functions>\n\n"
    "\t\tfunctions = { \n"
    "\t\t\t{ name='<>', varsCapacity=<>, varsCount=<>, paramsCount=<>, \n"
    "\t\t\t  vars=['<>', '<>', '<>']\n"
    "\t\t\t}\n"
    "\t\t}\n",

    /*====FLAG_USE_NUMERICS====*/
    "\tAllow using numbers (e.g. '3' instead of 'tria', or '22') in the input file.\n",

    /*====FLAG_HELP====*/
    "\tPrint this message.\n",

    /*====FLAG_OUTPUT====*/
    "\tSpecify output file.\n"
};

const FlagSpecification FLAG_SPECIFICATIONS[TOTAL_FLAGS] = 
{
    { FLAG_TOKEN_DUMP,      
      "--token-dump",      
      processFlagTokenDump,     
      FLAGS_HELP_MESSAGES[FLAG_TOKEN_DUMP] },

    { FLAG_SIMPLE_GRAPH_DUMP,
      "--simple-graph-dump",
      processFlagSimpleGraphDump,
      FLAGS_HELP_MESSAGES[FLAG_SIMPLE_GRAPH_DUMP] },

    { FLAG_DETAILED_GRAPH_DUMP,
      "--detailed-graph-dump",
      processFlagDetailedGraphDump,
      FLAGS_HELP_MESSAGES[FLAG_DETAILED_GRAPH_DUMP] },

    { FLAG_OPEN_GRAPH_DUMP,
      "--open-graph-dump",
      processFlagOpenGraphDump,
      FLAGS_HELP_MESSAGES[FLAG_OPEN_GRAPH_DUMP] },

    { FLAG_TREE_DUMP,
      "--tree-dump",
      processFlagTreeDump,
      FLAGS_HELP_MESSAGES[FLAG_TREE_DUMP] },

    { FLAG_SYMB_TABLE_DUMP,
      "--symb-table-dump",
      processFlagSymbTableDump,
      FLAGS_HELP_MESSAGES[FLAG_SYMB_TABLE_DUMP] },

    { FLAG_USE_NUMERICS,
      "--numeric",
      processFlagUseNumerics,
      FLAGS_HELP_MESSAGES[FLAG_USE_NUMERICS] },

    { FLAG_HELP,
      "-h",
      processFlagHelp,
      FLAGS_HELP_MESSAGES[FLAG_HELP] },

    { FLAG_OUTPUT,
      "-o",
      processFlagOutput,
      FLAGS_HELP_MESSAGES[FLAG_OUTPUT] },      
};

#include "compiler/x86_64_specification.h"

int main(int argc, const char* argv[])
{
    FlagManager flagManager = {};
    flagManager.argc = argc;
    flagManager.argv = argv;

    if (argc > 1) { flagManager.input = argv[1]; }

    Error flagProcessingResult = processFlags(&flagManager);
    if (flagProcessingResult != NO_ERROR)
    {
        return flagProcessingResult;
    }

    if (flagManager.input == nullptr)
    {
        printf("Input file unspecified!\n");
        return INPUT_UNSPECIFIED;
    }

    if (flagManager.output == nullptr) { flagManager.output = DEFAULT_OUTPUT; }

    return compile(&flagManager);
}

Error processFlags(FlagManager* flagManager) 
{
    assert(flagManager);

    for (int arg = 1; arg < flagManager->argc; arg++)
    {
        for (int flag = 0; flag < TOTAL_FLAGS; flag++)
        {
            FlagSpecification specification = FLAG_SPECIFICATIONS[flag];
            flagManager->curArg = arg;

            if (strcmp(flagManager->argv[arg], specification.string) == 0)
            {
                Error processingResult = specification.process(flagManager);    

                if (processingResult != NO_ERROR)
                {
                    return processingResult;
                }
            }
        }
    }

    return NO_ERROR;
}

Error processFlagTokenDump(FlagManager* flagManager)
{
    assert(flagManager);

    flagManager->tokenDumpEnabled = true;
    return NO_ERROR;
}

Error processFlagSimpleGraphDump(FlagManager* flagManager)
{
    assert(flagManager);

    flagManager->simpleGraphDumpEnabled = true;
    return NO_ERROR;
}

Error processFlagDetailedGraphDump(FlagManager* flagManager)
{
    assert(flagManager);

    flagManager->detailedGraphDumpEnabled = true;
    return NO_ERROR;
}

Error processFlagOpenGraphDump(FlagManager* flagManager)
{
    assert(flagManager);

    flagManager->openGraphDumpEnabled = true;
    return NO_ERROR;
}

Error processFlagTreeDump(FlagManager* flagManager)
{
    assert(flagManager);

    flagManager->treeDumpEnabled = true;
    return NO_ERROR;
}

Error processFlagSymbTableDump(FlagManager* flagManager)
{
    assert(flagManager);

    flagManager->symbTableDumpEnabled = true;
    return NO_ERROR;
}

Error processFlagUseNumerics(FlagManager* flagManager)
{
    assert(flagManager);

    flagManager->useNumerics = true;
    return NO_ERROR;
}

Error processFlagHelp(FlagManager* flagManager)
{
    assert(flagManager);

    printHelp();
    return NO_ERROR;
}

Error processFlagOutput(FlagManager* flagManager)
{
    assert(flagManager);

    if (flagManager->curArg + 1 >= flagManager->argc)
    {
        printf("Output file unspecified!\n");
        return OUTPUT_UNSPECIFIED;
    }

    flagManager->output = flagManager->argv[flagManager->curArg + 1];

    return NO_ERROR;
}

void printHelp()
{
    printf("Simple Harry Potter influenced programming language set. "
           "Works with my software cpu.\n");

    for (uint32_t i = 0; i < TOTAL_FLAGS; i++)
    {
        printf("%s\n%s", FLAG_SPECIFICATIONS[i].string, FLAG_SPECIFICATIONS[i].helpMessage);
    }
}

void makeGraphDump(const FlagManager* flagManager, const Node* tree, bool detailed)
{
    assert(flagManager);
    assert(tree);

    int count = counterFileUpdate("../examples/log/tree_dumps/graph/count.cnt");
       
    char textFilename[MAX_FILENAME_LENGTH] = {};
    snprintf(textFilename, sizeof(textFilename), "%s%u.txt", "../examples/log/tree_dumps/graph/text/tree", count);
    
    char imageFilename[MAX_FILENAME_LENGTH] = {};
    snprintf(imageFilename, sizeof(imageFilename), "%s%u.svg", "../examples/log/tree_dumps/graph/img/tree", count);

    graphDump(tree, textFilename, imageFilename, detailed);

    if (flagManager->openGraphDumpEnabled)
    {
        char dotCmd[MAX_COMMAND_LENGTH] = {};
        snprintf(dotCmd, sizeof(dotCmd), "xdg-open %s", imageFilename);
        system(dotCmd);
    }
}

Error compile(const FlagManager* flagManager)
{
    assert(flagManager);

    const char* input  = flagManager->input;
    const char* output = flagManager->output;

    char*  buffer     = nullptr;
    size_t bufferSize = 0;

    if (!loadFile(input, &buffer, &bufferSize))
    {
        printf("Couldn't load file '%s'\n", input);
        return INPUT_LOAD_FAILED;
    }

    Node* tree = nullptr;

    Tokenizer tokenizer = {};
    construct(&tokenizer, buffer, bufferSize, flagManager->useNumerics);
    tokenizeBuffer(&tokenizer);

    if (flagManager->tokenDumpEnabled)
    {
        FILE* tokensDumpFile = fopen("../examples/log/dumped_tokens.txt", "w");
        assert(tokensDumpFile);

        dumpTokens(&tokenizer, tokensDumpFile);

        fclose(tokensDumpFile);
    }

    SymbolTable table = {};
    construct(&table);

    Parser parser = {};
    construct(&parser, &tokenizer);
    if (parseProgram(&parser, &table, &tree) != PARSE_NO_ERROR)
    {
        printf("Couldn't compile the program.\n");
        return COMPILATION_FAILED;
    }

    if (flagManager->simpleGraphDumpEnabled)
    {
        makeGraphDump(flagManager, tree, false);
    }

    if (flagManager->detailedGraphDumpEnabled)
    {
        makeGraphDump(flagManager, tree, true);
    }

    if (flagManager->symbTableDumpEnabled)
    {   
        printf("\n");
        dump(&table);
        printf("\n");
    }

    if (flagManager->treeDumpEnabled)
    {
        FILE* file = fopen("../examples/log/dumped_tree.txt", "w");
        assert(file);

        dumpToFile(file, tree);

        fclose(file);
    }   

    Compiler compiler = {};
    construct(&compiler, tree, &table);
    if (compile(&compiler, output) != COMPILER_NO_ERROR)
    {
        printf("Couldn't compile the program.\n");
        return COMPILATION_FAILED;
    }

    // FIXME: get rid of
    // FILE* dumpedTreeFile = fopen("dumped_insertion_sort.txt", "w");
    // assert(dumpedTreeFile);
    // dumpToFile(dumpedTreeFile, tree);

    // fclose(dumpedTreeFile);

    destroy(&table);
    destroy(&tokenizer);
    destroy(&parser);
    destroy(&compiler);

    return NO_ERROR;
}