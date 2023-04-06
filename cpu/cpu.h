#pragma once
#include <types.h>

struct regs
{
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;    
};

uint8_t inportb (uint16_t _port);
void outportb (uint16_t _port, uint8_t _data);

void gdt_install();
void idt_install();
void isrs_install();
void irq_install();

void idt_set_gate(uint8_t num, unsigned long base, uint16_t sel, uint8_t flags);
void irq_install_handler(int irq, void (*handler)(struct regs *r));

void interupts_on();
void interupts_off();

