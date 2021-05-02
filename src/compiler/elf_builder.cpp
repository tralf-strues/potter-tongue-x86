#include <stdlib.h>
#include <string.h>
#include "elf_builder.h"

const size_t BYTECODE_REALLOC_MULTIPLIER = 2;

void makeNeededSpace(ElfBuilder* builder, size_t bytesNeeded)
{
    ASSERT_ELF_BUILDER(builder);

    while (builder->offset + bytesNeeded >= builder->elfFile.bytecodeCapacity)
    {
        builder->elfFile.bytecodeCapacity *= BYTECODE_REALLOC_MULTIPLIER;

        uint8_t* newBuffer = (uint8_t*) realloc(builder->elfFile.bytecode, 
                                                builder->elfFile.bytecodeCapacity);
        assert(newBuffer);

        builder->elfFile.bytecode = newBuffer;
    }
}

void construct(ElfFile* elfFile, FILE* file, size_t initialSize)
{
    assert(elfFile);
    assert(file);
    assert(initialSize > 0);

    elfFile->file = file;
    elfFile->bytecode = (uint8_t*) calloc(initialSize, sizeof(uint8_t));
    assert(elfFile->bytecode);

    elfFile->bytecodeCapacity = initialSize;
}

void destroy(ElfFile* elfFile)
{
    assert(elfFile);

    elfFile->file = nullptr;
    if (elfFile->bytecode != nullptr)
    {
        free(elfFile->bytecode);
    }

    elfFile->bytecode         = nullptr;
    elfFile->bytecodeCapacity = 0;
}

void writeBytes(ElfBuilder* builder, const uint8_t* buffer, size_t size)
{
    ASSERT_ELF_BUILDER(builder);

    makeNeededSpace(builder, size);
    memcpy(builder->elfFile.bytecode, buffer, size);
    builder->offset += size;
}

void writeByte(ElfBuilder* builder, uint8_t byte)
{
    ASSERT_ELF_BUILDER(builder);
    writeBytes(builder, &byte, sizeof(byte));
}

void writeUInt16(ElfBuilder* builder, uint16_t word)
{
    ASSERT_ELF_BUILDER(builder);
    writeBytes(builder, (uint8_t*) &word, sizeof(word));
}

void writeUInt32(ElfBuilder* builder, uint32_t doubleWord)
{
    ASSERT_ELF_BUILDER(builder);
    writeBytes(builder, (uint8_t*) &doubleWord, sizeof(doubleWord));
}

void writeUInt64(ElfBuilder* builder, uint64_t quadWord)
{
    ASSERT_ELF_BUILDER(builder);
    writeBytes(builder, (uint8_t*) &quadWord, sizeof(quadWord));
}