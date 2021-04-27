#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <stdio.h>

// flags for makeTextCaseHomogeneous
extern const char* TEXT_CASE_FORMAT_UPPER;
extern const char* TEXT_CASE_FORMAT_LOWER;

struct Text;

struct BinFileHeader
{
    short signature = 0;
    short version   = 0;
};

size_t getFileSize             (const char* fileName);
bool   loadFile                (const char* filename, char** buffer, size_t* bufferSize);

Text*  copyText                (Text* text);
Text*  readTextFromFile        (const char* fileName);
bool   writeTextToFile         (FILE* file, Text* text);
void   deleteText              (Text* text);
char*  nextTextLine            (Text* text);
void   resetTextToStart        (Text* text);
size_t getCurrentLineNumber    (Text* text);
char*  getLine                 (Text* text, size_t i);

size_t replaceAllOccurrences   (char* buffer, size_t bufferSize, char target, char replacement);
char*  strToLower              (char* str);
char*  strToUpper              (char* str);
Text*  makeTextCaseHomogeneous (Text* text, const char* flag);

#endif