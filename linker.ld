ENTRY(_start)

MEMORY
{
    RAM (rwx) : ORIGIN = 0x00200000, LENGTH = 17M
}

SECTIONS
{
    /* start at RAM origin (use ORIGIN(RAM) so it matches MEMORY) */
    . = ORIGIN(RAM);

    .text :
    {
        *(.multiboot)
        *(.text)
    } > RAM

    .rodata :
    {
        *(.rodata)
    } > RAM

    .data :
    {
        *(.data)
    } > RAM

    .bss :
    {
        *(.bss)
        *(COMMON)
    } > RAM
    
    .heap (NOLOAD) :
    {
        . = ALIGN(8);
        __heap_start = .;
        . += 0x80000; /* 512 KiB heap for testing */
        __heap_end = .;
        . = ALIGN(8);
    } > RAM
}
