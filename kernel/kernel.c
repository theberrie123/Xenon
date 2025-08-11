#include "../include/tty.h"
#include "../include/string.h"


extern char __heap_start;
extern char __heap_end;




// root@lithiumiso ~ #
// current logged-in user. On Lithium live ISO, you are logged in as the superuser (root) by default
// hostname of the machine. On Lithium live ISO, the hostname is set to lithiumiso
// current working directory. ~ means the user's home directory
// prompt symbol for the root user. Regular users get $ instead





void kmain()
{
        kinit();

        kprintf("Lithium 1.0.0 (tty)\n\n");
        kprintf("root@lithiumiso ~ # ");

        while (1) {
                asm volatile ("hlt");
        }
}


