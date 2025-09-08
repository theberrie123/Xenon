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

static inline void write_cr3(uintptr_t phys)
{
        __asm__ __volatile__ ("mov %0, %%cr3" :: "r"(phys) : "memory");
}

static inline uintptr_t read_cr2()
{
        uintptr_t cr2;
        __asm__ __volatile__ ("mov %%cr2, %0" : "=r"(cr2));
        return cr2;
}

static inline void enable_paging()
{
        uint32_t cr0;
        __asm__ __volatile__ ("mov %%cr0, %0" : "=r"(cr0));
        cr0 |= 0x80000000u; /* PG bit */
        __asm__ __volatile__ ("mov %0, %%cr0" :: "r"(cr0) : "memory");
}

static inline uintptr_t virt_to_phys(void *v)
{
        return (uintptr_t)v;
}

void paging_enable(page_directory_t *pd_phys)
{
        write_cr3((uintptr_t)pd_phys);
        enable_paging();
}

int map_page(page_directory_t *pd, uintptr_t vaddr, uintptr_t paddr, uint32_t flags)
{
        size_t pd_i = (vaddr >> 22) & 0x3FF;
        size_t pt_i = (vaddr >> 12) & 0x3FF;
        pde_t pde = pd->entries[pd_i];
        if (!(pde & P_PRESENT)) {
                return -1;
        }
        page_table_t *pt = (page_table_t*)(uintptr_t)(pde & ~0xFFFu);
        pt->entries[pt_i] = (pte_t)(PAGE_ALIGN_DOWN(paddr) | (flags | P_PRESENT));
        __asm__ __volatile__ ("invlpg (%0)" :: "r"(vaddr) : "memory");
        return 0;
}

int unmap_page(page_directory_t *pd, uintptr_t vaddr)
{
        size_t pd_i = (vaddr >> 22) & 0x3FF;
        size_t pt_i = (vaddr >> 12) & 0x3FF;
        pde_t pde = pd->entries[pd_i];
        if (!(pde & P_PRESENT)) return -1;
        page_table_t *pt = (page_table_t*)(uintptr_t)(pde & ~0xFFFu);
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
        uintptr_t frame = (uintptr_t)i * PAGE_SIZE;
        first_pt.entries[i] = (pte_t)(frame | P_RW | P_PRESENT); // kernel RW
    }

    kernel_pd.entries[0] = (pde_t)(virt_to_phys(&first_pt) | P_RW | P_PRESENT);

    isr_register_handler(14, page_fault_isr_wrapper);

    paging_enable((page_directory_t*)virt_to_phys(&kernel_pd));

}

void page_fault_isr(uint32_t err_code) {
        uintptr_t fault_addr = read_cr2();

        int present   = !(err_code & 0x1); // 0 = not-present
        int rw        = err_code & 0x2;
        int us        = err_code & 0x4;
        int reserved  = err_code & 0x8;
        int instr_fetch = err_code & 0x10; // if supported

        kprintf("Page fault @%p | %s | %s | %s | %s | %s\n",
                (void*)fault_addr,
                present ? "not-present" : "present",
                rw ? "write" : "read",
                us ? "user" : "kernel",
                reserved ? "reserved" : "ok",
                instr_fetch ? "ifetch" : "data");

        __asm__ __volatile__ ("cli; hlt");
}
