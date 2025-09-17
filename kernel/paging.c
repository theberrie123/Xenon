#include "paging.h"
#include "tty.h"
#include "isr.h"
#include "kernel.h"

static void page_fault_isr_wrapper(struct regs *regs)
{
        page_fault_isr(regs->err_code);
}

static page_directory_t kernel_pd __attribute__((aligned(4096)));
static page_table_t first_pt __attribute__((aligned(4096)));

static inline void write_cr3(UINT32 phys)
{
        __asm__ __volatile__ ("mov %0, %%cr3" :: "r"(phys) : "memory");
}

static inline UINT32 read_cr2()
{
        UINT32 cr2;
        __asm__ __volatile__ ("mov %%cr2, %0" : "=r"(cr2));
        return cr2;
}

static inline void enable_paging()
{
        UINT32 cr0;
        __asm__ __volatile__ ("mov %%cr0, %0" : "=r"(cr0));
        cr0 |= 0x80000000u;
        __asm__ __volatile__ ("mov %0, %%cr0" :: "r"(cr0) : "memory");
}

static inline UINT32 virt_to_phys(void *v)
{
        return (UINT32)v;
}

void paging_enable(page_directory_t *pd_phys)
{
        write_cr3((UINT32)pd_phys);
        enable_paging();
}

int map_page(page_directory_t *pd, UINT32 vaddr, UINT32 paddr, UINT32 flags)
{
        SIZE pd_i = (vaddr >> 22) & 0x3FF;
        SIZE pt_i = (vaddr >> 12) & 0x3FF;
        pde_t pde = pd->entries[pd_i];
        if (!(pde & P_PRESENT)) {
                return -1;
        }
        page_table_t *pt = (page_table_t*)(UINT32)(pde & ~0xFFFu);
        pt->entries[pt_i] = (pte_t)(PAGE_ALIGN_DOWN(paddr) | (flags | P_PRESENT));
        __asm__ __volatile__ ("invlpg (%0)" :: "r"(vaddr) : "memory");
        return 0;
}

int unmap_page(page_directory_t *pd, UINT32 vaddr)
{
        SIZE pd_i = (vaddr >> 22) & 0x3FF;
        SIZE pt_i = (vaddr >> 12) & 0x3FF;
        pde_t pde = pd->entries[pd_i];
        if (!(pde & P_PRESENT)) return -1;
        page_table_t *pt = (page_table_t*)(UINT32)(pde & ~0xFFFu);
        pt->entries[pt_i] = 0;
        __asm__ __volatile__ ("invlpg (%0)" :: "r"(vaddr) : "memory");
        return 0;
}

void paging_init_identity_4mb(void) {
    for (int i = 0; i < 1024; ++i) {
        kernel_pd.entries[i] = 0;
        first_pt.entries[i]  = 0;
    }

    for (int i = 0; i < 1024; ++i) {
        UINT32 frame = (UINT32)i * PAGE_SIZE;
        first_pt.entries[i] = (pte_t)(frame | P_RW | P_PRESENT);
    }

    kernel_pd.entries[0] = (pde_t)(virt_to_phys(&first_pt) | P_RW | P_PRESENT);

    isr_register_handler(14, page_fault_isr_wrapper);

    paging_enable((page_directory_t*)virt_to_phys(&kernel_pd));

}

void page_fault_isr(UINT32 err_code) {
        UINT32 fault_addr = read_cr2();

        int present = !(err_code & 0x1);        
        int rw = err_code & 0x2;
        int us = err_code & 0x4;
        int reserved = err_code & 0x8;
        int instr_fetch = err_code & 0x10;

        kprintf("Page fault @%p | %s | %s | %s | %s | %s\n",
                (void*)fault_addr,
                present ? "not-present" : "present",
                rw ? "write" : "read",
                us ? "user" : "kernel",
                reserved ? "reserved" : "ok",
                instr_fetch ? "ifetch" : "data");

        __asm__ __volatile__ ("cli; hlt");
}
