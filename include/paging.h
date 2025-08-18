#ifndef PAGING_H
#define PAGING_H


#include "type.h"


#define PAGE_SIZE               4096
#define PAGE_ALIGN_DOWN(x)      ((uintptr_t)(x) & ~(PAGE_SIZE-1))
#define PAGE_ALIGN_UP(x)        ((uintptr_t)(x) + PAGE_SIZE-1) & ~(PAGE_SIZE-1)

#define P_PRESENT       (1u << 0)
#define P_RW            (1u << 1)
#define P_USER          (1u << 2)
#define P_PWT           (1u << 3)
#define P_PCD           (1u << 4)
#define P_ACCESSED      (1u << 5)
#define P_DIRTY         (1u << 6)
#define PS_PS           (1u << 7)
#define P_GLOBAL        (1u << 8)


typedef uint32_t pte_t;
typedef uint32_t pde_t;

typedef struct page_table {
        pte_t entries[1024];
} __attribute__((packed, aligned(4096))) page_table_t;

typedef struct page_directory {
        pde_t entries[1024];
} __attribute__((packed, aligned(4096))) page_directory_t;


void paging_init_identity_4mb();
void paging_enable(page_directory_t *pd_phys);
void page_fault_isr(uint32_t err_code);
int map_page(page_directory_t *pd, uintptr_t vaddr, uintptr_t paddr, uint32_t flags);
int unmap_page(page_directory_t *pd, uintptr_t vaddr);


#endif
