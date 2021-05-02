#ifndef ELF_BUILDER_H
#define ELF_BUILDER_H

#include <assert.h>
#include <elf.h>
#include <stdio.h>

#define ASSERT_ELF_BUILDER(builder) assert(builder->elfFile.file);                               \
                                    assert(builder->elfFile.bytecode);                           \
                                    assert(builder->offset < builder->elfFile.bytecodeCapacity); 

struct ElfFile
{
    FILE*      file;
    Elf64_Ehdr elfHeader;
    Elf64_Phdr textHeader;
    Elf64_Phdr bssHeader;
    Elf64_Phdr dataHeader;

    uint8_t*   bytecode;
    size_t     bytecodeCapacity;
};

struct ElfBuilder
{
    ElfFile  elfFile;
    uint64_t offset;
};  

static const size_t USED_SEGMENTS_COUNT   = 3; 
static const size_t VIRTUAL_ADDRESS_START = 0x400000;
static const size_t PAGE_SIZE             = 0x1000; /* 4kB */
static const size_t FIRST_SEGMENT_OFFSET  = PAGE_SIZE;
static const size_t HEADERS_SIZE          = sizeof(Elf64_Ehdr) + 
                                            USED_SEGMENTS_COUNT * sizeof(Elf64_Phdr);

static const Elf64_Ehdr DEFAULT_ELF_HEADER = 
{
    /* Magic number and other info
     * 
     * Byte:  Value:   Description:
     * [ 0]   0x7F     ***
     * [ 1]   0x45 'E'   * Magic 
     * [ 2]   0x4c 'L'   * number
     * [ 3]   0x46 'F' ***
     *                
     * [ 4]   0x02     File class byte index    (64-bit mode)
     * [ 5]   0x01     Data encoding byte index (2's complement, little endian)
     * [ 6]   0x01     Current ELF version
     * [ 7]   0x00
     *   :      :
     * [15]   0x00 
     * */
    .e_ident = {ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3, ELFCLASS64, ELFDATA2LSB, EV_CURRENT, 0x00, 
                0x00,    0x00,    0x00,    0x00,    0x00,       0x00,        0x00,       0x00},

    .e_type      = ET_EXEC,
    .e_machine   = EM_X86_64,
    .e_version   = EV_CURRENT,
    .e_entry     = 0x401000,
    
    .e_phoff     = sizeof(Elf64_Ehdr),
    .e_shoff     = 0,
    .e_flags     = 0,
    .e_ehsize    = sizeof(Elf64_Ehdr),

    .e_phentsize = sizeof(Elf64_Phdr),
    .e_phnum     = USED_SEGMENTS_COUNT, 
    .e_shentsize = 0, 
    .e_shnum     = 0,

    .e_shstrndx  = 0
};

static const Elf64_Phdr DEFAULT_SEGMENT_HEADER = 
{
    .p_type   = PT_LOAD,
    .p_flags  = 0, /* OPTIONAL */

    .p_offset = 0, /* OPTIONAL */ 
    .p_vaddr  = 0, /* OPTIONAL */
    .p_paddr  = 0, 
    .p_filesz = 0, /* OPTIONAL */
    .p_memsz  = 0, /* OPTIONAL */
    .p_align  = 0 
};

void construct         (ElfBuilder* builder, FILE* file, size_t initialSize);
void destroy           (ElfBuilder* builder);
void writeElfFile      (ElfFile* elfFile, size_t size);

void setBuilderToStart (ElfBuilder* builder);
void startTextSegment  (ElfBuilder* builder); 
void startBssSegment   (ElfBuilder* builder); 
void startDataSegment  (ElfBuilder* builder); 
void endTextSegment    (ElfBuilder* builder); 
void endBssSegment     (ElfBuilder* builder); 
void endDataSegment    (ElfBuilder* builder); 

void writeBytes        (ElfBuilder* builder, const uint8_t* buffer, size_t size);
void writeByte         (ElfBuilder* builder, uint8_t byte);
void writeUInt16       (ElfBuilder* builder, uint16_t word);
void writeUInt32       (ElfBuilder* builder, uint32_t doubleWord);
void writeUInt64       (ElfBuilder* builder, uint64_t quadWord);

#endif