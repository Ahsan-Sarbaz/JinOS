#include <types.h>
#include "cpu/cpu.h"
#include "devices/devices.h"
#include "pmm.h"
#include <string/string.h>
#include <memory/memory.h>
#include "multiboot.h"
#include "shell.h"

void kmain(unsigned long magic, uint32_t* mbi)
{
    if(magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        return;
    }

    pmm_init_from_mbi(mbi);

    gdt_install();
    idt_install();
    isrs_install();
    irq_install();
    init_video();

    timer_install();
    keyboard_install();

    interupts_on();
// Read the current time from the RTC

    
    shell_init((uint32_t*)mbi);

    for (;;);
}
