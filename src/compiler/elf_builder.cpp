#include <stdlib.h>
#include <string.h>
#include "elf_builder.h"

const double   BYTECODE_REALLOC_MULTIPLIER = 1.6;
const uint64_t SEGMENT_ALIGNMENT_SIZE      = 0x1000;

void makeNeededSpace(ElfBuilder* builder, size_t bytesNeeded);
void startNewSegment(ElfBuilder* builder);

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

void construct(ElfBuilder* builder, FILE* file, size_t initialSize)
{
    assert(builder);
    assert(file);
    assert(initialSize > 0);

    builder->elfFile.file = file;
    builder->elfFile.bytecode = (uint8_t*) calloc(initialSize, sizeof(uint8_t));
    assert(builder->elfFile.bytecode);

    builder->elfFile.bytecodeCapacity = initialSize;
    builder->elfFile.elfHeader        = DEFAULT_ELF_HEADER;

    builder->elfFile.textHeader          = DEFAULT_SEGMENT_HEADER;
    builder->elfFile.textHeader.p_flags  = PF_X | PF_R; /* executable and readable */
    builder->elfFile.textHeader.p_offset = FIRST_SEGMENT_OFFSET;
    builder->elfFile.textHeader.p_vaddr  = FIRST_SEGMENT_OFFSET + VIRTUAL_ADDRESS_START;

    builder->elfFile.bssHeader           = DEFAULT_SEGMENT_HEADER;
    builder->elfFile.bssHeader.p_flags   = PF_W | PF_R; /* writable and readable */

    builder->elfFile.dataHeader          = DEFAULT_SEGMENT_HEADER;
    builder->elfFile.dataHeader.p_flags  = PF_R; /* readable */

    makeNeededSpace(builder, FIRST_SEGMENT_OFFSET + 1);
    setBuilderToStart(builder);
}

void destroy(ElfBuilder* builder)
{
    ASSERT_ELF_BUILDER(builder);

    builder->elfFile.file = nullptr;
    if (builder->elfFile.bytecode != nullptr)
    {
        free(builder->elfFile.bytecode);
    }

    builder->elfFile.bytecode         = nullptr;
    builder->elfFile.bytecodeCapacity = 0;
}

void writeElfFile(ElfFile* elfFile, size_t size)
{
    assert(elfFile);
    assert(elfFile->file);

    size_t offset = 0;
    
    memcpy(elfFile->bytecode + offset, (const void*) &elfFile->elfHeader, sizeof(elfFile->elfHeader));
    offset += sizeof(elfFile->elfHeader);

    memcpy(elfFile->bytecode + offset, (const void*) &elfFile->textHeader, sizeof(elfFile->textHeader));
    offset += sizeof(elfFile->textHeader);

    memcpy(elfFile->bytecode + offset, (const void*) &elfFile->bssHeader, sizeof(elfFile->bssHeader));
    offset += sizeof(elfFile->bssHeader);
    
    memcpy(elfFile->bytecode + offset, (const void*) &elfFile->dataHeader, sizeof(elfFile->dataHeader));
    offset += sizeof(elfFile->dataHeader);

    fwrite(elfFile->bytecode, sizeof(uint8_t), size, elfFile->file);
}

void setBuilderToStart(ElfBuilder* builder)
{
    ASSERT_ELF_BUILDER(builder);

    builder->offset = FIRST_SEGMENT_OFFSET;
}

void startNewSegment(ElfBuilder* builder, Elf64_Phdr* newSegment)
{
    ASSERT_ELF_BUILDER(builder);

    size_t newSegmentStart = (builder->offset & ~(PAGE_SIZE - 1)) + PAGE_SIZE;
    newSegment->p_offset   = newSegmentStart;
    newSegment->p_vaddr    = newSegmentStart + VIRTUAL_ADDRESS_START;

    makeNeededSpace(builder, newSegmentStart - builder->offset + 1);
    builder->offset = newSegmentStart;
}

void startTextSegment(ElfBuilder* builder)
{
    ASSERT_ELF_BUILDER(builder);
    
    builder->elfFile.textHeader.p_offset = FIRST_SEGMENT_OFFSET;
    builder->elfFile.textHeader.p_vaddr  = FIRST_SEGMENT_OFFSET + VIRTUAL_ADDRESS_START;

    makeNeededSpace(builder, FIRST_SEGMENT_OFFSET - builder->offset + 1);
    builder->offset = FIRST_SEGMENT_OFFSET;
}

void startBssSegment(ElfBuilder* builder)
{
    ASSERT_ELF_BUILDER(builder);
    startNewSegment(builder, &builder->elfFile.bssHeader);
}

void startDataSegment(ElfBuilder* builder)
{
    ASSERT_ELF_BUILDER(builder);
    startNewSegment(builder, &builder->elfFile.dataHeader);
}

void endCurSegment(ElfBuilder* builder, Elf64_Phdr* curSegment)
{
    ASSERT_ELF_BUILDER(builder);
    assert(builder->offset >= curSegment->p_offset);

    curSegment->p_filesz = builder->offset - curSegment->p_offset;
    curSegment->p_memsz  = builder->offset - curSegment->p_offset;
}

void endTextSegment(ElfBuilder* builder)
{
    ASSERT_ELF_BUILDER(builder);
    endCurSegment(builder, &builder->elfFile.textHeader);
}

void endBssSegment(ElfBuilder* builder)
{
    ASSERT_ELF_BUILDER(builder);
    endCurSegment(builder, &builder->elfFile.bssHeader);
}

void endDataSegment(ElfBuilder* builder)
{
    ASSERT_ELF_BUILDER(builder);
    endCurSegment(builder, &builder->elfFile.dataHeader);
}

void writeBytes(ElfBuilder* builder, const uint8_t* buffer, size_t size)
{
    ASSERT_ELF_BUILDER(builder);

    makeNeededSpace(builder, size);
    memcpy(builder->elfFile.bytecode + builder->offset, buffer, size);
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