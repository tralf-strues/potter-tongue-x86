#ifndef ELF_BUILDER_H
#define ELF_BUILDER_H

#include <assert.h>
#include <elf.h>
#include <stdio.h>

#define ASSERT_ELF_BUILDER(builder) assert(builder->elfFile.file);                               \
                                    assert(builder->elfFile.bytecode);                           \
                                    assert(builder->offset < builder->elfFile.bytecodeCapacity); \

struct ElfFile
{
    FILE*      file;
    Elf64_Ehdr elfHeader;
    Elf64_Phdr textHeader;
    Elf64_Phdr dataHeader;
    Elf64_Phdr bssHeader;

    uint8_t*   bytecode;
    size_t     bytecodeCapacity;
};

struct ElfBuilder
{
    ElfFile  elfFile;
    uint64_t offset;
};  

void construct   (ElfFile* elfFile, FILE* file, size_t initialSize);
void destroy     (ElfFile* elfFile);

void writeBytes  (ElfBuilder* builder, const uint8_t* buffer, size_t size);
void writeByte   (ElfBuilder* builder, uint8_t byte);
void writeUInt16 (ElfBuilder* builder, uint16_t word);
void writeUInt32 (ElfBuilder* builder, uint32_t doubleWord);
void writeUInt64 (ElfBuilder* builder, uint64_t quadWord);

#endif