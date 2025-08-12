void timer_tick(void)
{

}

void irq0_handler(void) __attribute__((naked));
void irq0_handler(void) {
    asm volatile (
        "pusha\n\t"               // save registers
        "call timer_tick\n\t"     // call your timer tick function (implement)
        "movb $0x20, %al\n\t"     // send EOI to PIC1
        "outb %al, $0x20\n\t"
        "popa\n\t"
        "iret\n\t"
    );
}


void irq1_handler(void) __attribute__((naked));
void irq1_handler(void) {
    asm volatile(
        "pusha\n\t"
        "call keyboard_interrupt\n\t"  // Your keyboard handler in C
        "movb $0x20, %al\n\t"
        "outb %al, $0x20\n\t"  // Send EOI to master PIC
        "popa\n\t"
        "iret\n\t"
    );
}
