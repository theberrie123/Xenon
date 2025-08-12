ENTRY(kmain)

MEMORY
{
    RAM (rwx) : ORIGIN = 0x00100000, LENGTH = 16M
}

SECTIONS
{
    . = 0x00100000;

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
        __heap_start = .;
        . += 0x10000; /* 64 KB heap */
        __heap_end = .;
    } > RAM
}
