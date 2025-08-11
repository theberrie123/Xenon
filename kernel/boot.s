/* align loaded modules on page boundaries */
.set ALIGN, 1<<0
/* provide memory map */
.set MEMINFO, 1<<1
/* Multiboot 'flag' field */
.set FLAGS, ALIGN | MEMINFO
/* 'magic number' lets bootloader find the header */
.set MAGIC, 0x1BADB002
/* checksum of above, to prove we are multiboot */
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

.section .text
.global _start
.type _start, @function
_start:
        /* set up 32-bit stack pointer */
        mov $stack_top, %esp

        call kmain

        cli
halt_loop:
        hlt
        jmp halt_loop

.size _start, . - _start
