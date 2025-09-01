#ifndef ELF_H
#define ELF_H


#include "xenon/type.h"


/* Loads an ELF or flat binary from memory and returns entrypoint.
 * - blob: pointer to file bytes (from initramfs)
 * - blob_size: size of file bytes
 * - entry_out: receives entry virtual address
 * - loaded_mem_size_out: optional; receives total memory consumed/mapped (0 if unknown)
 *
 * Returns 0 on success, nonzero on error.
 *
 * Notes:
 * - This loader copies PT_LOAD segments to their p_vaddr addresses.
 * - Ensure the destination virtual addresses are mapped/writable (or use identity mapping).
 * - If the file is NOT an ELF, it is copied as a flat binary to FLAT_LOAD_ADDR and entry_out = FLAT_LOAD_ADDR.
 */
#define EI_NIDENT 16
#define PT_LOAD   1
#define ELF32_MAGIC0 0x7F
#define ELF32_MAGIC1 'E'
#define ELF32_MAGIC2 'L'
#define ELF32_MAGIC3 'F'

/* Fallback flat load address (inside your linker RAM range). Adjust if you map other addresses. */
#define FLAT_LOAD_ADDR 0x00280000U
#define FLAT_STACK_TOP 0x00300000U

/* ELF32 structures (minimal) */
typedef struct {
    unsigned char e_ident[EI_NIDENT];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint32_t e_entry;
    uint32_t e_phoff;
    uint32_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
} __attribute__((packed)) Elf32_Ehdr;

typedef struct {
    uint32_t p_type;
    uint32_t p_offset;
    uint32_t p_vaddr;
    uint32_t p_paddr;
    uint32_t p_filesz;
    uint32_t p_memsz;
    uint32_t p_flags;
    uint32_t p_align;
} __attribute__((packed)) Elf32_Phdr;

/* Minimal memcpy/memset wrappers if your kernel lacks standard ones */
static void *k_memcpy(void *dst, const void *src, size_t n) {
    uint8_t *d = dst;
    const uint8_t *s = src;
    for (size_t i = 0; i < n; ++i) d[i] = s[i];
    return dst;
}

static void *k_memset(void *s, int c, size_t n) {
    uint8_t *p = s;
    for (size_t i = 0; i < n; ++i) p[i] = (uint8_t)c;
    return s;
}

/* Helper: check if address range [vaddr, vaddr+size) is inside your RAM region.
   This is a conservative check to avoid writing outside kernel-mapped RAM. */
static int is_addr_in_ram(uint32_t vaddr, uint32_t size) {
    const uint32_t RAM_ORIGIN = 0x00200000U;
    const uint32_t RAM_END    = RAM_ORIGIN + (17 * 1024 * 1024); /* 17 MiB */
    if (size == 0) return 0;
    if (vaddr < RAM_ORIGIN) return 0;
    if ((uint64_t)vaddr + (uint64_t)size > (uint64_t)RAM_END) return 0;
    return 1;
}

int elf_loader_load_from_memory(uint8_t *blob, size_t blob_size,
                                uint32_t *entry_out, size_t *loaded_mem_size_out)
{
    if (!blob || blob_size < sizeof(Elf32_Ehdr) || !entry_out) return -1;

    Elf32_Ehdr *eh = (Elf32_Ehdr *)blob;

    /* Quick ELF magic check */
    if ((uint8_t)eh->e_ident[0] == ELF32_MAGIC0 &&
        (uint8_t)eh->e_ident[1] == ELF32_MAGIC1 &&
        (uint8_t)eh->e_ident[2] == ELF32_MAGIC2 &&
        (uint8_t)eh->e_ident[3] == ELF32_MAGIC3) {

        /* Validate basic program header availability */
        if (eh->e_phoff == 0 || eh->e_phnum == 0) return -2;

        /* Iterate program headers and copy PT_LOAD segments */
        Elf32_Phdr *ph = (Elf32_Phdr *)(blob + eh->e_phoff);
        uint32_t highest_addr = 0;
        for (uint16_t i = 0; i < eh->e_phnum; ++i) {
            if (ph[i].p_type != PT_LOAD) continue;
            /* bounds-check p_offset and file size */
            if ((uint64_t)ph[i].p_offset + (uint64_t)ph[i].p_filesz > blob_size) return -3;

            uint32_t dst = ph[i].p_vaddr;
            uint32_t filesz = ph[i].p_filesz;
            uint32_t memsz = ph[i].p_memsz;

            /* Safety: ensure we are writing into kernel RAM (or else caller must map) */
            if (!is_addr_in_ram(dst, memsz)) {
                /* Fail early: target vaddr is not inside RAM region. Caller should map or choose different vaddrs. */
                return -4;
            }

            /* Copy file bytes to p_vaddr */
            k_memcpy((void *)dst, blob + ph[i].p_offset, filesz);

            /* Zero the rest (BSS) */
            if (memsz > filesz) {
                k_memset((void *)(dst + filesz), 0, (size_t)(memsz - filesz));
            }

            /* track highest address used */
            if (dst + memsz > highest_addr) highest_addr = dst + memsz;
        }

        *entry_out = eh->e_entry;
        if (loaded_mem_size_out) *loaded_mem_size_out = (size_t)(highest_addr ? (highest_addr - 0) : 0);
        return 0;
    }

    /* NOT ELF: treat as flat binary. Copy to FLAT_LOAD_ADDR and set entry there. */
    if (!is_addr_in_ram(FLAT_LOAD_ADDR, (uint32_t)blob_size)) {
        return -5; /* flat target not in RAM */
    }

    k_memcpy((void *)FLAT_LOAD_ADDR, blob, blob_size);
    *entry_out = FLAT_LOAD_ADDR;
    if (loaded_mem_size_out) *loaded_mem_size_out = blob_size;
    return 0;
}

#endif
